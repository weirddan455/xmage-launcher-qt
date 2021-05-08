#include "settings.h"

void Settings::setXmageInstallLocation(QString location)
{
    xmageInstallLocation = location;
    diskSettings.setValue("xmageInstallLocation", location);
}

void Settings::setJavaInstallLocation(QString location)
{
    javaInstallLocation = location;
    diskSettings.setValue("javaInstallLocation", location);
}
