#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QRandomGenerator>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardPaths>
#include <QDesktopServices>
#include "settingsdialog.h"
#include "settings.h"
#include "xmageprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void update_progress_bar(qint64 bytesReceived, qint64 bytesTotal);
    void log(QString message);
    void download_fail(QString errorMessage);
    void download_success(QString installLocation, XMageVersion versionInfo);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_updateButton_clicked();
    void on_clientButton_clicked();
    void on_clientServerButton_clicked();
    void on_serverButton_clicked();
    void on_downloadButton_clicked();
    void on_actionSettings_triggered();
    void on_actionQuit_triggered();
    void on_actionForum_triggered();
    void on_actionWebsite_triggered();
    void on_server_quit();

private:
    Ui::MainWindow *ui;
    QLabel *background;
    Settings *settings;
    QPlainTextEdit *clientConsole;
    QPlainTextEdit *serverConsole;
    XMageProcess *serverProcess = nullptr;

    void loadBrowser(QString url);
    bool validateJavaSettings();
    bool findClientJar(QString *jar);
    bool findServerJar(QString *jar);
    void launchClient();
    void launchServer();
};
#endif // MAINWINDOW_H
