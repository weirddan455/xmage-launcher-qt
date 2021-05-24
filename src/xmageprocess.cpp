#include "xmageprocess.h"

XMageProcess::XMageProcess(QPlainTextEdit *console)
{
    this->console = console;
    connect(this, &QProcess::readyReadStandardOutput, this, &XMageProcess::standard_read);
    connect(this, &QProcess::readyReadStandardError, this, &XMageProcess::error_read);
    connect(this, &QProcess::errorOccurred, this, &XMageProcess::process_error);
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &XMageProcess::xmage_quit);
}

void XMageProcess::standard_read()
{
    console->appendPlainText(this->readAllStandardOutput());
}

void XMageProcess::error_read()
{
    console->appendPlainText(this->readAllStandardError());
}

void XMageProcess::process_error()
{
    console->appendPlainText(this->errorString());
}

void XMageProcess::xmage_quit(int, QProcess::ExitStatus)
{
    this->deleteLater();
}
