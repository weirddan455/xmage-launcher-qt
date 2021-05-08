#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>

class Settings
{
public:
    QSettings diskSettings { "xmage", "xmage-launcher-qt" };
    QString xmageInstallLocation { diskSettings.value("xmageInstallLocation").toString() };
    QString javaInstallLocation { diskSettings.value("javaInstallLocation").toString() };

    void setXmageInstallLocation(QString location);
    void setJavaInstallLocation(QString location);
};

#endif // SETTINGS_H
