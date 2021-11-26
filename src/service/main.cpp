/*
 * Copyright (c) 2020-2021. xxxxxx Software Ltd. All rights reserved.
 *
 * Author:     xxxxxx <xxxxxx@163.com>
 *
 * Maintainer: xxxxxx <xxxxxx@163.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QDateTime>

#include "utils/RegisterDbusType.h"
#include "appmanageradaptor.h"

bool dirExists(const QString &path)
{
    QFileInfo fs(path);
    return fs.exists() && fs.isDir() ? true : false;
}

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("[Debug]");
        break;
    case QtInfoMsg:
        text = QString("[Info]");
        break;
    case QtWarningMsg:
        text = QString("[Warning]");
        break;
    case QtCriticalMsg:
        text = QString("[Critical]");
        break;
    case QtFatalMsg:
        text = QString("[Fatal]");
    }

    QString context_info =
        QString("[File:%1 Line:%2 Func:%3]").arg(QString(context.file)).arg(context.line).arg(context.function);

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    QString current_date = QString("%1").arg(current_date_time);

    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(text).arg(context_info).arg(msg);

    QFile file("/tmp/debug/ostreehelp_log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << message << "\n";
    file.flush();
    file.close();
    mutex.unlock();
}

int main(int argc, char **argv)
{
    // 打开日志的时间戳文件行开关
    if (dirExists("/tmp/debug")) {
        qInstallMessageHandler(outputMessage);
    } else {
        qSetMessagePattern(
            "%{time yyyy-MM-dd hh:mm:ss.zzz} [%{type}] [File:%{file} Line:%{line} Function:%{function}] %{message}");
    }
    qInfo() << "qdbus service start";
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("QTDbusDemo");
    RegisterDbusType();
    // 建立到session bus的连接
    QDBusConnection dbus = QDBusConnection::sessionBus();
    // 在session bus上注册名为com.demo.qdbus的服务
    if (!dbus.registerService("com.demo.qdbus")) {
        qCritical() << "registerService err, errinfo:" << dbus.lastError().message();
        return -1;
    }
    AppManager am;
    dbus.registerObject("/com/demo/qdbus/AppManager", &am, QDBusConnection::ExportAllSlots);
    // 注册名为/test/objects的对象，把类Object所有槽函数导出为object的method
    // dbus.registerObject("/com/demo/qdbus/AppManager", &object, QDBusConnection::ExportAllSlots);
    return QCoreApplication::exec();
}
