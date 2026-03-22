#ifndef LIST_H
#define LIST_H

#include <QUuid>
#include <QString>

class List
{
private:
    QUuid uuid;
    QString name;

public:
    List() {}
    List(QUuid uuid, QString name);
    List(QString name);

    QUuid getUUID();
    QString getName();

    void setName(QString value);
    List duplicate();

};

#endif // LIST_H
