
#pragma once

#include <QDBusArgument>
#include <QString>

class AppInfo
{
public:
    QString appId;
    QString appName;
    QString appVer;
    QString appArch;
    QString appDesc;
    QString appUrl;
    QString runtime;
    QString repoName;
};

inline QDBusArgument &operator<<(QDBusArgument &argument, const AppInfo &message)
{
    argument.beginStructure();
    argument << message.appId;
    argument << message.appName;
    argument << message.appVer;
    argument << message.appArch;
    argument << message.appDesc;
    argument << message.appUrl;
    argument << message.runtime;
    argument << message.repoName;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, AppInfo &message)
{
    argument.beginStructure();
    argument >> message.appId;
    argument >> message.appName;
    argument >> message.appVer;
    argument >> message.appArch;
    argument >> message.appDesc;
    argument >> message.appUrl;
    argument >> message.runtime;
    argument >> message.repoName;
    argument.endStructure();
    return argument;
}

typedef QList<AppInfo> AppInfoList;

Q_DECLARE_METATYPE(AppInfo)

Q_DECLARE_METATYPE(AppInfoList)

typedef QMap<QString, QString> ParamStringMap;
Q_DECLARE_METATYPE(ParamStringMap)