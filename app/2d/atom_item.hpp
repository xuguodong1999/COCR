#ifndef _ATOM_ITEM_HPP_
#define _ATOM_ITEM_HPP_

#include "base_item.hpp"
#include <QGraphicsTextItem>

class AtomItem : public BaseItem {
    QGraphicsTextItem *textItem;
public:
    AtomItem(QGraphicsItem *parent = nullptr);

    void setHTML(const QString &_html);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;

    void setPos2D(const float &_x, const float &_y);
};

#endif//_ATOM_ITEM_HPP_
