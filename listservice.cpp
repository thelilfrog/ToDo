#include "listservice.h"

ListService* ListService::_instance = nullptr;

ListService::ListService() {}

ListService *ListService::getInstance()
{
    if (_instance == nullptr) {
        _instance = new ListService();
    }

    return _instance;
}

List ListService::create(QString name)
{
    // create the object, the uuid is already assigned by itself
    List l = List(name);

    // store
    // TODO: implement the real datastore (SQLite or something like that)
    this->store[l.getUUID().toUInt128()] = l;

    // return the actual value
    onListCreated(l);
    return l;
}

List ListService::update(QUuid uuid, QString newName)
{
    if (!this->store.contains(uuid.toUInt128())) {
        throw new std::exception();
    }

    List l = this->store.value(uuid.toUInt128(), List("!!!"));
    l.setName(newName);
    this->store[uuid.toUInt128()] = l;

    onListUpdated(l);
    return l;
}

void ListService::remove(QUuid uuid)
{
    this->store.remove(uuid.toUInt128());
    onListDeleted(uuid);
}


