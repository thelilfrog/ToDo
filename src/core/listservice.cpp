#include "src/core/listservice.h"

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
} // namespace

ListService* ListService::_instance = nullptr;

ListService::ListService()
{
    QSqlDatabase db = getOpenDatabase();
    ensureSchema(db);
}

ListService *ListService::getInstance()
{
    if (_instance == nullptr) {
        _instance = new ListService();
    }
    return _instance;
}

List ListService::create(QString name)
{
    List list(name);
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("INSERT INTO lists(id, name) VALUES(:id, :name);");
    query.bindValue(":id", toDbUuid(list.getUUID()));
    query.bindValue(":name", list.getName());

    if (!query.exec()) {
        throw makeSqlException("Failed to insert list", query.lastError().text());
    }

    emit onListCreated(list);
    return list;
}

List ListService::update(QUuid uuid, QString newName)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("UPDATE lists SET name = :name WHERE id = :id;");
    query.bindValue(":id", toDbUuid(uuid));
    query.bindValue(":name", newName);

    if (!query.exec()) {
        throw makeSqlException("Failed to update list", query.lastError().text());
    }
    if (query.numRowsAffected() <= 0) {
        throw std::runtime_error("List not found");
    }

    List updated(uuid, newName);
    emit onListUpdated(updated);
    return updated;
}

void ListService::remove(QUuid uuid)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("DELETE FROM lists WHERE id = :id;");
    query.bindValue(":id", toDbUuid(uuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to delete list", query.lastError().text());
    }

    emit onListDeleted(uuid);
}

QList<List> ListService::getAll()
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    if (!query.exec("SELECT id, name FROM lists ORDER BY name COLLATE NOCASE ASC;")) {
        throw makeSqlException("Failed to read lists", query.lastError().text());
    }

    QList<List> lists;
    while (query.next()) {
        const QUuid uuid(query.value(0).toString());
        const QString name = query.value(1).toString();
        lists.append(List(uuid, name));
    }

    return lists;
}

List ListService::getByUuid(QUuid uuid)
{
    QSqlDatabase db = getOpenDatabase();

    QSqlQuery query(db);
    query.prepare("SELECT name FROM lists WHERE id = :id LIMIT 1;");
    query.bindValue(":id", toDbUuid(uuid));

    if (!query.exec()) {
        throw makeSqlException("Failed to read list by uuid", query.lastError().text());
    }

    if (!query.next()) {
        throw std::runtime_error("List not found");
    }

    return List(uuid, query.value(0).toString());
}
