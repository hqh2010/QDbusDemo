#include "AppMeta/AppInfo.h"

class AppManager;

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

