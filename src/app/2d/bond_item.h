#pragma once

#include "2d/base_item.h"

#include "ckit/bond.h"
#include "2d/atom_item.h"

#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>

class AtomItem;

class Bond;


class BondItem : public BaseItem {
    BondType mType;
    QGraphicsPathItem *mPathItem;
    AtomItem *mFrom, *mTo;
    float offset1, offset2;
    inline static float sDoubleBondAngle = 12, sTripleBondAngle = 15;
    inline static int sDelocalizedPart = 5, sDownPart = 8;

    friend class AtomItem;

    id_type bid;
public:

    BondItem(const id_type &_bid, QGraphicsItem *parent = nullptr);

    void setBond(AtomItem *_from, AtomItem *_to, const BondType &_type = BondType::SingleBond,
                 const float &_offset1 = 0.5, const float &_offset2 = 0.5);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

private slots:

    void updateBond();
};