#ifndef NOTESERVICE_H
#define NOTESERVICE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUuid>
#include <optional>

#include "src/obj/note.h"

class NoteService : public QObject
{
    Q_OBJECT

private:
    NoteService();
    static NoteService* _instance;

public:
    static NoteService* getInstance();

    QUuid create(QUuid listUuid, QString value);
    QString update(QUuid listUuid, QUuid noteUuid, QString newValue, bool isFinished);
    void remove(QUuid listUuid, QUuid noteUuid);

    QList<Note> getByList(QUuid listUuid);
    std::optional<Note> getByUUID(QUuid noteUuid);

signals:
    void onNoteCreated(Note value);
    void onNoteUpdated(Note value);
    void onNoteDeleted(QUuid listUuid, QUuid noteUuid);
};

#endif // NOTESERVICE_H
