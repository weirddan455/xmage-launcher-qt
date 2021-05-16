#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
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
    void on_javaTree_itemClicked(QTreeWidgetItem *item, int);
    void on_xmageTree_itemClicked(QTreeWidgetItem *item, int);
    void on_xmageAdd_clicked();
    void on_xmageRemove_clicked();
    void on_javaBrowse_clicked();

private:
    Ui::SettingsDialog *ui;
    Settings *settings;
    QTreeWidgetItem *selectedXmage = nullptr;
    QVector<JavaInfo> detectSystemJava();
    bool validateXmage(QString location);
    bool validateJava();
};

#endif // SETTINGSDIALOG_H
