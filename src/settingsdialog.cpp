#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->javaSelection->setText(settings->javaInstallLocation);
    ui->clientOptions->setText(settings->currentClientOptions.join(' '));
    ui->serverOptions->setText(settings->currentServerOptions.join(' '));
    QVector<JavaInfo> javaLocations = detectSystemJava();
    for (int i = 0; i < javaLocations.size(); i++)
    {
        const JavaInfo &javaInfo = javaLocations.at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->javaTree);
        item->setText(0, javaInfo.version);
        item->setText(1, javaInfo.vendor);
        item->setText(2, javaInfo.filePath);
    }
    QMapIterator<QString, XMageVersion> xmageIterator(settings->xmageInstallations);
    while (xmageIterator.hasNext())
    {
        xmageIterator.next();
        QString xmageLocation = xmageIterator.key();
        const XMageVersion &versionInfo = xmageIterator.value();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->xmageTree);
        switch (versionInfo.branch)
        {
            case STABLE:
                item->setText(0, "Stable");
                break;
            case BETA:
                item->setText(0, "Beta");
                break;
            default:
                item->setText(0, "Unknown");
                break;
        }
        item->setText(1, versionInfo.version);
        item->setText(2, xmageLocation);
        if (xmageLocation == settings->xmageInstallLocation)
        {
            item->setSelected(true);
            selectedXmage = item;
        }
    }
    this->settings = settings;
    connect(this, &QDialog::finished, this, &QObject::deleteLater);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::showXmageSettings()
{
    ui->tabs->setCurrentWidget(ui->xmageTab);
}

void SettingsDialog::showJavaSettings()
{
    ui->tabs->setCurrentWidget(ui->javaTab);
}

void SettingsDialog::on_javaTree_itemClicked(QTreeWidgetItem *item, int)
{
    ui->javaSelection->setText(item->text(2));
}

void SettingsDialog::on_xmageTree_itemClicked(QTreeWidgetItem *item, int)
{
    selectedXmage = item;
}

void SettingsDialog::on_javaBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Java Executable");
    if (!fileName.isEmpty())
    {
        ui->javaSelection->setText(fileName);
    }
}

void SettingsDialog::on_xmageAdd_clicked()
{
    QString location = QFileDialog::getExistingDirectory(this, "Select XMage Install Location", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (validateXmage(location))
    {
        QStringList branches;
        branches << "Stable" << "Beta";
        bool dialogAccepted;
        QString selectedBranch = QInputDialog::getItem(this, "Select Branch", "XMage Branch", branches, 0, false, &dialogAccepted);
        if (dialogAccepted)
        {
            QString userVersion = QInputDialog::getText(this, "Input Version Number", "Example: xmage_1.4.48V2 (may be left blank, needed for update check)", QLineEdit::Normal, QString(), &dialogAccepted);
            if (dialogAccepted)
            {
                XMageVersion versionInfo;
                if (userVersion.isEmpty())
                {
                    versionInfo.version = "Unknown";
                }
                else
                {
                    versionInfo.version = userVersion;
                }
                if (selectedBranch == branches.at(1))
                {
                    versionInfo.branch = BETA;
                }
                else
                {
                    versionInfo.branch = STABLE;
                }
                settings->addXmageInstallation(location, versionInfo);
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->xmageTree);
                item->setText(0, selectedBranch);
                item->setText(1, versionInfo.version);
                item->setText(2, location);
            }
        }
    }
}

void SettingsDialog::on_xmageRemove_clicked()
{
    if (selectedXmage != nullptr)
    {
        settings->removeXmageInstallaion(selectedXmage->text(2));
        delete selectedXmage;
        selectedXmage = nullptr;
    }
}

void SettingsDialog::on_resetButton_clicked()
{
    ui->clientOptions->setText(settings->defaultClientOptions.join(' '));
    ui->serverOptions->setText(settings->defaultServerOptions.join(' '));
}

