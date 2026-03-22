#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "src/gui/dialog/input/inputdialog.h"
#include "src/core/listservice.h"
#include "src/core/noteservice.h"

#include <QDialogButtonBox>
#include <QMenu>

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
    connect(ui->lists, &QListWidget::currentRowChanged, this, &MainWindow::onListSelected);
    connect(ui->lists, &QWidget::customContextMenuRequested, this, &MainWindow::onListRightClick);
    connect(ui->saveNoteButton, &QPushButton::clicked, this, &MainWindow::onSaveNoteButtonClicked);
    connect(ui->notes, &QListWidget::itemChanged, this, &MainWindow::onNoteChanged);

    // services
    connect(ListService::getInstance(), &ListService::onListCreated, this, &MainWindow::onListCreated);
    connect(ListService::getInstance(), &ListService::onListUpdated, this, &MainWindow::onListUpdate);
    connect(ListService::getInstance(), &ListService::onListDeleted, this, &MainWindow::onListDeleted);
    connect(NoteService::getInstance(), &NoteService::onNoteCreated, this, &MainWindow::onNoteCreated);

    preload();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openCreateListDialog(bool _)
{
    // create the input dialog
    InputDialog d = InputDialog(this, "Create a list", "New List", "Give a name to this list");
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

void MainWindow::onListUpdate(List value)
{
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < ui->lists->count(); i++) {
        item = ui->lists->item(i);

        if (item->data(Qt::UserRole).toUuid() == value.getUUID()) {
            item->setText(value.getName());
            return;
        }
    }
}

void MainWindow::onListDeleted(QUuid uuid)
{
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < ui->lists->count(); i++) {
        item = ui->lists->item(i);

        if (item->data(Qt::UserRole).toUuid() == uuid) {
            ui->lists->removeItemWidget(item);
            delete item;
            return;
        }
    }
}

void MainWindow::onNoteCreated(Note value)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setCheckState((value.isFinished() ? Qt::Checked : Qt::Unchecked));
    item->setText(value.getContent());
    item->setData(Qt::UserRole, QVariant(value.getUUID()));

    this->ui->notes->addItem(item);
}

void MainWindow::onNoteUpdated(Note value)
{
    QListWidgetItem *item = ui->lists->currentItem();
    if (item == nullptr) {
        qDebug() << "item null";
        return;
    }

    if (item->data(Qt::UserRole).toUuid() != value.getParentUUID()) {
        qDebug() << "item uuid not matching";
        return;
    }

    for (int i = 0; i < ui->notes->count(); i++) {
        item = ui->notes->item(i);

        if (item->data(Qt::UserRole).toUuid() == value.getUUID()) {
            item->setCheckState((value.isFinished() ? Qt::Checked : Qt::Unchecked));
            item->setText(value.getContent());
            return;
        }
    }
}

void MainWindow::onListSelected(int i)
{
    if (i == -1) {
        ui->notes->clear();
        ui->newNoteEdit->setDisabled(true);
        ui->saveNoteButton->setDisabled(true);
        ui->notes->setDisabled(true);
        return;
    }
    QListWidgetItem *item = ui->lists->item(i);
    QVariant uuid = item->data(Qt::UserRole);

    QList<Note> notes = NoteService::getInstance()->getByList(uuid.toUuid());

    ui->notes->clear();
    foreach (Note n, notes) {
        onNoteCreated(n);
    }

    ui->newNoteEdit->setEnabled(true);
    ui->saveNoteButton->setEnabled(true);
    ui->notes->setEnabled(true);
}

void MainWindow::onSaveNoteButtonClicked(bool _)
{
    QString content = ui->newNoteEdit->text();
    if (content.isEmpty()) {
        return;
    }

    int li = ui->lists->currentRow();
    if (li == -1) {
        return;
    }
    QListWidgetItem *item = ui->lists->item(li);
    QVariant listUUID = item->data(Qt::UserRole);

    NoteService::getInstance()->create(listUUID.toUuid(), content);
    ui->newNoteEdit->clear();
}

void MainWindow::onNoteChanged(QListWidgetItem *item)
{
    NoteService *service = NoteService::getInstance();
    QString content = item->text();

    QVariant uuid = item->data(Qt::UserRole);
    std::optional<Note> opt = service->getByUUID(uuid.toUuid());
    if (opt.has_value()) {
        Note note = opt.value();

        if (content.isEmpty()) {
            service->remove(note.getParentUUID(), note.getUUID());
        } else {
            service->update(note.getParentUUID(), note.getUUID(), content);
            service->setFinishedValue(note.getParentUUID(), note.getUUID(), item->checkState() == Qt::Checked ? true : false);
        }
    }
}

void MainWindow::onListRightClick(const QPoint &pos)
{
    if (ui->lists->selectedItems().length() == 1) {
        QMenu menu(this);

        QAction *renameAction = new QAction(tr("Rename"), this);
        connect(renameAction, &QAction::triggered, this, &MainWindow::onListContextMenuRename);
        menu.addAction(renameAction);

        menu.addSeparator();

        QAction *deleteAction = new QAction(tr("Delete"), this);
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onListContextMenuDelete);
        menu.addAction(deleteAction);

        menu.exec(ui->lists->mapToGlobal(pos));
    }
}

void MainWindow::onListContextMenuDelete(bool _)
{

}

void MainWindow::onListContextMenuRename(bool)
{
    if (ui->lists->selectedItems().length() == 1) {
        QListWidgetItem *item = ui->lists->selectedItems()[0];

        InputDialog d = InputDialog(this, "Edit", "Edit", "Edit the list's name", item->text());
        auto res = d.exec();

        // execute, ignore if not saved
        if (res != QDialog::Accepted) {
            return;
        }

        QString newListName = d.getInput();
        ListService::getInstance()->update(item->data(Qt::UserRole).toUuid(), newListName);
    }
}

void MainWindow::preload()
{
    QList<List> lists = ListService::getInstance()->getAll();

    foreach (List l, lists) {
        onListCreated(l);
    }

    if (lists.count() > 0) {
        ui->lists->setCurrentItem(ui->lists->item(0));
    }
}
