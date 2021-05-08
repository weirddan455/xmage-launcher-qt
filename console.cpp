#include "console.h"

Console::Console(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("XMage Console");
    setGeometry(0, 0, 860, 480);
    text = new QPlainTextEdit(this);
    text->setGeometry(0, 0, 860, 480);
    text->setStyleSheet("background-color:black; color:white;");
    text->setReadOnly(true);
}

Console::~Console()
{
    delete text;
}

void Console::resizeEvent(QResizeEvent *event)
{
    text->resize(event->size());
}

void Console::logProcess(QProcess *process)
{
    processes.append(process);
    connect(process, &QProcess::readyRead, this, &Console::update_console);
}

void Console::update_console()
{
    for (int i = 0; i < processes.size(); i++)
    {
        text->appendPlainText(processes.at(i)->readAll());
    }
}
