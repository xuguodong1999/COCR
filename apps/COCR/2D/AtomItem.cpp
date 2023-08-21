#include "AtomItem.h"
#include "View2DWidget.h"

AtomItem::AtomItem(const id_type &_aid, QGraphicsItem *parent) :
        aid(_aid), BaseItem(parent) {
    mTextItem = new QGraphicsTextItem(this);
    mTextItem->setDefaultTextColor(Qt::GlobalColor::black);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void AtomItem::setHTML(const QString &_html) {
    mTextItem->setHtml(_html);
}

#include <QPen>
#include <QPainter>

void AtomItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    mTextItem->paint(painter, option, widget);

}

QRectF AtomItem::boundingRect() const {
    return mTextItem->boundingRect();
}

void AtomItem::setLeftTop(const qreal &_x, const qreal &_y) {
    setPos(_x, _y);
}

float AtomItem::GetCommonSize() {
    return sCommonSize;
}


void AtomItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    BaseItem::mousePressEvent(e);
    if (view2DWidget) {
        view2DWidget->onAtomPicked(aid);
    }
}

