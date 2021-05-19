#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , background(new QLabel(this))
    , settings(new Settings)
    , console(new QPlainTextEdit)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    this->setFixedSize(this->size());
    QString imageName = QString::number(QRandomGenerator::global()->bounded(1, 17)) + ".jpg";
    QPixmap backgroundImage(":/backgrounds/" + imageName);
    background->setGeometry(0, ui->menubar->size().height(), this->size().width(), this->size().height() - ui->menubar->size().height());
    background->setPixmap(backgroundImage.scaled(background->size()));
    background->lower();
    console->setWindowTitle("XMage Console");
    console->setWindowIcon(this->windowIcon());
    console->setGeometry(0, 0, 860, 480);
    console->setStyleSheet("background-color:black; color:white;");
    console->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete settings;
    delete background;
    delete ui;
}

void MainWindow::log(QString message)
{
    ui->log->appendPlainText(message);
}

void MainWindow::download_fail(QString errorMessage)
{
    log(errorMessage);
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->downloadButton->setEnabled(true);
    ui->updateButton->setEnabled(true);
}

void MainWindow::download_success(QString installLocation, XMageVersion versionInfo)
{
    settings->setXmageInstallLocation(installLocation);
    settings->addXmageInstallation(installLocation, versionInfo);
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->downloadButton->setEnabled(true);
    ui->updateButton->setEnabled(true);
}

void MainWindow::on_updateButton_clicked()
{
    if (settings->xmageInstallLocation.isEmpty())
    {
        QMessageBox::warning(this, "XMage location not set", "Please set XMage location in settings.");
        SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
        settingsDialog->showXmageSettings();
        settingsDialog->open();
    }
    else
    {
        XMageVersion versionInfo;
        versionInfo.version = "Unknown";
        versionInfo.branch = UNKNOWN;
        if (settings->xmageInstallations.contains(settings->xmageInstallLocation))
        {
            versionInfo = settings->xmageInstallations.value(settings->xmageInstallLocation);
        }
        if (versionInfo.branch == UNKNOWN)
        {
            QStringList branches;
            branches << "Stable" << "Beta";
            bool dialogAccepted;
            QString selectedBranch = QInputDialog::getItem(this, "Select Branch", "XMage Branch", branches, 0, false, &dialogAccepted);
            if (dialogAccepted)
            {
                if (selectedBranch == branches.at(1))
                {
                    versionInfo.branch = BETA;
                }
                else
                {
                    versionInfo.branch = STABLE;
                }
            }
        }
        if (versionInfo.branch != UNKNOWN)
        {
            ui->downloadButton->setEnabled(false);
            ui->updateButton->setEnabled(false);
            ui->progressBar->show();
            DownloadManager *downloadManager = new DownloadManager(settings->xmageInstallLocation, this);
            downloadManager->updateXmage(versionInfo);
        }
    }
}

void MainWindow::on_launchButton_clicked()
{
    if (settings->xmageInstallLocation.isEmpty())
    {
        QMessageBox::warning(this, "XMage location not set", "Please set XMage location in settings.");
        SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
        settingsDialog->showXmageSettings();
        settingsDialog->open();
    }
    else if (settings->javaInstallLocation.isEmpty())
    {
        QMessageBox::warning(this, "Java location not set", "Please set Java location in settings");
        SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
        settingsDialog->showJavaSettings();
        settingsDialog->open();
    }
    else
    {
        QString clientLocation = settings->xmageInstallLocation + "/mage-client";
        QDir clientLibDir(clientLocation + "/lib");
        QStringList filter;
        filter << "mage-client*.jar";
        QFileInfoList infoList = clientLibDir.entryInfoList(filter, QDir::Files);
        if (infoList.isEmpty())
        {
            log("Unable to find client jar file");
        }
        else
        {
            console->show();
            XMageProcess *process = new XMageProcess(console);
            process->setWorkingDirectory(clientLocation);
            QString program = settings->javaInstallLocation;
            QStringList arguments = settings->currentClientOptions;
            arguments << "-jar" << infoList.at(0).absoluteFilePath();
            process->start(program, arguments);
        }
    }
}

void MainWindow::on_downloadButton_clicked()
{
    QStringList branches;
    branches << "Stable" << "Beta";
    bool dialogAccepted;
    QString selectedBranch = QInputDialog::getItem(this, "Select Branch", "XMage Branch", branches, 0, false, &dialogAccepted);
    if (dialogAccepted)
    {
        QString downloadLocation = QFileDialog::getExistingDirectory(this, "Select location to install XMage", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!downloadLocation.isEmpty())
        {
            ui->downloadButton->setEnabled(false);
            ui->progressBar->show();
            DownloadManager *downloadManager = new DownloadManager(downloadLocation, this);
            if (selectedBranch == branches.at(1))
            {
                downloadManager->downloadBeta();
            }
            else
            {
                downloadManager->downloadStable();
            }
        }
    }
}

void MainWindow::update_progress_bar(qint64 bytesReceived, qint64 bytesTotal)
{
    float percentage = (float)bytesReceived / (float)bytesTotal * 100.0f + 0.5f;
    ui->progressBar->setValue((int)percentage);
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
    settingsDialog->open();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    delete console;
    event->accept();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionForum_triggered()
{
    loadBrowser("https://www.slightlymagic.net/forum/viewforum.php?f=70");
}

void MainWindow::on_actionWebsite_triggered()
{
    loadBrowser("http://xmage.de/");
}

void MainWindow::loadBrowser(QString url)
{
    if (QDesktopServices::openUrl(QUrl(url)))
    {
        log("Loading " + url + " in default web browser");
    }
    else
    {
        log("Failed to open web browser for " + url);
    }
}
