/*
 * Copyright (c) 2020-2021. xxxxxx Software Ltd. All rights reserved.
 *
 * Author:     xxxxxx <xxxxxx@163.com>
 *
 * Maintainer: xxxxxx <xxxxxx@163.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "AppMeta/AppInfo.h"

#include "AppManager.h"

class AppManagerPrivate
{
public:
    explicit AppManagerPrivate(AppManager *parent)
        : q_ptr(parent)
    {
    }
    AppManager *q_ptr = nullptr;

    AppInfoList QueryImpl(const QString &appID, const ParamStringMap &paramMap = {});
};

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

/*
 * 查询系统架构
 *
 * @return QString: 系统架构字符串
 */
QString getHostArch()
{
    const QString arch = QSysInfo::currentCpuArchitecture();
    // Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (arch.startsWith("x86_64", Qt::CaseInsensitive)) {
        return "x86_64";
    }
    if (arch.startsWith("arm", Qt::CaseInsensitive)) {
        return "arm64";
    }
    if (arch.startsWith("mips", Qt::CaseInsensitive)) {
        return "mips64";
    }
    return "unknown";
}

QString getUserName()
{
    // QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    // QString userName = userPath.section("/", -1, -1);
    // return userName;
    uid_t uid = geteuid();
    struct passwd *user = getpwuid(uid);
    QString userName = "";
    if (user && user->pw_name) {
        userName = QString(QLatin1String(user->pw_name));
    } else {
        qInfo() << "getUserName err";
    }
    return userName;
}

bool inline fileExists(const QString &path)
{
    QFileInfo fs(path);
    return fs.exists() && fs.isFile() ? true : false;
}

/*
 * 更新应用安装状态到本地文件
 *
 * @param appStreamPkgInfo: 安装成功的软件包信息
 *
 * @return bool: true:成功 false:失败
 */
bool updateAppStatus(AppInfo appStreamPkgInfo)
{
    // file lock to do
    // 数据库的文件路径
    QString dbPath = "../data/AppInfoDB.json";
    QFile dbFile(dbPath);
    // 首次安装
    if (fileExists(dbPath)) {
        dbFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QJsonObject jsonObject;
        // QJsonArray emptyArray;
        QJsonObject emptyObject;
        // jsonObject.insert("pkgs", emptyArray);
        jsonObject.insert("pkgs", emptyObject);
        jsonObject.insert("users", emptyObject);
        QJsonDocument jsonDocTmp;
        jsonDocTmp.setObject(jsonObject);
        dbFile.write(jsonDocTmp.toJson());
        dbFile.close();
    }

    if (!dbFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "db file open failed!";
        return false;
    }
    // 读取文件的全部内容
    QString qValue = dbFile.readAll();
    dbFile.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(qValue.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        qCritical() << "updateAppStatus parse json file err";
        return false;
    }

    QJsonObject jsonObject = document.object();
    QJsonObject pkgsObject = jsonObject["pkgs"].toObject();
    QJsonObject appItemValue;
    appItemValue.insert("name", appStreamPkgInfo.appName);
    appItemValue.insert("arch", appStreamPkgInfo.appArch);
    appItemValue.insert("summary", appStreamPkgInfo.appDesc);
    appItemValue.insert("runtime", appStreamPkgInfo.runtime);
    appItemValue.insert("reponame", appStreamPkgInfo.repoName);
    QJsonArray userArray;
    // different user install same app to do fix
    QString userName = getUserName();
    userArray.append(userName);
    appItemValue.insert("users", userArray);
    pkgsObject.insert(appStreamPkgInfo.appId, appItemValue);

    QJsonObject usersObject = jsonObject["users"].toObject();
    QJsonObject usersSubItem;
    usersSubItem.insert("ref", "app:" + appStreamPkgInfo.appId);
    // usersSubItem.insert("commitv", "0123456789");
    QJsonObject userItem;

    // users下用户名已存在,将软件包添加到该用户名对应的软件包列表中
    QJsonObject userNameObject;
    QJsonArray versionArray;
    if (usersObject.contains(userName)) {
        userNameObject = usersObject[userName].toObject();
        // 首次安装该应用
        if (!userNameObject.contains(appStreamPkgInfo.appId)) {
            versionArray.append(appStreamPkgInfo.appVer);
            usersSubItem.insert("version", versionArray);
            userNameObject.insert(appStreamPkgInfo.appId, usersSubItem);
        } else {
            QJsonObject usersPkgObject = userNameObject[appStreamPkgInfo.appId].toObject();
            QJsonValue arrayValue = usersPkgObject.value(QStringLiteral("version"));
            versionArray = arrayValue.toArray();
            versionArray.append(appStreamPkgInfo.appVer);
            usersPkgObject["version"] = versionArray;
            userNameObject[appStreamPkgInfo.appId] = usersPkgObject;
        }
        usersObject[userName] = userNameObject;
    } else {
        versionArray.append(appStreamPkgInfo.appVer);
        usersSubItem.insert("version", versionArray);
        userItem.insert(appStreamPkgInfo.appId, usersSubItem);
        usersObject.insert(userName, userItem);
    }

    jsonObject.insert("pkgs", pkgsObject);
    jsonObject.insert("users", usersObject);

    document.setObject(jsonObject);
    dbFile.open(QIODevice::WriteOnly | QIODevice::Text);
    // 将修改后的内容写入文件
    QTextStream wirteStream(&dbFile);
    // 设置编码UTF8
    wirteStream.setCodec("UTF-8");
    wirteStream << document.toJson();
    dbFile.close();
    return true;
}

