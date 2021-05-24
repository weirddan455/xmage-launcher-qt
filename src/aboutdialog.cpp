#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "mainwindow.h"

AboutDialog::AboutDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    connect(ui->label, &QLabel::linkActivated, parent, &MainWindow::load_browser);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
