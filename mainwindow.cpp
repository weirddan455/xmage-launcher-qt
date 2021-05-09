#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , background(new QLabel(this))
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
}

MainWindow::~MainWindow()
{
    delete console;
    delete settingsDialog;
    delete settings;
    delete background;
    delete ui;
}

void MainWindow::log(QString message)
{
    ui->log->appendPlainText(message);
}

void MainWindow::downloadComplete()
{
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->downloadButton->setEnabled(true);
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
    QString downloadLocation = QFileDialog::getExistingDirectory(this, "Select location to install XMage", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!downloadLocation.isEmpty())
    {
        ui->downloadButton->setEnabled(false);
        ui->progressBar->show();
        DownloadManager *downloadManager = new DownloadManager(downloadLocation, this);
        downloadManager->download();
    }
}

void MainWindow::on_testButton_clicked()
{
}

void MainWindow::update_progress_bar(qint64 bytesReceived, qint64 bytesTotal)
{
    float percentage = (float)bytesReceived / (float)bytesTotal * 100.0f + 0.5f;
    ui->progressBar->setValue((int)percentage);
}

void MainWindow::on_actionSettings_triggered()
{
    settingsDialog->show();
}
