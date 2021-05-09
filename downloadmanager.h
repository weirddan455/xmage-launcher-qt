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
#include "unzipthread.h"

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    DownloadManager(QString downloadLocation, MainWindow *mainWindow);
    ~DownloadManager();
    void download();

private:
    QString downloadLocation;
    MainWindow *mainWindow;
    QNetworkAccessManager *networkManager;
    QNetworkReply *downloadReply;
    QSaveFile *saveFile;

private slots:
    void poll_github(QNetworkReply *reply);
    void download_complete(QNetworkReply *reply);
    void save_data();
};

#endif // DOWNLOADMANAGER_H
