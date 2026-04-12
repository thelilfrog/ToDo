#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->versionLabel->setText(QString("%1-%2").arg(APP_VERSION, APP_ARCH));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
