#ifndef UNZIPTHREAD_H
#define UNZIPTHREAD_H

#include <QDir>
#include <QSaveFile>
#include <QString>
#include <QThread>
#include <settings.h>
#include <zip.h>

#define UNZIP_BUFFER_SIZE 1024

class UnzipThread : public QThread
{
    Q_OBJECT
public:
    UnzipThread(QString fileName, XMageVersion versionInfo, bool update);
    void run() override;

private:
    QString fileName;
    XMageVersion versionInfo;
    bool update;

signals:
    void log(QString message);
    void progress(qint64 complete, qint64 total);
    void unzip_complete(QString installLocation, XMageVersion versionInfo);
    void unzip_fail(QString errorMessage);
};

#endif // UNZIPTHREAD_H
