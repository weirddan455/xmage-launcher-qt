#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class MainWindow;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(MainWindow *parent);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
