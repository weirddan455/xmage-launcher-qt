#include "javadialog.h"
#include "ui_javadialog.h"
#include "mainwindow.h"

JavaDialog::JavaDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::JavaDialog)
{
    ui->setupUi(this);
    connect(ui->javaLinks, &QLabel::linkActivated, parent, &MainWindow::load_browser);
}

JavaDialog::~JavaDialog()
{
    delete ui;
}

void JavaDialog::on_archButton_clicked()
{
    QApplication::clipboard()->setText(ui->archCmd->text());
}
