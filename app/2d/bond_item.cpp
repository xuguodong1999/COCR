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
    float k = 1.5f * AtomItem::GetCommonSize() / length;
    QPointF o1 = (1 - k) * mFrom->mCenter + k * mTo->mCenter, o2 = k * mFrom->mCenter + (1 - k) * mTo->mCenter;
    switch (mType) {
        case BondType::SingleBond:
            path.moveTo(o1);
            path.lineTo(o2);
            break;
        case BondType::DoubleBond: {
            length = MathUtil::getDistance(o1, o2);
            QPointF v = MathUtil::getVerticalUnitVec(o1 - o2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            path.moveTo(o1 + offset * v);
            path.lineTo(o2 + offset * v);
            path.moveTo(o1 - offset * v);
            path.lineTo(o2 - offset * v);
            break;
        }
        case BondType::TripleBond: {
            length = MathUtil::getDistance(o1, o2);
            QPointF v = MathUtil::getVerticalUnitVec(o1 - o2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sTripleBondAngle)) * length,
                    AtomItem::GetCommonSize());
            path.moveTo(o1);
            path.lineTo(o2);
            path.moveTo(o1 + offset * v);
            path.lineTo(o2 + offset * v);
            path.moveTo(o1 - offset * v);
            path.lineTo(o2 - offset * v);
            break;
        }
        case BondType::UpBond: {
            length = MathUtil::getDistance(o1, o2);
            QPointF v = MathUtil::getVerticalUnitVec(o1 - o2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            path.moveTo(o1);
            path.lineTo(o2 + offset * v);
            path.lineTo(o2 - offset * v);
            path.lineTo(o1);
            break;
        }
        case BondType::DownBond: {// 虚楔形键
            length = MathUtil::getDistance(o1, o2);
            QPointF v = MathUtil::getVerticalUnitVec(o1 - o2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            auto p3 = o2 + offset * v, p4 = o2 - offset * v;
            float x1 = o1.x(), y1 = o1.y(), x3 = p3.x(), y3 = p3.y(), x4 = p4.x(), y4 = p4.y();
            float dx1 = (x3 - x1) / sDownPart, dy1 = (y3 - y1) / sDownPart;
            float dx2 = (x4 - x1) / sDownPart, dy2 = (y4 - y1) / sDownPart;
            for (int i = 0; i < sDownPart; i++) {
                path.moveTo(x3, y3);
                path.lineTo(x4, y4);
                x3 -= dx1;
                y3 -= dy1;
                x4 -= dx2;
                y4 -= dy2;
            }
            break;
        }
        case BondType::ImplicitBond: {// 波浪线
            length = MathUtil::getDistance(o1, o2);
            QPointF v = MathUtil::getVerticalUnitVec(o1 - o2);
            float offset = std::min(
                    std::tan(MathUtil::convertDegreeToRad(sDoubleBondAngle)) * length,
                    AtomItem::GetCommonSize() / 1.5f);
            auto p1 = o1 + offset * v, p2 = o1 - offset * v;
            float x0 = o1.x(), y0 = o1.y(), dx = (o2.x() - x0) / sDownPart, dy = (o2.y() - y0) / sDownPart;
            float x1 = p1.x(), y1 = p1.y(), x2 = p2.x(), y2 = p2.y();
            for (int i = 0; i < sDownPart; i++) {
                path.moveTo(x0, y0);
                if (i % 2) {
                    path.cubicTo(x0, y0, x2 + dx / 2, y2 + dy / 2, x0 + dx, y0 + dy);
                } else {
                    path.cubicTo(x0, y0, x1 + dx / 2, y1 + dy / 2, x0 + dx, y0 + dy);
                }
                x0 += dx;
                y0 += dy;
                x2 += dx;
                y2 += dy;
                x1 += dx;
                y1 += dy;
            }
            break;
        }
        case BondType::DelocalizedBond: {// 虚线
            float x1 = o1.x(), x2 = o2.x(), y1 = o1.y(), y2 = o2.y();
            float dx = (x2 - x1) / sDelocalizedPart / 2, dy = (y2 - y1) / sDelocalizedPart / 2;
            for (int i = 0; i < sDelocalizedPart; i++) {
                path.moveTo(x1, y1);
                x1 += dx;
                y1 += dy;
                path.lineTo(x1, y1);
                x1 += dx;
                y1 += dy;
            }
            break;
        }
    }
    mPathItem->setPath(path);
}