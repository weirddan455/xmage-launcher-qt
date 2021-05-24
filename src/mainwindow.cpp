#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , background(new QLabel(this))
    , settings(new Settings)
    , clientConsole(new QPlainTextEdit)
    , serverConsole(new QPlainTextEdit)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->progressBar->setAlignment(Qt::AlignCenter);
    this->setFixedSize(this->size());
    QString imageName = QString::number(QRandomGenerator::global()->bounded(1, 17)) + ".jpg";
    QPixmap backgroundImage(":/backgrounds/" + imageName);
    background->setGeometry(0, ui->menubar->size().height(), this->size().width(), this->size().height() - ui->menubar->size().height());
    background->setPixmap(backgroundImage.scaled(background->size()));
    background->lower();
    clientConsole->setWindowTitle("XMage Client Console");
    clientConsole->setWindowIcon(this->windowIcon());
    clientConsole->setGeometry(0, 0, 860, 480);
    clientConsole->setStyleSheet("background-color:black; color:white;");
    clientConsole->setReadOnly(true);
    serverConsole->setWindowTitle("XMage Server Console");
    serverConsole->setWindowIcon(this->windowIcon());
    serverConsole->setGeometry(0, 0, 860, 480);
    serverConsole->setStyleSheet("background-color:black; color:white;");
    serverConsole->setReadOnly(true);
    javaDialog = new JavaDialog(this);
    aboutDialog = new AboutDialog(this);
}

MainWindow::~MainWindow()
{
    delete aboutDialog;
    delete javaDialog;
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

void MainWindow::on_clientButton_clicked()
{
    launchClient();
}

void MainWindow::on_serverButton_clicked()
{
    if (serverProcess == nullptr)
    {
        launchServer();
    }
    else
    {
        stopServer();
    }
}

void MainWindow::on_clientServerButton_clicked()
{
    launchServer();
    launchClient();
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

void MainWindow::on_javaButton_clicked()
{
    javaDialog->open();
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
    if (serverProcess != nullptr && QMessageBox::question(this, "XMage Server Running", "XMage server is still running. Would you like to close it? If not, it will need to be closed manually.") == QMessageBox::Yes)
    {
        stopServer();
    }
    delete clientConsole;
    delete serverConsole;
    event->accept();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{
    aboutDialog->open();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionForum_triggered()
{
    load_browser("https://www.slightlymagic.net/forum/viewforum.php?f=70");
}

void MainWindow::on_actionWebsite_triggered()
{
    load_browser("http://xmage.de/");
}

void MainWindow::on_actionGitHub_XMage_triggered()
{
    load_browser("https://github.com/magefree/mage");
}

void MainWindow::on_actionGitHub_xmage_launcher_qt_triggered()
{
    load_browser("https://github.com/weirddan455/xmage-launcher-qt");
}

void MainWindow::load_browser(const QString &url)
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

void MainWindow::server_finished()
{
    serverProcess = nullptr;
    ui->serverButton->setText("Launch Server");
    ui->clientServerButton->setEnabled(true);
}

void MainWindow::launchClient()
{
    QString clientJar;
    if (validateJavaSettings() && findClientJar(&clientJar))
    {
        clientConsole->show();
        XMageProcess *process = new XMageProcess(clientConsole);
        process->setWorkingDirectory(settings->xmageInstallLocation + "/mage-client");
        QStringList arguments = settings->currentClientOptions;
        arguments << "-jar" << clientJar;
        process->start(settings->javaInstallLocation, arguments);
    }
}

void MainWindow::launchServer()
{
    QString serverJar;
    if (serverProcess == nullptr && validateJavaSettings() && findServerJar(&serverJar))
    {
        ui->serverButton->setText("Stop Server");
        ui->clientServerButton->setEnabled(false);
        serverConsole->show();
        serverProcess = new XMageProcess(serverConsole);
        connect(serverProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::server_finished);
        serverProcess->setWorkingDirectory(settings->xmageInstallLocation + "/mage-server");
        QStringList arguments = settings->currentServerOptions;
        arguments << "-jar" << serverJar;
        serverProcess->start(settings->javaInstallLocation, arguments);
    }
}

void MainWindow::stopServer()
{
    /* Qt Documentation: https://doc.qt.io/qt-5/qprocess.html#terminate
     * On Windows, terminate works by sending a WM_CLOSE message which does not work for console applications.
     * On Unix based systems (including Mac OSX), it sends a SIGTERM which should work fine.
     */
#ifdef Q_OS_WINDOWS
    serverProcess->kill();
#else
    serverProcess->terminate();
#endif
}

bool MainWindow::validateJavaSettings()
{
    QFileInfo java(settings->javaInstallLocation);
    if (java.isExecutable())
    {
        return true;
    }
    QMessageBox::warning(this, "Invalid Java Configuration", "Invalid Java configuration. Please set Java location in settings.");
    SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
    settingsDialog->showJavaSettings();
    settingsDialog->open();
    return false;
}

bool MainWindow::findClientJar(QString *jar)
{
    QDir clientDir(settings->xmageInstallLocation + "/mage-client/lib");
    QStringList filter;
    filter << "mage-client*.jar";
    QFileInfoList infoList = clientDir.entryInfoList(filter, QDir::Files);
    if (infoList.isEmpty())
    {
        QMessageBox::warning(this, "Invalid XMage Configuration", "Unable to find XMage client jar file. Please set XMage location in settings.");
        SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
        settingsDialog->showXmageSettings();
        settingsDialog->open();
        return false;
    }
    *jar = infoList.at(0).absoluteFilePath();
    return true;
}

bool MainWindow::findServerJar(QString *jar)
{
    QDir clientDir(settings->xmageInstallLocation + "/mage-server/lib");
    QStringList filter;
    filter << "mage-server*.jar";
    QFileInfoList infoList = clientDir.entryInfoList(filter, QDir::Files);
    if (infoList.isEmpty())
    {
        QMessageBox::warning(this, "Invalid XMage Configuration", "Unable to find XMage server jar file. Please set XMage location in settings.");
        SettingsDialog *settingsDialog = new SettingsDialog(settings, this);
        settingsDialog->showXmageSettings();
        settingsDialog->open();
        return false;
    }
    *jar = infoList.at(0).absoluteFilePath();
    return true;
}
