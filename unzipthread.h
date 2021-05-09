#ifndef UNZIPTHREAD_H
#define UNZIPTHREAD_H

#include <QDir>
#include <QSaveFile>
#include <QString>
#include <QThread>
#include <zip.h>

#define UNZIP_BUFFER_SIZE 1024

class UnzipThread : public QThread
{
    Q_OBJECT
public:
    UnzipThread(QString fileName);
    void run() override;

private:
    QString fileName;

signals:
    void log(QString message);
    void progress(qint64 complete, qint64 total);
};

#endif // UNZIPTHREAD_H
