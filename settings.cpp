#include "settings.h"

Settings::Settings()
{
    qRegisterMetaType<XMageVersion>();
    QJsonArray jsonArray = diskSettings.value("xmageInstallations").toJsonArray();
    for (int i = 0; i < jsonArray.size(); i++)
    {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        QString location = jsonObject.value("installLocation").toString();
        XMageVersion versionInfo;
        versionInfo.version = jsonObject.value("version").toString();
        QString branch = jsonObject.value("branch").toString();
        if (branch == "stable")
        {
            versionInfo.branch = STABLE;
        }
        else if (branch == "beta")
        {
            versionInfo.branch = BETA;
        }
        else
        {
            versionInfo.branch = UNKNOWN;
        }
        xmageInstallations.insert(location, versionInfo);
    }
}

void Settings::addXmageInstallation(QString location, XMageVersion &versionInfo)
{
    xmageInstallations.insert(location, versionInfo);
    saveXmageInstallation();
}

void Settings::removeXmageInstallaion(QString location)
{
    if (xmageInstallations.remove(location) != 0)
    {
        saveXmageInstallation();
    }
}

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

void Settings::saveXmageInstallation()
{
    QJsonArray jsonArray;
    QMapIterator<QString, XMageVersion> i(xmageInstallations);
    while (i.hasNext())
    {
        i.next();
        const XMageVersion &curValue = i.value();
        QJsonObject jsonObject;
        jsonObject.insert("installLocation", i.key());
        jsonObject.insert("version", curValue.version);
        switch (curValue.branch)
        {
            case STABLE:
                jsonObject.insert("branch", "stable");
                break;
            case BETA:
                jsonObject.insert("branch", "beta");
                break;
            default:
                jsonObject.insert("branch", "unknown");
                break;
        }
        jsonArray.append(jsonObject);
    }
    diskSettings.setValue("xmageInstallations", jsonArray);
}
