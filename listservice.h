#ifndef LISTSERVICE_H
#define LISTSERVICE_H

#include <QObject>
#include <QMap>

#include "list.h"

/*
 *
 * ListService is a singleton that query the "lists" over the datasource layer
 *
 */
class ListService : public QObject
{
    Q_OBJECT
private:
    ListService();
    static ListService* _instance;

    QMap<qint128,List> store;

public:
    static ListService* getInstance();

    List create(QString name);
    List update(QUuid uuid, QString newName);
    void remove(QUuid uuid);

signals:
    void onListCreated(List value);
    void onListUpdated(List value);
    void onListDeleted(QUuid uuid);

};

#endif // LISTSERVICE_H
