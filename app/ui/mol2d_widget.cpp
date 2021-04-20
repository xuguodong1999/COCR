#include "mol2d_widget.hpp"
#include "2d/atom_item.hpp"
#include "2d/bond_item.hpp"
#include "waithint_widget.h"

#include <QGraphicsTextItem>
#include <QDebug>

Mol2DWidget::Mol2DWidget(QWidget *parent) : GestureView(parent) {
    scene = new QGraphicsScene();
    setScene(scene);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hintWidget = new WaitHintWidget(this);
}

inline static QString getRichText(const std::string &_text) {
    // TODO: 细化实现
    QString str = "<h1>";
    for (auto &c:_text) {
        switch (c) {
            case '_':
                str.append("<sup>㊀</sup>");
                break;
            case '+':
                str.append("<sup>⊕</sup>");
                break;
            case '#':
                str.append("≡");
                break;
            default:
                str.append(c);
        }
    }
    str.append("</h1>");
    return str;
}

void Mol2DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    endWaitHint();
    mol = _mol;
    scene->clear();
    if (!mol)return;
    float delta = (std::min)(width(), height()) * 0.1;
    mol->norm2D(width(), height(), delta, delta);
    std::unordered_map<size_t, AtomItem *> atomItemMap;
    mol->loopAtomVec([&](xgd::JAtom &_atom) {
        auto atomItem = new AtomItem();
        atomItem->setHTML(getRichText(_atom.getName()));
        atomItem->setLeftTop(_atom.x0, _atom.y0);
        atomItemMap[_atom.getId()] = atomItem;
        if (_atom.isImplicit()) {
            atomItem->setVisible(false);
        }
        scene->addItem(atomItem);
    });
    mol->loopBondVec([&](xgd::JBond &_bond) {
        auto bondItem = new BondItem();
        auto from = _bond.getFrom(), to = _bond.getTo();
        if (!(from && to)) { return; }
        auto itFrom = atomItemMap.find(from->getId()), itTo = atomItemMap.find(to->getId());
        if (atomItemMap.end() == itFrom) { return; }
        if (atomItemMap.end() == itTo) { return; }
//        qDebug() << _bond.getFromOffset() << "," << _bond.getToOffset();
        bondItem->setBond(itFrom->second, itTo->second, _bond.getType(), _bond.getFromOffset(), _bond.getToOffset());
        scene->addItem(bondItem);
    });
    // 图元显示的位置依赖确定的窗体布局
    update();
}

void Mol2DWidget::mouseDoubleClickEvent(QMouseEvent *e) {
    normalizeMol();
    GestureView::mouseDoubleClickEvent(e);
}

void Mol2DWidget::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            normalizeMol();
            break;
        default:
            break;
    }
    GestureView::keyReleaseEvent(event);
}

void Mol2DWidget::normalizeMol() {
    mol->set2DInfoLatest(false);
    mol->norm2D(width(), height());
    syncMolToScene(mol);
}

void Mol2DWidget::startWaitHint() {
    hintWidget->show();
    hintWidget->startWaitHint();
}

void Mol2DWidget::endWaitHint() {
    hintWidget->hide();
}



