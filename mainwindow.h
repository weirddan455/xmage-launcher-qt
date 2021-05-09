#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPlainTextEdit>
#include <QRandomGenerator>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
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

public slots:
    void update_progress_bar(qint64 bytesReceived, qint64 bytesTotal);
    void log(QString message);
    void downloadComplete();

private slots:
    void on_launchButton_clicked();
    void on_downloadButton_clicked();
    void on_testButton_clicked();
    void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *background;
    Settings *settings;
    SettingsDialog *settingsDialog;
    Console *console;
};
#endif // MAINWINDOW_H
