#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , background(new QLabel(this))
    , versionManager(new QNetworkAccessManager(this))
    , downloadManager(new QNetworkAccessManager(this))
    , settings(new Settings)
    , settingsDialog(new SettingsDialog(settings, this))
    , console(new Console(this))
{
    ui->setupUi(this);
    ui->progressBar->hide();
    this->setFixedSize(this->size());
    QString imageName = QString::number(QRandomGenerator::global()->bounded(1, 17)) + ".jpg";
    QPixmap backgroundImage(":/backgrounds/" + imageName);
    background->setGeometry(0, ui->menubar->size().height(), this->size().width(), this->size().height() - ui->menubar->size().height());
    background->setPixmap(backgroundImage.scaled(background->size()));
    background->lower();
    connect(versionManager, &QNetworkAccessManager::finished, this, &MainWindow::poll_github);
    connect(downloadManager, &QNetworkAccessManager::finished, this, &MainWindow::download_xmage);
}

MainWindow::~MainWindow()
{
    delete console;
    delete settingsDialog;
    delete settings;
    delete downloadManager;
    delete versionManager;
    delete background;
    delete ui;
}

void MainWindow::on_launchButton_clicked()
{
    if (settings->xmageInstallLocation.isEmpty())
    {
        QMessageBox::warning(this, "XMage location not set", "Please set XMage location in settings.");
        settingsDialog->showXmageSettings();
        return;
    }
    if (settings->javaInstallLocation.isEmpty())
    {
        QMessageBox::warning(this, "Java location not set", "Please set Java location in settings");
        settingsDialog->showJavaSettings();
        return;
    }
    QString clientLocation = settings->xmageInstallLocation + "/mage-client";
    QString clientJar = clientLocation + "/lib/mage-client-1.4.48.jar";
    console->show();
    QProcess *process = new QProcess();
    process->setWorkingDirectory(clientLocation);
    console->logProcess(process);
    QString program = settings->javaInstallLocation;
    QStringList arguments;
    arguments << "-jar" << clientJar;
    process->start(program, arguments);
}

void MainWindow::on_downloadButton_clicked()
{
    xmageDownloadLocation = QFileDialog::getExistingDirectory(this, "Select location to install XMage", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!xmageDownloadLocation.isEmpty())
    {
        ui->downloadButton->setEnabled(false);
        ui->log->appendPlainText("Polling GitHub for latest version...");
        QNetworkRequest request(QUrl("https://api.github.com/repos/magefree/mage/releases"));
        versionManager->get(request);
    }
}

void MainWindow::on_testButton_clicked()
{
    QString linuxJavaHome("/usr/lib/jvm");
    QDir dir(linuxJavaHome);
    QStringList filter;
    filter << "java*";
    QStringList javaList = dir.entryList(filter, QDir::Dirs);
    QVector<JavaInfo> javaLocations;
    for (int i = 0; i < javaList.size(); i++)
    {
        QFileInfo javaExe(linuxJavaHome + '/' + javaList.at(i) + "/bin/java");
        if (javaExe.exists())
        {
            JavaInfo java;
            java.filePath = javaExe.filePath();
            const QChar *stringData = javaList.at(i).constData();
            int firstDashIndex = -1;
            for (int j = 0; j < javaList.at(i).size(); j++)
            {
                if (stringData[j] == '-')
                {
                    firstDashIndex = j;
                    break;
                }
            }
            if (firstDashIndex != -1 && javaList.at(i).size() > firstDashIndex + 1)
            {
                java.version.append("Java ");
                firstDashIndex++;
                for (int j = firstDashIndex; j < javaList.at(i).size() && stringData[j] != '-'; j++)
                {
                    java.version.append(stringData[j]);
                }
            }
            else
            {
                java.version.append("Unknown Java Version");
            }
            if (javaList.at(i).contains("openjdk", Qt::CaseInsensitive))
            {
                java.vendor.append("OpenJDK");
            }
            else if (javaList.at(i).contains("oracle", Qt::CaseInsensitive))
            {
                java.vendor.append("Oracle");
            }
            else
            {
                java.vendor.append("Unknown Java Vendor");
            }
            javaLocations.append(java);
        }
    }
    for (int i = 0; i < javaLocations.size(); i++)
    {
        ui->log->appendPlainText(javaLocations.at(i).version);
        ui->log->appendPlainText(javaLocations.at(i).vendor);
        ui->log->appendPlainText(javaLocations.at(i).filePath);
    }
}

