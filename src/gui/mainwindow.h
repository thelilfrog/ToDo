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
    void openAboutDialog();
    void onListCreated(List value);
    void onListUpdate(List value);
    void onListDeleted(QUuid uuid);
    void onNoteCreated(Note value);
    void onNoteUpdated(Note value);
    void onNoteDeleted(QUuid listUuid, QUuid noteUuid);
    void onListSelected(int i);
    void onSaveNoteButtonClicked(bool);
    void onNewNoteEditReturnPressed();
    void onNoteChanged(QListWidgetItem*);
    void onListRightClick(const QPoint &pos);
    void onListContextMenuDelete(bool);
    void onListContextMenuRename(bool);
    void quit();

private:
    Ui::MainWindow *ui;

    int noteSeparation = -1;

    void preload();
    void updateNoteSeparation();

};
#endif // MAINWINDOW_H
