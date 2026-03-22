#include "inputdialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QWidget *parent, QString title, QString headline, QString message) : QDialog(parent), ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->errorLabel->setVisible(false);
    ui->headline->setText(headline);
    ui->message->setText(message);
}

InputDialog::InputDialog(QWidget *parent, QString title, QString headline, QString message, QString content) : QDialog(parent), ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->errorLabel->setVisible(false);
    ui->headline->setText(headline);
    ui->message->setText(message);
    ui->lineEdit->setText(content);
}

InputDialog::~InputDialog()
{
    delete ui;
}

QString InputDialog::getInput()
{
    if (this->result() != Accepted) {
        return "";
    }
    return this->ui->lineEdit->text();
}

void InputDialog::on_buttonBox_accepted()
{
    if (ui->lineEdit->text().isEmpty()) {
        setErrorLabel("Empty name is not valid");
        return;
    }
    accept();
}

void InputDialog::setErrorLabel(QString error)
{
    ui->errorLabel->setText(error);
    ui->errorLabel->setVisible(true);
}

void InputDialog::clearErrorLabel()
{
    ui->errorLabel->setVisible(false);
    ui->errorLabel->setText("error");
}

void InputDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Discard) {
        reject();
    }
}
