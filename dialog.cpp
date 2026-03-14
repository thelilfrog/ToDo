#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString title, QString headline, QString message) : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->headline->setText(headline);
    ui->message->setText(message);
}

Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::getInput()
{
    if (this->result() != Accepted) {
        return "";
    }
    return this->ui->lineEdit->text();
}


void Dialog::on_buttonBox_accepted()
{
    accept();
}


void Dialog::on_buttonBox_rejected()
{
    reject();
}

