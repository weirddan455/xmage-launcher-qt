#include "downloadmanager.h"

DownloadManager::DownloadManager(QString downloadLocation, MainWindow *mainWindow)
    : QObject(mainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    connect(networkManager, &QNetworkAccessManager::finished, this, &DownloadManager::poll_github);
    this->downloadLocation = downloadLocation;
    this->mainWindow = mainWindow;
}

DownloadManager::~DownloadManager()
{
    delete networkManager;
}

void DownloadManager::download()
{
    mainWindow->log("Polling GitHub for latest version...");
    QNetworkRequest request(QUrl("https://api.github.com/repos/magefree/mage/releases"));
    networkManager->get(request);
}

void DownloadManager::poll_github(QNetworkReply *reply)
{
    bool fail = false;
    if (reply->error() != QNetworkReply::NoError)
    {
        fail = true;
        mainWindow->log("Network error: " + reply->errorString());
        mainWindow->downloadComplete();
    }
    else
    {
        QJsonObject latestRelease = QJsonDocument::fromJson(reply->readAll()).array().first().toObject();
        QString latestXmageVersion = latestRelease.value("name").toString();
        QUrl url(latestRelease.value("assets").toArray().first().toObject().value("browser_download_url").toString());
        if (!url.isValid())
        {
            fail = true;
            mainWindow->log("Error parsing JSON");
            mainWindow->downloadComplete();
        }
        else
        {
            QString fileName;
            if (!downloadLocation.isEmpty())
            {
                fileName.append(downloadLocation + '/');
            }
            if (!latestXmageVersion.isEmpty())
            {
                fileName.append(latestXmageVersion + ".zip");
            }
            else
            {
                fileName.append("xmage.zip");
            }
            saveFile = new QSaveFile(fileName);
            if (!saveFile->open(QIODevice::WriteOnly))
            {
                fail = true;
                mainWindow->log("Failed to create file " + saveFile->fileName());
                mainWindow->downloadComplete();
                delete saveFile;
            }
            else
            {
                mainWindow->log("Downloading XMage from " + url.toString());
                networkManager->disconnect();
                connect(networkManager, &QNetworkAccessManager::finished, this, &DownloadManager::download_complete);
                QNetworkRequest request(url);
                request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
                downloadReply = networkManager->get(request);
                connect(downloadReply, &QNetworkReply::downloadProgress, mainWindow, &MainWindow::update_progress_bar);
                connect(downloadReply, &QNetworkReply::readyRead, this, &DownloadManager::save_data);
            }
        }
    }
    reply->deleteLater();
    if (fail)
    {
        this->deleteLater();
    }
}

void DownloadManager::save_data()
{
    if (!saveFile->write(downloadReply->readAll()))
    {
        mainWindow->log("Error writing to file " + saveFile->fileName());
        mainWindow->downloadComplete();
        delete saveFile;
        downloadReply->deleteLater();
        this->deleteLater();
    }
}

void DownloadManager::download_complete(QNetworkReply *reply)
{
    bool fail = false;
    QString fileName(saveFile->fileName());
    if (reply->error() != QNetworkReply::NoError)
    {
        fail = true;
        mainWindow->log("Network error: " + reply->errorString());
    }
    else
    {
        saveFile->write(reply->readAll());
        if (saveFile->commit())
        {
            mainWindow->log("Download complete");
        }
        else
        {
            fail = true;
            mainWindow->log("Error writing to file " + fileName);
        }
    }
    delete saveFile;
    reply->deleteLater();
    this->deleteLater();
    if (!fail)
    {
        UnzipThread *unzip = new UnzipThread(fileName);
        connect(unzip, &UnzipThread::finished, mainWindow, &MainWindow::downloadComplete);
        connect(unzip, &UnzipThread::finished, unzip, &QObject::deleteLater);
        connect(unzip, &UnzipThread::log, mainWindow, &MainWindow::log);
        connect(unzip, &UnzipThread::progress, mainWindow, &MainWindow::update_progress_bar);
        unzip->start();
    }
    else
    {
        mainWindow->downloadComplete();
    }
}
