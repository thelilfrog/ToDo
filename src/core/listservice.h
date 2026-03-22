#ifndef LISTSERVICE_H
#define LISTSERVICE_H

#include <QObject>
#include <QList>
#include <QUuid>

#include "src/obj/list.h"

class ListService : public QObject
{
    Q_OBJECT

private:
    ListService();
    static ListService* _instance;

public:
    static ListService* getInstance();

    List create(QString name);
    List update(QUuid uuid, QString newName);
    void remove(QUuid uuid);

    QList<List> getAll();
    List getByUuid(QUuid uuid);

signals:
    void onListCreated(List value);
    void onListUpdated(List value);
    void onListDeleted(QUuid uuid);
};

#endif // LISTSERVICE_H
