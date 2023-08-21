#include "Mol2DWidget.h"
#include "AtomItem.h"
#include "BondItem.h"
#include "Common/WaitHintWidget.h"
#include "QtUtil.h"

#include "XGD/Chem/Mol.h"

#include <QDebug>
#include <cmath>

Mol2DWidget::Mol2DWidget(QWidget *parent) : GestureView(parent) {
    setBackgroundBrush(Qt::GlobalColor::white);
    setForegroundBrush(Qt::NoBrush);
    setAutoFillBackground(true);
    scene = new QGraphicsScene();
    setScene(scene);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hintWidget = new WaitHintWidget(this);
}

inline static QString getRichText(const std::string &_text) {
    // TODO: THIS IS A NAIVE IMPL
    QString str = "<h1>";
    for (auto &c: _text) {
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

void Mol2DWidget::syncMolToScene(std::shared_ptr<GuiMol> _mol) {
    endWaitHint();
    mol = _mol;
    scene->clear();
    reset();
    if (!mol)return;
    float delta = (std::min)(width(), height()) * 0.1;
    mol->norm2D(width(), height(), delta, delta);
    std::unordered_map<size_t, AtomItem *> atomItemMap;
    mol->loopAtomVec([&](Atom &_atom) {
        auto atomItem = new AtomItem(_atom.getId());
        atomItem->setHTML(getRichText(_atom.getName()));
        if (std::isnan(_atom.x0) || std::isnan(_atom.y0)) {
            atomItem->setLeftTop(0, 0);
        } else {
            atomItem->setLeftTop(_atom.x0, _atom.y0);
        }
        atomItemMap[_atom.getId()] = atomItem;
        if (_atom.isImplicit()) {
            atomItem->setVisible(false);
        }
//        qDebug() << __FUNCTION__ << "add atom, implicit=" << _atom.isImplicit();
        scene->addItem(atomItem);
    });
    mol->loopBondVec([&](Bond &_bond) {
        auto bondItem = new BondItem(_bond.getId());
        auto from = _bond.getFrom(), to = _bond.getTo();
        if (!(from && to)) { return; }
        auto itFrom = atomItemMap.find(from->getId()), itTo = atomItemMap.find(to->getId());
        if (atomItemMap.end() == itFrom) { return; }
        if (atomItemMap.end() == itTo) { return; }
//        qDebug() << _bond.getFromOffset() << "," << _bond.getToOffset();
        bondItem->setBond(itFrom->second, itTo->second, _bond.getType(), _bond.getFromOffset(), _bond.getToOffset());
        scene->addItem(bondItem);
    });
    // item position relies on stable screen status
    update();
}

void Mol2DWidget::mouseDoubleClickEvent(QMouseEvent *e) {
//    normalizeMol();
    GestureView::mouseDoubleClickEvent(e);
}

void Mol2DWidget::keyReleaseEvent(QKeyEvent *event) {
//    switch (event->key()) {
//        case Qt::Key_Enter:
//        case Qt::Key_Return:
//        case Qt::Key_Space:
//            normalizeMol();
//            break;
//        default:
//            break;
//    }
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


QString Mol2DWidget::makeAtomInfo(const size_t &_aid) {
    QString info = tr("atom (global id=") + QString::number(_aid) + tr(") picked\n");
    if (mol) {
        auto atom = mol->getAtom(_aid);
        if (atom) {
            info.append("\n" + tr("element: ") + QString::fromStdString(atom->getName()));
            if (ElementType::SA != atom->getType()) {
                info.append("\n" + tr("mass: ") + QString::number(atom->getMass(), 'f', 4));
            }
        }
    }
    return info;
}

QString Mol2DWidget::makeBondInfo(const size_t &_bid) {
    QString info = tr("bond (global id=") + QString::number(_bid) + tr(") picked\n");
    if (mol) {
        auto bond = mol->getBond(_bid);
        if (bond) {
            info.append("\n" + tr("type: ") + getBondName(*bond));
            auto from = bond->getFrom();
            auto to = bond->getTo();
            if (from && to) {
                info.append("\n" + tr("atom 1: ") + QString::fromStdString(from->getName()));
                info.append("\n" + tr("atom 2: ") + QString::fromStdString(to->getName()));
            }
        }
    }
    return info;
}

