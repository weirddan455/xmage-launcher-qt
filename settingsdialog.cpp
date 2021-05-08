#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->xmageSelection->setText(settings->xmageInstallLocation);
    ui->javaSelection->setText(settings->javaInstallLocation);
    QVector<JavaInfo> javaLocations = detectSystemJava();
    ui->javaTable->setRowCount(javaLocations.size());
    ui->javaTable->setColumnWidth(2, 250);
    for (int i = 0; i < javaLocations.size(); i++)
    {
        QTableWidgetItem *version = new QTableWidgetItem(javaLocations.at(i).version);
        ui->javaTable->setItem(i, 0, version);
        QTableWidgetItem *vendor = new QTableWidgetItem(javaLocations.at(i).vendor);
        ui->javaTable->setItem(i, 1, vendor);
        QTableWidgetItem *filePath = new QTableWidgetItem(javaLocations.at(i).filePath);
        ui->javaTable->setItem(i, 2, filePath);
    }
    this->settings = settings;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::showXmageSettings()
{
    ui->tabs->setCurrentWidget(ui->xmageTab);
    show();
}

void SettingsDialog::showJavaSettings()
{
    ui->tabs->setCurrentWidget(ui->javaTab);
    show();
}

QVector<JavaInfo> SettingsDialog::detectSystemJava()
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
    return javaLocations;
}

void SettingsDialog::on_javaTable_cellClicked(int row, int column)
{
    (void)column;
    ui->javaSelection->setText(ui->javaTable->item(row, 2)->text());
}

void SettingsDialog::on_xmageBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Java Executable");
    if (!fileName.isEmpty())
    {
        ui->javaSelection->setText(fileName);
    }
}

void SettingsDialog::on_javaBrowse_clicked()
{
    QString location = QFileDialog::getExistingDirectory(this, "Select XMage Install Location", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!location.isEmpty())
    {
        ui->xmageSelection->setText(location);
    }
}

void SettingsDialog::accept()
{
    if (validateXmage() && validateJava())
    {
        settings->setXmageInstallLocation(ui->xmageSelection->text());
        settings->setJavaInstallLocation(ui->javaSelection->text());
        QDialog::accept();
    }
}

bool SettingsDialog::validateJava()
{
    QFileInfo javaExe(ui->javaSelection->text());
    if (javaExe.isExecutable())
    {
        return true;
    }
    ui->tabs->setCurrentWidget(ui->javaTab);
    QMessageBox::warning(this, "Invalid Java", javaExe.filePath() + " is not a valid executable");
    return false;
}

bool SettingsDialog::validateXmage()
{
    QDir dir(ui->xmageSelection->text());
    if (dir.exists())
    {
        return true;
    }
    ui->tabs->setCurrentWidget(ui->xmageTab);
    QMessageBox::warning(this, "Invalid XMage Path", dir.path() + " does not exist");
    return false;
}
