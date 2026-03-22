#ifndef NOTE_H
#define NOTE_H

#include <QUuid>
#include <QString>

class Note
{
public:
    Note() {};
    Note(QUuid listParent, QUuid noteID, QString content, bool finished);
    Note(QUuid listParent, QString content);

    QUuid getUUID();
    QUuid getParentUUID();
    QString getContent();
    bool isFinished();

    void setContent(QString content);
    void setParentUUID(QUuid parent);
    void setFinished(bool value);
private:
    QUuid id;
    QUuid parent;
    QString content;
    bool finished;

};

#endif // NOTE_H
