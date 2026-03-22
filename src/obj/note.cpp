#include "note.h"

Note::Note(QUuid listParent, QUuid noteID, QString content, bool finished)
{
    this->id = noteID;
    this->parent = listParent;
    this->content = content;
    this->finished = finished;
}

Note::Note(QUuid listParent, QString content)
{
    this->id = QUuid::createUuid();
    this->parent = listParent;
    this->content = content;
    this->finished = false;
}

QUuid Note::getUUID()
{
    return this->id;
}

QUuid Note::getParentUUID()
{
    return this->parent;
}

QString Note::getContent()
{
    return this->content;
}

bool Note::isFinished()
{
    return this->finished;
}

void Note::setContent(QString content)
{
    this->content = content;
}

void Note::setParentUUID(QUuid parent)
{
    this->parent = parent;
}

void Note::setFinished(bool value)
{
    this->finished = value;
}
