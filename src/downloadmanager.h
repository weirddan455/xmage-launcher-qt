#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "mainwindow.h"
#include "settings.h"
#include "unzipthread.h"

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    DownloadManager(QString downloadLocation, MainWindow *mainWindow);
    ~DownloadManager();
    void downloadStable();
    void downloadBeta();
    void updateXmage(XMageVersion versionInfo);

private:
    bool update = false;
    QString downloadLocation;
    XMageVersion currentVersion;
    XMageVersion newVersion;
    MainWindow *mainWindow;
    QNetworkAccessManager *networkManager;
    QNetworkReply *downloadReply;
    QSaveFile *saveFile = nullptr;

    void pollFailed(QNetworkReply *reply, QString errorMessage);
    void startDownload(QUrl url, QNetworkReply *reply);

private slots:
    void poll_github(QNetworkReply *reply);
    void poll_beta(QNetworkReply *reply);
    void download_complete(QNetworkReply *reply);
    void save_data();
};

#endif // DOWNLOADMANAGER_H
