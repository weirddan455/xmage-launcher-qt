#include "xmageprocess.h"

XMageProcess::XMageProcess(QPlainTextEdit *console)
{
    this->console = console;
    connect(this, &QProcess::readyRead, this, &XMageProcess::updateConsole);
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &XMageProcess::xmageQuit);
}

void XMageProcess::updateConsole()
{
    console->appendPlainText(this->readAll());
}

void XMageProcess::xmageQuit(int, QProcess::ExitStatus)
{
    this->deleteLater();
}
