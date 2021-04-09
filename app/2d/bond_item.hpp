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
    float offset1, offset2;
    inline static float sDoubleBondAngle = 12, sTripleBondAngle = 15;
    inline static int sDelocalizedPart = 5, sDownPart = 8;

    friend class AtomItem;

public:
    BondItem(QGraphicsItem *parent = nullptr);

    void setBond(AtomItem *_from, AtomItem *_to, const xgd::BondType &_type = xgd::BondType::SingleBond,
                 const float &_offset1 = 0.5, const float &_offset2 = 0.5);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;


private slots:

    void updateBond();
};


#endif//_BOND_ITEM_HPP_
