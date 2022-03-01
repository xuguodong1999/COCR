#pragma once

#include "base_item.hpp"

#include "chem/jbond.hpp"
#include "atom_item.hpp"

#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>

class AtomItem;

namespace cocr {
    class JBond;
}

class BondItem : public BaseItem {
    cocr::BondType mType;
    QGraphicsPathItem *mPathItem;
    AtomItem *mFrom, *mTo;
    float offset1, offset2;
    inline static float sDoubleBondAngle = 12, sTripleBondAngle = 15;
    inline static int sDelocalizedPart = 5, sDownPart = 8;

    friend class AtomItem;

    cocr::id_type bid;
public:

    BondItem(const cocr::id_type &_bid, QGraphicsItem *parent = nullptr);

    void setBond(AtomItem *_from, AtomItem *_to, const cocr::BondType &_type = cocr::BondType::SingleBond,
                 const float &_offset1 = 0.5, const float &_offset2 = 0.5);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

private slots:

    void updateBond();
};