#include "bond_item.hpp"
#include "math_util.hpp"
#include "atom_item.hpp"
#include <QDebug>
#include <QPainter>
#include <cmath>

using xgd::MathUtil;

BondItem::BondItem(QGraphicsItem *parent)
        : BaseItem(parent), mFrom(nullptr), mTo(nullptr), mType(xgd::BondType::SingleBond) {
    mPathItem = new QGraphicsPathItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable);
}

void BondItem::setBond(AtomItem *_from, AtomItem *_to, const xgd::BondType &_type) {
    mFrom = _from;
    mTo = _to;
    mType = _type;
    connect(mFrom, &AtomItem::mouse_move, this, &BondItem::updateBond);
    connect(mTo, &AtomItem::mouse_move, this, &BondItem::updateBond);
    updateBond();
}

void BondItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (xgd::BondType::UpBond == mType) {
        painter->fillPath(mPathItem->path(), QBrush(Qt::black));
    } else {
        mPathItem->paint(painter, option, widget);
    }
}

QRectF BondItem::boundingRect() const {
    return mPathItem->boundingRect();
}

void BondItem::updateBond() {
    using namespace xgd;
    QPainterPath path;
    float length = MathUtil::getDistance(mFrom->mCenter, mTo->mCenter);
    float k = AtomItem::GetCommonSize() / length;
    QPointF p1 = k * mFrom->mCenter + (1 - k) * mTo->mCenter, p2 = (1 - k) * mFrom->mCenter + k * mTo->mCenter;
    switch (mType) {
        case BondType::SingleBond:
            path.moveTo(p1);
            path.lineTo(p2);
            break;
        case BondType::DoubleBond: {
            length = MathUtil::getDistance(p1, p2);
            QPointF v = MathUtil::getVerticalUnitVec(p1 - p2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            path.moveTo(p1 + offset * v);
            path.lineTo(p2 + offset * v);
            path.moveTo(p1 - offset * v);
            path.lineTo(p2 - offset * v);
            break;
        }
        case BondType::TripleBond: {
            length = MathUtil::getDistance(p1, p2);
            QPointF v = MathUtil::getVerticalUnitVec(p1 - p2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sTripleBondAngle)) * length,
                    AtomItem::GetCommonSize());
            path.moveTo(p1);
            path.lineTo(p2);
            path.moveTo(p1 + offset * v);
            path.lineTo(p2 + offset * v);
            path.moveTo(p1 - offset * v);
            path.lineTo(p2 - offset * v);
            break;
        }
        case BondType::UpBond: {
            length = MathUtil::getDistance(p1, p2);
            QPointF v = MathUtil::getVerticalUnitVec(p1 - p2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            path.moveTo(p1);
            path.lineTo(p2 + offset * v);
            path.lineTo(p2 - offset * v);
            path.lineTo(p1);
            break;
        }
        case BondType::DownBond:// 虚楔形键
            path.moveTo(p1);
            path.lineTo(p2);
            break;
        case BondType::ImplicitBond: {// 波浪线
            path.moveTo(p1);
            path.lineTo(p2);
            break;
        }
        case BondType::DelocalizedBond:// 虚线
            path.moveTo(p1);
            path.lineTo(p2);
            break;
    }
    mPathItem->setPath(path);
}
