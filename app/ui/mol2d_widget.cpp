#include "mol2d_widget.hpp"
#include "2d/atom_item.hpp"
#include "2d/bond_item.hpp"
#include <QGraphicsTextItem>
#include <QDebug>

Mol2DWidget::Mol2DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol)
        : GestureView(parent), mol(std::move(_mol)) {
    scene = new QGraphicsScene();
    setScene(scene);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

inline static QString getRichText(const std::string &_text) {
    return QString("<h1>") + _text.c_str() + QString("</h1>");// just test
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
        auto itFrom = atomItemMap.find(from->getId()), itTo = atomItemMap.find(to->getId());
        if (atomItemMap.end() == itFrom) { return; }
        if (atomItemMap.end() == itTo) { return; }
        bondItem->setBond(itFrom->second, itTo->second, _bond.getType());
        scene->addItem(bondItem);
    });
}

void Mol2DWidget::mouseDoubleClickEvent(QMouseEvent *e) {
    syncMolToScene();
    GestureView::mouseDoubleClickEvent(e);
}

void Mol2DWidget::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            syncMolToScene();
            break;
        default:
            break;
    }
    GestureView::keyReleaseEvent(event);
}