void MainWindow::update_progress_bar(qint64 bytesReceived, qint64 bytesTotal)
{
    float percentage = (float)bytesReceived / (float)bytesTotal * 100.0f + 0.5f;
    ui->progressBar->setValue((int)percentage);
}

void MainWindow::poll_github(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject latestRelease = QJsonDocument::fromJson(reply->readAll()).array().first().toObject();
        latestXmageVersion = latestRelease.value("name").toString();
        QUrl url(latestRelease.value("assets").toArray().first().toObject().value("browser_download_url").toString());
        if (url.isValid())
        {
            ui->log->appendPlainText("Downloading XMage from " + url.toString());
            ui->progressBar->show();
            QNetworkRequest request(url);
            request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
            QNetworkReply *newReply = downloadManager->get(request);
            connect(newReply, &QNetworkReply::downloadProgress, this, &MainWindow::update_progress_bar);
        }
        else
        {
            ui->log->appendPlainText("Error parsing JSON");
            ui->downloadButton->setEnabled(true);
        }
    }
    else
    {
        ui->log->appendPlainText("Network error");
        ui->downloadButton->setEnabled(true);
    }
    reply->deleteLater();
}

void MainWindow::download_xmage(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString fileName;
        if (!xmageDownloadLocation.isEmpty())
        {
            fileName.append(xmageDownloadLocation + '/');
        }
        if (!latestXmageVersion.isEmpty())
        {
            fileName.append(latestXmageVersion + ".zip");
        }
        else
        {
            fileName.append("xmage.zip");
        }
        QSaveFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            if (file.commit())
            {
                ui->log->appendPlainText("Download complete. Unzipping file...");
                unzip_file(fileName);
            }
        }
        else
        {
            ui->log->appendPlainText("Failed to create file. Check write privlages.");
        }
    }
    else
    {
        ui->log->appendPlainText("Network error");
    }
    reply->deleteLater();
    ui->downloadButton->setEnabled(true);
    ui->progressBar->hide();
}

void MainWindow::unzip_file(QString fileName)
{
    int error = 0;
    zip_t *zip = zip_open(fileName.toLocal8Bit(), ZIP_RDONLY, &error);
    if (error != 0)
    {
        ui->log->appendPlainText("Unzip: Error opening zip file");
    }
    else
    {
        QString outDir(fileName);
        outDir.truncate(outDir.lastIndexOf('/') + 1);
        zip_int64_t numEntries = zip_get_num_entries(zip, 0);
        for (zip_int64_t i = 0; i < numEntries; i++)
        {
            zip_stat_t stat;
            if (zip_stat_index(zip, i, 0, &stat) != 0)
            {
                ui->log->appendPlainText("Unzip: Error getting info on file at index " + QString::number(i));
                continue;
            }
            if (stat.name[strlen(stat.name) - 1] == '/')
            {
                QDir().mkdir(outDir + stat.name);
                continue;
            }
            QSaveFile out(outDir + stat.name);
            if (!out.open(QIODevice::WriteOnly))
            {
                ui->log->appendPlainText("Unzip: Error creating file " + QString(stat.name));
                continue;
            }
            zip_file_t *in = zip_fopen_index(zip, i, 0);
            if (in == NULL)
            {
                ui->log->appendPlainText("Unzip: Failed to open " + QString(stat.name));
                continue;
            }
            char buf[1024];
            int len = zip_fread(in, buf, 1024);
            bool writeError = false;
            while (len > 0)
            {
                if (out.write(buf, len) == -1)
                {
                    ui->log->appendPlainText("Unzip: Error writing to file " + QString(stat.name));
                    writeError = true;
                    break;
                }
                len = zip_fread(in, buf, 1024);
            }
            if (len == -1)
            {
                ui->log->appendPlainText("Unzip: Error reading from file " + QString(stat.name));
            }
            else if (!writeError)
            {
                out.commit();
            }
            if (zip_fclose(in) != 0)
            {
                ui->log->appendPlainText("Unzip: Error closing file " + QString(stat.name));
            }
        }
    }
    zip_discard(zip);
    ui->log->appendPlainText("Unzip complete");
}

void MainWindow::on_actionSettings_triggered()
{
    settingsDialog->show();
}
