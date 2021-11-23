#include <QDBusArgument>
#include <QDBusContext>
#include <QList>
#include <QObject>
#include <QScopedPointer>

#include "utils/RegisterDbusType.h"

// include "AppManagerPrivate.h"
class AppManagerPrivate;
class AppManager : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.demo.qdbus.AppManager")

public Q_SLOTS:

    AppInfoList Query(const QString &appID, const ParamStringMap &paramMap = {});
// protected:
public:
    AppManager();
    ~AppManager() override;

private:
    QScopedPointer<AppManagerPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), AppManager)
};
