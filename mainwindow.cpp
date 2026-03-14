#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialog.h"
#include "listservice.h"

#include <QDialogButtonBox>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Events
     */

    // ui
    connect(ui->addListButton, &QPushButton::clicked, this, &MainWindow::openCreateListDialog);

    // services
    connect(ListService::getInstance(), &ListService::onListCreated, this, &MainWindow::onListCreated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openCreateListDialog(bool _)
{
    // create the input dialog
    Dialog d = Dialog(this, "Create a list", "New List", "Give a name to this list");
    auto res = d.exec();

    // execute, ignore if not saved
    if (res != QDialog::Accepted) {
        return;
    }

    QString newListName = d.getInput();
    ListService::getInstance()->create(newListName);
}

void MainWindow::onListCreated(List value)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(value.getName());
    item->setData(Qt::UserRole, QVariant(value.getUUID()));

    this->ui->lists->addItem(item);
}
