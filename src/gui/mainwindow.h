#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "src/obj/list.h"
#include "src/obj/note.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void openCreateListDialog(bool);
    void onListCreated(List value);
    void onListUpdate(List value);
    void onListDeleted(QUuid uuid);
    void onNoteCreated(Note value);
    void onNoteUpdated(Note value);
    void onListSelected(int i);
    void onSaveNoteButtonClicked(bool);
    void onNoteChanged(QListWidgetItem*);
    void onListRightClick(const QPoint &pos);
    void onListContextMenuDelete(bool);
    void onListContextMenuRename(bool);

private:
    Ui::MainWindow *ui;

    void preload();

};
#endif // MAINWINDOW_H
