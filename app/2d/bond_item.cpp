#include "bond_item.hpp"


BondItem::BondItem(QGraphicsItem *parent) : BaseItem(parent) {
    pathItem = new QGraphicsPathItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable);
}

void BondItem::setBond(const QPointF &_from, const QPointF &_to, const xgd::BondType &_type) {
    from = _from;
    to = _to;
//    type=_type;
    type = xgd::BondType::SingleBond;// just test
    updateBond();
}

void BondItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    pathItem->paint(painter, option, widget);
}

QRectF BondItem::boundingRect() const {
    return pathItem->boundingRect();
}

void BondItem::updateFrom(const QPointF &_from) {
    from = _from;
    updateBond();
}

void BondItem::updateTo(const QPointF &_to) {
    to = _to;
    updateBond();
}

void BondItem::updateBond() {
    using namespace xgd;
    QPainterPath path;
    switch (type) {
        case BondType::SingleBond:
            path.moveTo(from);
            path.lineTo(to);
            break;
        case BondType::DoubleBond:
            break;
        case BondType::TripleBond:
            break;
        case BondType::UpBond:
            break;
        case BondType::DownBond:
            break;
        case BondType::ImplicitBond:
            break;
        case BondType::DelocalizedBond:
            break;
    }
    pathItem->setPath(path);
}
