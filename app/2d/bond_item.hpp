#ifndef _BOND_ITEM_HPP_
#define _BOND_ITEM_HPP_

#include "base_item.hpp"
#include "jbond.hpp"
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>

class AtomItem;

class BondItem : public BaseItem {
    xgd::BondType mType;
    QGraphicsPathItem *mPathItem;
    AtomItem *mFrom, *mTo;
    inline static float sDoubleBondAngle = 12, sTripleBondAngle = 15;

    friend class AtomItem;

public:
    BondItem(QGraphicsItem *parent = nullptr);

    void setBond(AtomItem *_from, AtomItem *_to, const xgd::BondType &_type = xgd::BondType::SingleBond);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;


private slots:

    void updateBond();
};


#endif//_BOND_ITEM_HPP_
