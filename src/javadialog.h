#ifndef JAVADIALOG_H
#define JAVADIALOG_H

#include <QDialog>
#include <QClipboard>

namespace Ui {
class JavaDialog;
}

class MainWindow;

class JavaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JavaDialog(MainWindow *parent);
    ~JavaDialog();

private slots:
    void on_archButton_clicked();
    void on_ubuntuButton_clicked();
    void on_ubuntuFXButton_clicked();

private:
    Ui::JavaDialog *ui;
};

#endif // JAVADIALOG_H
