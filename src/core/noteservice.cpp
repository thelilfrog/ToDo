#include "noteservice.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include <stdexcept>

namespace {
const QString kConnectionName = "todo_connection";
const QString kDatabaseFileName = "todo.sqlite3";

QString toDbUuid(const QUuid &uuid)
{
    return uuid.toString(QUuid::WithoutBraces);
}

std::runtime_error makeSqlException(const QString &context, const QString &error)
{
    return std::runtime_error(QString("%1: %2").arg(context, error).toStdString());
}

QSqlDatabase getOpenDatabase()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains(kConnectionName)) {
        db = QSqlDatabase::database(kConnectionName);
    } else {
        const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (appDataPath.isEmpty()) {
            throw std::runtime_error("Unable to resolve AppDataLocation for SQLite database");
        }

        QDir dir(appDataPath);
        if (!dir.exists() && !dir.mkpath(".")) {
            throw std::runtime_error("Unable to create application data directory for SQLite database");
        }

        db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
        db.setDatabaseName(dir.filePath(kDatabaseFileName));
    }

    if (!db.isOpen() && !db.open()) {
        throw makeSqlException("Failed to open SQLite database", db.lastError().text());
    }

    QSqlQuery pragma(db);
    if (!pragma.exec("PRAGMA foreign_keys = ON;")) {
        throw makeSqlException("Failed to enable SQLite foreign keys", pragma.lastError().text());
    }

    return db;
}

void ensureSchema(QSqlDatabase &db)
{
    QSqlQuery query(db);

    if (!query.exec("CREATE TABLE IF NOT EXISTS lists ("
                    "id TEXT PRIMARY KEY,"
                    "name TEXT NOT NULL"
                    ");")) {
        throw makeSqlException("Failed to create lists table", query.lastError().text());
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS notes ("
                    "id TEXT PRIMARY KEY,"
                    "list_id TEXT NOT NULL,"
                    "content TEXT NOT NULL,"
                    "finished BOOL NOT NULL,"
                    "FOREIGN KEY(list_id) REFERENCES lists(id) ON DELETE CASCADE"
                    ");")) {
        throw makeSqlException("Failed to create notes table", query.lastError().text());
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_notes_list_id ON notes(list_id);")) {
        throw makeSqlException("Failed to create notes index", query.lastError().text());
    }
}

void ensureListExists(QSqlDatabase &db, const QUuid &listUuid)
{
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM lists WHERE id = :id LIMIT 1;");
    query.bindValue(":id", toDbUuid(listUuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to validate list existence", query.lastError().text());
    }

    if (!query.next()) {
        throw std::runtime_error("Parent list not found");
    }
}
} // namespace

NoteService* NoteService::_instance = nullptr;

NoteService::NoteService()
{
    QSqlDatabase db = getOpenDatabase();
    ensureSchema(db);
}

NoteService *NoteService::getInstance()
{
    if (_instance == nullptr) {
        _instance = new NoteService();
    }

    return _instance;
}

QUuid NoteService::create(QUuid listUuid, QString value)
{
    QSqlDatabase db = getOpenDatabase();
    ensureListExists(db, listUuid);

    const QUuid newNoteUuid = QUuid::createUuid();

    QSqlQuery query(db);
    query.prepare("INSERT INTO notes(id, list_id, content, finished) VALUES(:id, :list_id, :content, false);");
    query.bindValue(":id", toDbUuid(newNoteUuid));
    query.bindValue(":list_id", toDbUuid(listUuid));
    query.bindValue(":content", value);

    if (!query.exec()) {
        throw makeSqlException("Failed to insert note", query.lastError().text());
    }

    Note n = Note(listUuid, newNoteUuid, value, false);
    emit onNoteCreated(n);
    return newNoteUuid;
}

QString NoteService::update(QUuid listUuid, QUuid noteUuid, QString newValue)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("UPDATE notes SET content = :content WHERE id = :id AND list_id = :list_id;");
    query.bindValue(":id", toDbUuid(noteUuid));
    query.bindValue(":list_id", toDbUuid(listUuid));
    query.bindValue(":content", newValue);

    if (!query.exec()) {
        throw makeSqlException("Failed to update note", query.lastError().text());
    }
    if (query.numRowsAffected() <= 0) {
        throw std::runtime_error("Note not found");
    }

    std::optional<Note> note = getByUUID(noteUuid);

    if (!note.has_value()) {
        throw std::runtime_error("database integrity corrupted");
    }

    Note n = Note(listUuid, noteUuid, newValue, note.value().isFinished());
    emit onNoteUpdated(n);
    return newValue;
}

void NoteService::setFinishedValue(QUuid listUuid, QUuid noteUuid, bool isFinished)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("UPDATE notes SET finished = :finished WHERE id = :id AND list_id = :list_id;");
    query.bindValue(":id", toDbUuid(noteUuid));
    query.bindValue(":list_id", toDbUuid(listUuid));
    query.bindValue(":finished", isFinished);

    if (!query.exec()) {
        throw makeSqlException("Failed to update note", query.lastError().text());
    }
    if (query.numRowsAffected() <= 0) {
        throw std::runtime_error("Note not found");
    }

    std::optional<Note> note = getByUUID(noteUuid);

    if (!note.has_value()) {
        throw std::runtime_error("database integrity corrupted");
    }
    Note n = Note(listUuid, noteUuid, note.value().getContent(), isFinished);
    emit onNoteUpdated(n);
}

void NoteService::remove(QUuid listUuid, QUuid noteUuid)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("DELETE FROM notes WHERE id = :id AND list_id = :list_id;");
    query.bindValue(":id", toDbUuid(noteUuid));
    query.bindValue(":list_id", toDbUuid(listUuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to delete note", query.lastError().text());
    }

    emit onNoteDeleted(listUuid, noteUuid);
}

QList<Note> NoteService::getByList(QUuid listUuid)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("SELECT id, content, finished FROM notes WHERE list_id = :list_id;");
    query.bindValue(":list_id", toDbUuid(listUuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to read notes by list", query.lastError().text());
    }

    QList<Note> notes;
    while (query.next()) {
        const QUuid noteId(query.value(0).toString());
        const QString content = query.value(1).toString();
        const bool finished = query.value(2).toBool();
        notes.append(Note(listUuid, noteId, content, finished));
    }

    return notes;
}

std::optional<Note> NoteService::getByUUID(QUuid noteUuid)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("SELECT id, content, finished, list_id FROM notes WHERE id = :id;");
    query.bindValue(":id", toDbUuid(noteUuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to read note by uuid", query.lastError().text());
    }

    if (query.next()) {
        const QUuid noteId(query.value(0).toString());
        const QString content = query.value(1).toString();
        const bool finished = query.value(2).toBool();
        const QUuid listUuid(query.value(3).toString());

        return std::optional(Note(listUuid, noteId, content, finished));
    }

    return std::nullopt;
}
