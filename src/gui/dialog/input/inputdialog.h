#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent, QString title, QString headline, QString message);
    InputDialog(QWidget *parent, QString title, QString headline, QString message, QString content);
    ~InputDialog();

    QString getInput();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::InputDialog *ui;

    void setErrorLabel(QString error);
    void clearErrorLabel();
};

#endif // INPUTDIALOG_H
