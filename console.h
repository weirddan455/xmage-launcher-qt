#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QProcess>
#include <QVector>

class Console : public QDialog
{
    Q_OBJECT
public:
    Console(QWidget *parent = nullptr);
    ~Console();
    QPlainTextEdit *text;
    void logProcess(QProcess *process);

private:
    QVector<QProcess*> processes;

private slots:
    void update_console();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // CONSOLE_H
