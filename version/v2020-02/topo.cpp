#include "topo.h"
#include <QGraphicsItem>
#include <QGraphicsScene>

ItemTopo::ItemTopo()
        : index(std::make_pair(0, 0)) {

}

int ItemTopo::getNextAtomId() {
    return ++index.first;
}

int ItemTopo::getNextBondId() {
    return --index.second;
}

void ItemTopo::clear() {
    index.first = index.second = 0;
    abMap.kMap.clear();
    abMap.vMap.clear();
}

void ItemTopo::setToScene(QGraphicsScene *scene) {
    for (auto &i : abMap.kMap) {
        scene->addItem(i.first);
    }
}

int ItemTopo::insertAtom(const char *className, const char *memberName, const QPointF &p) {
    return insert(NEW_ITEM(className, memberName), p);
}

int ItemTopo::insertBond(const char *className) {
    return insert(NEW_ITEM(className, nullptr));
}

bool ItemTopo::FromNull(int bid) {
    BondItem *bondItem = static_cast<BondItem *>(*abMap.getKeys(bid).begin());
    return bondItem->from() == nullptr;
}

bool ItemTopo::ToNull(int bid) {
    BondItem *bondItem = static_cast<BondItem *>(*abMap.getKeys(bid).begin());
    return bondItem->to() == nullptr;
}

void ItemTopo::setFromTo(int bid, int from, int to) {
    BondItem *bondItem = static_cast<BondItem *>(*abMap.getKeys(bid).begin());
    if (from > 0)
        bondItem->setFrom(static_cast<AtomItem *>(*abMap.getKeys(from).begin()));
    if (to > 0)
        bondItem->setTo(static_cast<AtomItem *>(*abMap.getKeys(to).begin()));
}

int ItemTopo::insert(QGraphicsItem *atomItem, const QPointF &p) {
    atomItem->setPos(p);
    int id = getNextAtomId();
    abMap.set(atomItem, id);
    return id;
}

int ItemTopo::insert(QGraphicsItem *bondItem) {
    int id = getNextBondId();
    abMap.set(bondItem, id);
    return id;
}

int ItemTopo::getAtomNum() {
    return index.first;
}

int ItemTopo::getBondNum() {
    return -index.second;
}