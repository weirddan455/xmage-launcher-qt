#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPlainTextEdit>
#include <QRandomGenerator>
#include <QLabel>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSaveFile>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <zip.h>
#include <string.h>
#include "console.h"
#include "settingsdialog.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_launchButton_clicked();
    void on_downloadButton_clicked();
    void on_testButton_clicked();
    void poll_github(QNetworkReply *reply);
    void download_xmage(QNetworkReply *reply);
    void update_progress_bar(qint64 bytesReceived, qint64 bytesTotal);
    void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *background;
    QNetworkAccessManager *versionManager;
    QNetworkAccessManager *downloadManager;
    Settings *settings;
    SettingsDialog *settingsDialog;
    Console *console;
    QString latestXmageVersion;
    QString xmageDownloadLocation;

    void unzip_file(QString fileName);
};
#endif // MAINWINDOW_H
