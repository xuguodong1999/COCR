#ifndef _BOND_ITEM_HPP_
#define _BOND_ITEM_HPP_

#include "base_item.hpp"
#include "jbond.hpp"
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>

class BondItem : public BaseItem {
    xgd::BondType type;
    QGraphicsPathItem *pathItem;
    QPointF from, to;
    inline static float sMinSize = 10;
public:
    BondItem(QGraphicsItem *parent = nullptr);

    void setBond(const QPointF &_from, const QPointF &_to, const xgd::BondType &_type = xgd::BondType::SingleBond);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;

public slots:

    void updateFrom(const QPointF &_from);

    void updateTo(const QPointF &_to);

private:
    void updateBond();
};


#endif//_BOND_ITEM_HPP_
