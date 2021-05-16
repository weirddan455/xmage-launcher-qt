#include "unzipthread.h"

UnzipThread::UnzipThread(QString fileName, XMageVersion versionInfo)
{
    this->fileName = fileName;
    this->versionInfo = versionInfo;
}

void UnzipThread::run()
{
    emit log("Unzipping file " + fileName);
    int error = 0;
    zip_t *zip = zip_open(fileName.toLocal8Bit(), ZIP_RDONLY, &error);
    if (error != 0)
    {
        zip_discard(zip);
        emit unzip_fail("Unzip: Error opening zip file");
        return;
    }
    char buf[UNZIP_BUFFER_SIZE];
    QString outDir(fileName);
    outDir.truncate(outDir.lastIndexOf('/') + 1);
    zip_int64_t numEntries = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < numEntries; i++)
    {
        emit progress(i, numEntries);
        zip_stat_t stat;
        if (zip_stat_index(zip, i, 0, &stat) != 0)
        {
            emit log("Unzip: Error getting info on file at index " + QString::number(i));
            continue;
        }
        if (stat.name[strlen(stat.name) - 1] == '/')
        {
            QDir().mkdir(outDir + stat.name);
            continue;
        }
        QSaveFile out(outDir + stat.name);
        if (!out.open(QIODevice::WriteOnly))
        {
            emit log("Unzip: Error creating file " + QString(stat.name));
            continue;
        }
        zip_file_t *in = zip_fopen_index(zip, i, 0);
        if (in == NULL)
        {
            emit log("Unzip: Failed to open " + QString(stat.name));
            continue;
        }
        int len = zip_fread(in, buf, UNZIP_BUFFER_SIZE);
        bool writeError = false;
        while (len > 0)
        {
            if (out.write(buf, len) == -1)
            {
                emit log("Unzip: Error writing to file " + QString(stat.name));
                writeError = true;
                break;
            }
            len = zip_fread(in, buf, UNZIP_BUFFER_SIZE);
        }
        if (len == -1)
        {
            emit log("Unzip: Error reading from file " + QString(stat.name));
        }
        else if (!writeError)
        {
            out.commit();
        }
        if (zip_fclose(in) != 0)
        {
            emit log("Unzip: Error closing file " + QString(stat.name));
        }
    }
    zip_discard(zip);
    emit log("Unzip complete");
    outDir.truncate(outDir.size() - 1);     // Remove trailing '/'
    emit unzip_complete(outDir, versionInfo);
}