/*
 * 根据AppStream.json查询目标软件包信息
 *
 * @param savePath: AppStream.json文件存储路径
 * @param remoteName: 远端仓库名称
 * @param pkgName: 软件包包名
 * @param pkgVer: 软件包版本
 * @param pkgArch: 软件包对应的架构
 * @param pkgInfo: 查询结果
 * @param err: 错误信息
 *
 * @return bool: true:成功 false:失败
 */
bool getAppInfoByAppStream(const QString &savePath, const QString &remoteName, const QString &pkgName,
                           const QString &pkgVer, const QString &pkgArch, AppInfo &pkgInfo, QString &err)
{
    // resource 资源文件
    QString fullPath = ":/AppStream.json";
    // absolute path test ok
    // QString fullPath = "/deepin/linglong/repo/AppStream.json";
    // 判断文件是否存在
    if (!fileExists(fullPath)) {
        err = fullPath + " is not exist";
        return false;
    }
    // 读的是当前二进制的路径
    // char curpath[512] = {'\0'};
    // getcwd(curpath, 512);
    // QString curPath = QDir::currentPath();
    QFile jsonFile(fullPath);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString qValue = jsonFile.readAll();
    jsonFile.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(qValue.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        qCritical() << "getAppInfoByAppStream parse json file wrong";
        err = fullPath + " json file wrong";
        return false;
    }

    // 自定义软件包信息metadata 继承JsonSerialize 来处理，to do fix
    QJsonObject jsonObject = document.object();
    if (jsonObject.size() == 0) {
        err = fullPath + " is empty";
        return false;
    }

    // 查找指定版本和架构的软件包是否存在
    if (!pkgVer.isEmpty()) {
        QString appKey = pkgName + "_" + pkgVer;
        if (!jsonObject.contains(appKey)) {
            err = pkgName + "-" + pkgVer + " not found";
            return false;
        }

        QJsonObject subObj = jsonObject[appKey].toObject();
        // 判断指定架构是否存在 to do fix optimized code
        QJsonValue arrayValue = subObj.value(QStringLiteral("arch"));
        if (arrayValue.isArray()) {
            QJsonArray arr = arrayValue.toArray();
            bool flag = false;
            for (int i = 0; i < arr.size(); i++) {
                QString item = arr.at(i).toString();
                if (item == pkgArch) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                err = pkgName + "-" + pkgArch + " not support";
                return false;
            }
        }
        pkgInfo.appId = subObj["appid"].toString();
        pkgInfo.appName = subObj["name"].toString();
        pkgInfo.appVer = subObj["version"].toString();
        pkgInfo.appUrl = subObj["appUrl"].toString();
        pkgInfo.appDesc = subObj["summary"].toString();
        pkgInfo.runtime = subObj["runtime"].toString();
        pkgInfo.repoName = subObj["reponame"].toString();
        pkgInfo.appArch = pkgArch;
        return true;
    }

    QStringList pkgsList = jsonObject.keys();
    QString filterString = ".*" + pkgName + ".*";
    QStringList appList = pkgsList.filter(QRegExp(filterString, Qt::CaseInsensitive));
    if (appList.isEmpty()) {
        err = "app:" + pkgName + " not found";
        qInfo() << err;
        return false;
    }
    QString appKey = appList.at(0);
    // QMap<QString, QString> verMap;
    // for (QString key : appList) {
    //     QJsonObject tmp = jsonObject[key].toObject();
    //     QString value = tmp["version"].toString();
    //     verMap.insert(key, value);
    // }
    // QString appKey = getLatestAppInfo(verMap);
    // qInfo() << "latest appKey:" << appKey;
    // if (!jsonObject.contains(appKey)) {
    //     err = "getLatestAppInfo err";
    //     return false;
    // }
    QJsonObject subObj = jsonObject[appKey].toObject();
    // 判断指定架构是否存在  to do fix optimized code
    QJsonValue arrayValue = subObj.value(QStringLiteral("arch"));
    if (arrayValue.isArray()) {
        QJsonArray arr = arrayValue.toArray();
        bool flag = false;
        for (int i = 0; i < arr.size(); i++) {
            QString item = arr.at(i).toString();
            if (item == pkgArch) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            err = pkgName + "-" + pkgArch + " not support";
            return false;
        }
    }
    pkgInfo.appId = subObj["appid"].toString();
    pkgInfo.appName = subObj["name"].toString();
    pkgInfo.appVer = subObj["version"].toString();
    pkgInfo.appUrl = subObj["appUrl"].toString();
    pkgInfo.appDesc = subObj["summary"].toString();
    pkgInfo.runtime = subObj["runtime"].toString();
    pkgInfo.repoName = subObj["reponame"].toString();
    pkgInfo.appArch = pkgArch;
    return true;
}

