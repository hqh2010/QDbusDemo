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

#include "utils/RegisterDbusType.h"
#include "appmanageradaptor.h"

int main(int argc, char **argv)
{
    // 打开日志的时间戳文件行开关
    qSetMessagePattern("%{time yyyy-MM-dd hh:mm:ss.zzz} [%{type}] [File:%{file} Line:%{line} Function:%{function}] %{message}");
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
