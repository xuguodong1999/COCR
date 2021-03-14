#include "mol2d_widget.hpp"
#include "2d/atom_item.hpp"
#include "2d/bond_item.hpp"
#include <QGraphicsTextItem>
#include <QDebug>

Mol2DWidget::Mol2DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol)
        : QGraphicsView(parent), mol(std::move(_mol)) {
    scene = new QGraphicsScene();
    setScene(scene);
}

inline static QString getRichText(const std::string &_text) {
    return _text.c_str();// just test
}

void Mol2DWidget::syncMolToScene() {
    scene->clear();
    if (!mol)return;
    float delta = std::min(width(), height()) * 0.1;
    mol->norm2D(width(), height(), delta, delta);
    std::unordered_map<size_t, AtomItem *> atomItemMap;
    mol->loopAtomVec([&](xgd::JAtom &_atom) {
        auto atomItem = new AtomItem();
        atomItem->setHTML(getRichText(_atom.getName()));
        atomItem->setPos2D(_atom.x, _atom.y);
        atomItemMap[_atom.getId()] = atomItem;
        scene->addItem(atomItem);
    });
    mol->loopBondVec([&](xgd::JBond &_bond) {
        auto bondItem = new BondItem();
        auto from = _bond.getFrom(), to = _bond.getTo();
        if (!(from && to)) { return; }
        bondItem->setBond({from->x, from->y}, {to->x, to->y}, _bond.getType());
        auto itFrom = atomItemMap.find(from->getId()), itTo = atomItemMap.find(to->getId());
        if (atomItemMap.end() != itFrom) {
            connect(itFrom->second, &AtomItem::mouse_move, bondItem, &BondItem::updateFrom);
        }
        if (atomItemMap.end() != itTo) {
            connect(itTo->second, &AtomItem::mouse_move, bondItem, &BondItem::updateTo);
        }
        scene->addItem(bondItem);
    });
}
