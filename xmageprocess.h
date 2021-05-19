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
    void standard_read();
    void error_read();
    void process_error();
    void xmage_quit(int, QProcess::ExitStatus);
};

#endif // XMAGEPROCESS_H
