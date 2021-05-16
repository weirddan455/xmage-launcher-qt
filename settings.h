#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QSettings>

enum XMageBranch { UNKNOWN, STABLE, BETA };

struct XMageVersion
{
    QString version;
    XMageBranch branch;
};

Q_DECLARE_METATYPE(XMageVersion);

class Settings
{
public:
    Settings();
    QSettings diskSettings { "xmage", "xmage-launcher-qt" };
    QString xmageInstallLocation { diskSettings.value("xmageInstallLocation").toString() };
    QString javaInstallLocation { diskSettings.value("javaInstallLocation").toString() };
    QMap<QString, XMageVersion> xmageInstallations;

    void addXmageInstallation(QString location, XMageVersion &version);
    void removeXmageInstallaion(QString location);
    void setXmageInstallLocation(QString location);
    void setJavaInstallLocation(QString location);

private:
    void saveXmageInstallation();
};

#endif // SETTINGS_H