/*
 * 查询未安装软件包信息
 *
 * @param pkgName: 软件包包名
 * @param pkgVer: 软件包版本号
 * @param pkgArch: 软件包对应的架构
 * @param pkgList: 查询结果
 * @param err: 错误信息
 *
 * @return bool: true:成功 false:失败
 */
bool getUnInstalledAppInfo(const QString &pkgName, const QString &pkgVer, const QString &pkgArch, AppInfoList &pkgList,
                           QString &err)
{
    AppInfo appStreamPkgInfo;
    bool ret = getAppInfoByAppStream("/deepin/linglong/", "repo", pkgName, pkgVer, pkgArch, appStreamPkgInfo, err);
    if (ret) {
        // auto info = QPointer<AppInfo>(new AppInfo);
        AppInfo info;
        info.appId = appStreamPkgInfo.appId;
        info.appName = appStreamPkgInfo.appName;
        info.appVer = appStreamPkgInfo.appVer;
        info.appArch = appStreamPkgInfo.appArch;
        info.appDesc = appStreamPkgInfo.appDesc;
        pkgList.push_back(info);
    }
    return ret;
}

/*
 * 查询软件包
 *
 * @param appID: 软件包的appid
 * @param paramMap: 命令参数列表
 *
 * @return AppInfoList 查询结果列表
 */
AppInfoList AppManagerPrivate::QueryImpl(const QString &appID, const ParamStringMap &paramMap)
{
    AppInfoList pkglist;
    QString err = "";
    QString arch = getHostArch();
    bool ret = getUnInstalledAppInfo(appID, "", arch, pkglist, err);
    if (!ret) {
        qCritical() << "getUnInstalledAppInfo err, message:" << err;
    }
    return pkglist;
}
