/*
 * Copyright (c) 2020-2021. xxxxxx Software Ltd. All rights reserved.
 *
 * Author:     xxxxxx <xxxxxx@163.com>
 *
 * Maintainer: xxxxxx <xxxxxx@163.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDebug>

#include "AppManagerPrivate.h"
#include "AppManager.h"

AppManager::AppManager()
    : dd_ptr(new AppManagerPrivate(this))
{
}

AppManager::~AppManager() = default;

/*
 * 查询软件包
 *
 * @param appID: 软件包的appid
 * @param paramMap: 命令参数列表
 *
 * @return AppInfoList 查询结果列表
 */
AppInfoList AppManager::Query(const QString &appID, const ParamStringMap &paramMap)
{
    Q_D(AppManager);
    qInfo() << "service Query called";
    return d->QueryImpl(appID, paramMap);
}
