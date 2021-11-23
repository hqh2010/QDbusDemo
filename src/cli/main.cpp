/*
 * Copyright (c) 2020-2021. xxxxxx Software Ltd. All rights reserved.
 *
 * Author:     xxxxxx <xxxxxx@163.com>
 *
 * Maintainer: xxxxxx <xxxxxx@163.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iomanip>
#include <iostream>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDBusPendingReply>
#include <QDebug>

// cmake 里面添加了module的路径
#include "utils/RegisterDbusType.h"
// 注意　这里包含的是生成的AppManager.h文件
#include "AppManager.h"

using namespace std;

void printAppInfo(AppInfoList retMsg)
{
    if (retMsg.size() > 0) {
        std::cout << std::setiosflags(std::ios::left) << std::setw(24) << "id" << std::setw(16)
                  << "name" << std::setw(16) << "version" << std::setw(12) << "arch"
                  << "description" << std::endl;
        // 最长显示字符数
        const int maxDisSize = 50;
        for (auto const &it : retMsg) {
            QString simpleDescription = it.appDesc;
            if (it.appDesc.length() > maxDisSize) {
                simpleDescription = it.appDesc.left(maxDisSize) + "...";
            }
            std::cout << std::setiosflags(std::ios::left) << std::setw(24) << it.appId.toStdString()
                      << std::setw(16) << it.appName.toStdString() << std::setw(16) << it.appVer.toStdString()
                      << std::setw(12) << it.appArch.toStdString() << simpleDescription.toStdString() << std::endl;
        }
    } else {
        std::cout << "app not found in repo" << std::endl;
    }
}

int main(int argc, char **argv)
{
    // 使用环境变量控制打印信息输出
    if (QString(qgetenv("LL_BUILDER_CONSOLE_LOG_ENABLE")) == QString("true")) {
        qputenv("QT_LOGGING_RULES", "*=true");
    } else {
        qputenv("QT_LOGGING_RULES", "*=false");
    }

    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("QTDbusDemo");
    RegisterDbusType();

    QCommandLineParser parser;
    parser.addHelpOption();

    QStringList subCommandList = {"query", "list"};
    parser.addPositionalArgument("subcommand", subCommandList.join("\n"), "subcommand [sub-option]");
    parser.parse(QCoreApplication::arguments());

    QStringList args = parser.positionalArguments();
    QString command = args.isEmpty() ? QString() : args.first();

    ComDemoQdbusAppManagerInterface pm("com.demo.qdbus", "/com/demo/qdbus/AppManager", QDBusConnection::sessionBus());

    QMap<QString, std::function<int(QCommandLineParser & parser)>> subcommandMap = {
        {"query",
         [&](QCommandLineParser &parser) -> int {
             parser.clearPositionalArguments();
             parser.addPositionalArgument("query", "query app info", "query");
             parser.addPositionalArgument("app-id", "app id", "com.deepin.demo");
             auto optVersion = QCommandLineOption("version", "special app version to query", "--version=1.0.0", "");
             parser.addOption(optVersion);
             parser.process(app);
             auto version = parser.value(optVersion);
             QMap<QString, QString> paramMap;
             if (!version.isEmpty()) {
                 paramMap.insert("version", version);
             }
             auto args = parser.positionalArguments();
             auto appID = args.value(1);
             QDBusPendingReply<AppInfoList> reply = pm.Query(appID, paramMap);
             AppInfoList retMsg = reply.value();
             printAppInfo(retMsg);
             return 0;
         }},
    };

    if (subcommandMap.contains(command)) {
        auto subcommand = subcommandMap[command];
        return subcommand(parser);
    } else {
        parser.showHelp();
    }
}
