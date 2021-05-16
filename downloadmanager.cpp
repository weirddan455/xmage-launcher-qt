#include "downloadmanager.h"

DownloadManager::DownloadManager(QString downloadLocation, MainWindow *mainWindow)
    : QObject(mainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    this->downloadLocation = downloadLocation;
    this->mainWindow = mainWindow;
}

DownloadManager::~DownloadManager()
{
    delete networkManager;
}

void DownloadManager::downloadStable()
{
    versionInfo.branch = STABLE;
    connect(networkManager, &QNetworkAccessManager::finished, this, &DownloadManager::poll_github);
    mainWindow->log("Polling GitHub for latest stable version...");
    QNetworkRequest request(QUrl("https://api.github.com/repos/magefree/mage/releases"));
    networkManager->get(request);
}

void DownloadManager::downloadBeta()
{
    versionInfo.branch = BETA;
    connect(networkManager, &QNetworkAccessManager::finished, this, &DownloadManager::poll_beta);
    mainWindow->log("Polling xmage.today for latest beta version...");
    QNetworkRequest request(QUrl("http://xmage.today/config.json"));
    networkManager->get(request);
}

void DownloadManager::poll_github(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        pollFailed(reply, "Network error: " + reply->errorString());
    }
    else
    {
        QJsonObject latestRelease = QJsonDocument::fromJson(reply->readAll()).array().first().toObject();
        QUrl url(latestRelease.value("assets").toArray().first().toObject().value("browser_download_url").toString());
        if (url.isValid())
        {
            versionInfo.version = latestRelease.value("name").toString();
            startDownload(url, reply);
        }
        else
        {
            pollFailed(reply, "Error parsing JSON");
        }
    }
}

void DownloadManager::poll_beta(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        pollFailed(reply, "Network error: " + reply->errorString());
    }
    else
    {
        QJsonObject betaInfo = QJsonDocument::fromJson(reply->readAll()).object().value("XMage").toObject();
        QUrl url(betaInfo.value("location").toString());
        if (url.isValid())
        {
            versionInfo.version = betaInfo.value("version").toString();
            startDownload(url, reply);
        }
        else
        {
            pollFailed(reply, "Error parsing JSON");
        }
    }
}

void DownloadManager::pollFailed(QNetworkReply *reply, QString errorMessage)
{
    mainWindow->download_fail(errorMessage);
    reply->deleteLater();
    this->deleteLater();
}

void DownloadManager::startDownload(QUrl url, QNetworkReply *reply)
{
    QString fileName;
    if (!downloadLocation.isEmpty())
    {
        fileName.append(downloadLocation + '/');
    }
    if (!versionInfo.version.isEmpty())
    {
        fileName.append(versionInfo.version + ".zip");
    }
    else
    {
        fileName.append("xmage.zip");
    }
    saveFile = new QSaveFile(fileName);
    if (!saveFile->open(QIODevice::WriteOnly))
    {
        pollFailed(reply, "Failed to create file " + saveFile->fileName());
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
        reply->deleteLater();
    }
}

void DownloadManager::save_data()
{
    if (!saveFile->write(downloadReply->readAll()))
    {
        mainWindow->download_fail("Error writing to file " + saveFile->fileName());
        delete saveFile;
        downloadReply->deleteLater();
        this->deleteLater();
    }
}

void DownloadManager::download_complete(QNetworkReply *reply)
{
    QString errorMessage;
    QString fileName(saveFile->fileName());
    if (reply->error() != QNetworkReply::NoError)
    {
        errorMessage = "Network error: " + reply->errorString();
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
            errorMessage = "Error writing to file " + fileName;
        }
    }
    delete saveFile;
    reply->deleteLater();
    this->deleteLater();
    if (errorMessage.isEmpty())
    {
        UnzipThread *unzip = new UnzipThread(fileName, versionInfo);
        connect(unzip, &UnzipThread::log, mainWindow, &MainWindow::log);
        connect(unzip, &UnzipThread::progress, mainWindow, &MainWindow::update_progress_bar);
        connect(unzip, &UnzipThread::unzip_fail, mainWindow, &MainWindow::download_fail);
        connect(unzip, &UnzipThread::unzip_complete, mainWindow, &MainWindow::download_success);
        connect(unzip, &UnzipThread::finished, unzip, &QObject::deleteLater);
        unzip->start();
    }
    else
    {
        mainWindow->download_fail(errorMessage);
    }
}
