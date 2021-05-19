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
        QString savedVersion = jsonObject.value("version").toString();
        if (savedVersion.isEmpty())
        {
            versionInfo.version = "Unknown";
        }
        else
        {
            versionInfo.version = savedVersion;
        }
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

    defaultClientOptions << "-Xms256m" << "-Xmx512m" << "-Dfile.encoding=UTF-8";
    defaultServerOptions << "-Xms256m" << "-Xmx1g" << "-Dfile.encoding=UTF-8";
    if (diskSettings.contains("clientOptions"))
    {
        currentClientOptions = diskSettings.value("clientOptions").toStringList();
    }
    else
    {
        currentClientOptions = defaultClientOptions;
    }
    if (diskSettings.contains("serverOptions"))
    {
        currentServerOptions = diskSettings.value("serverOptions").toStringList();
    }
    else
    {
        currentServerOptions = defaultServerOptions;
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

void Settings::setClientOptions(QString options)
{
    currentClientOptions = stringToList(options);
    diskSettings.setValue("clientOptions", currentClientOptions);
}

void Settings::setServerOptions(QString options)
{
    currentServerOptions = stringToList(options);
    diskSettings.setValue("serverOptions", currentServerOptions);
}

QStringList Settings::stringToList(QString str)
{
    QStringList list;
    const QChar *data = str.constData();
    for (int i = 0; i < str.size(); i++)
    {
        QString item;
        while (i < str.size() && data[i] != ' ')
        {
            item.append(data[i]);
            i++;
        }
        if (!item.isEmpty())
        {
            list.append(item);
        }
    }
    return list;
}
