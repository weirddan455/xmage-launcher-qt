#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVector>
#include <settings.h>

struct JavaInfo
{
    QString filePath;
    QString version;
    QString vendor;
};

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Settings *settings, QWidget *parent = nullptr);
    ~SettingsDialog();
    void showXmageSettings();
    void showJavaSettings();

public slots:
    void accept() override;

private slots:
    void on_javaTable_cellClicked(int row, int column);
    void on_xmageBrowse_clicked();
    void on_javaBrowse_clicked();

private:
    Ui::SettingsDialog *ui;
    Settings *settings;
    QVector<JavaInfo> detectSystemJava();
    bool validateXmage();
    bool validateJava();
};

#endif // SETTINGSDIALOG_H