void SettingsDialog::accept()
{
    if (selectedXmage == nullptr)
    {
        showXmageSettings();
        QMessageBox::warning(this, "No XMage Install", "Please select an XMage installation");
        return;
    }
    QString xmageLocation = selectedXmage->text(2);
    if (validateXmage(xmageLocation) && validateJava())
    {
        settings->setXmageInstallLocation(xmageLocation);
        settings->setJavaInstallLocation(ui->javaSelection->text());
        settings->setClientOptions(ui->clientOptions->text());
        settings->setServerOptions(ui->serverOptions->text());
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
    showJavaSettings();
    QMessageBox::warning(this, "Invalid Java", javaExe.filePath() + " is not a valid executable");
    return false;
}

bool SettingsDialog::validateXmage(QString location)
{
    if (!location.isEmpty() && (QDir(location + "/mage-client").exists() || QDir(location + "/mage-server").exists()))
    {
        return true;
    }
    showXmageSettings();
    QMessageBox::warning(this, "Invalid XMage Path", location + " is not a valid XMage installation");
    return false;
}

#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <msi.h>

QVector<JavaInfo> SettingsDialog::detectSystemJava()
{
    QVector<JavaInfo> javaLocations;
    char guid[39];
    char buf[200];
    DWORD index = 0;
    UINT ret = MsiEnumProductsA(index, guid);
    while (ret == ERROR_SUCCESS)
    {
        DWORD bufSize = 200;
        if (MsiGetProductInfoA(guid, "InstalledProductName", buf, &bufSize) == ERROR_SUCCESS
                && (strstr(buf, "Java") != NULL || strstr(buf, "JDK") != NULL))
        {
            bufSize = 200;
            if (MsiGetProductInfoA(guid, "InstallLocation", buf, &bufSize) == ERROR_SUCCESS)
            {
                QString location(buf);
                if (!location.isEmpty())
                {
                    QDir dir(location);
                    if (dir.exists())
                    {
                        QFileInfo javaExe(dir.path() + "/bin/java.exe");
                        if (javaExe.isExecutable())
                        {
                            JavaInfo java;
                            java.filePath = javaExe.filePath();
                            bufSize = 200;
                            if (MsiGetProductInfoA(guid, "Publisher", buf, &bufSize) == ERROR_SUCCESS)
                            {
                                java.vendor = buf;
                            }
                            else
                            {
                                java.vendor = "Unknown Java Vendor";
                            }
                            bufSize = 200;
                            if (MsiGetProductInfoA(guid, "VersionString", buf, &bufSize) == ERROR_SUCCESS)
                            {
                                java.version = buf;
                            }
                            else
                            {
                                java.version = "Unknown Java Version";
                            }
                            javaLocations.append(java);
                        }
                    }
                }
            }
        }
        ret = MsiEnumProductsA(++index, guid);
    }
    return javaLocations;
}

#else

QVector<JavaInfo> SettingsDialog::detectSystemJava()
{
    QString linuxJavaHome("/usr/lib/jvm");
    QDir dir(linuxJavaHome);
    QStringList filter;
    filter << "java*";
    QStringList javaList = dir.entryList(filter, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QVector<JavaInfo> javaLocations;
    for (int i = 0; i < javaList.size(); i++)
    {
        QFileInfo javaExe(linuxJavaHome + '/' + javaList.at(i) + "/bin/java");
        if (javaExe.isExecutable())
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
                java.version = "Java ";
                firstDashIndex++;
                for (int j = firstDashIndex; j < javaList.at(i).size() && stringData[j] != '-'; j++)
                {
                    java.version.append(stringData[j]);
                }
            }
            else
            {
                java.version = "Unknown Java Version";
            }
            if (javaList.at(i).contains("openjdk", Qt::CaseInsensitive))
            {
                java.vendor = "OpenJDK";
            }
            else if (javaList.at(i).contains("oracle", Qt::CaseInsensitive))
            {
                java.vendor = "Oracle";
            }
            else
            {
                java.vendor = "Unknown Java Vendor";
            }
            javaLocations.append(java);
        }
    }
    return javaLocations;
}

#endif
