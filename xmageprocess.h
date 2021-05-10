#ifndef XMAGEPROCESS_H
#define XMAGEPROCESS_H

#include <QProcess>
#include <QPlainTextEdit>

class XMageProcess : public QProcess
{
    Q_OBJECT
public:
    XMageProcess(QPlainTextEdit *console);

private:
    QPlainTextEdit *console;

private slots:
    void updateConsole();
    void xmageQuit(int, QProcess::ExitStatus);
};

#endif // XMAGEPROCESS_H
