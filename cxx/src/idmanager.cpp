#include "idmanager.hpp"

size_t IdInterface::Get_new_id() {
    if (idPool.empty()) {
        return currentMaxId++;
    } else {
        size_t ret = idPool.back();
        idPool.pop_back();
        return ret;
    }
}

void IdInterface::Give_back_id(const size_t &_freedId) {
    idPool.push_back(_freedId);
}

const size_t IdInterface::getId() { return id; }

IdInterface::IdInterface() : id(Get_new_id()) {

}

IdInterface::~IdInterface() {
    Give_back_id(id);
}