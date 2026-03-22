#include "list.h"

List::List(QUuid uuid, QString name)
{
    this->uuid = uuid;
    this->name = name;
}

List::List(QString name)
{
    this->uuid = QUuid::createUuid();
    this->name = name;
}

QUuid List::getUUID()
{
    return this->uuid;
}

QString List::getName()
{
    return this->name;
}

void List::setName(QString value)
{
    this->name = value;
}

List List::duplicate()
{
    List l = List(this->getName() + " (copy)");
    return l;
}
