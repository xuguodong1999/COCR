#pragma once

#include "base_item.hpp"
#include "chem/jatom.hpp"
#include <QGraphicsTextItem>

class BondItem;

namespace cocr {
    class JAtom;
}
class AtomItem : public BaseItem {
    QGraphicsTextItem *mTextItem;

    friend class BondItem;

    inline static float sCommonSize = 10;
    cocr::id_type aid;
public:

    AtomItem(const cocr::id_type &_aid, QGraphicsItem *parent = nullptr);

    void setHTML(const QString &_html);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;

    /**
     * 设置图元左边缘中心位置，仅在初始化时调用
     * @param _x
     * @param _y
     */
    void setLeftTop(const qreal &_x, const qreal &_y);

    static float GetCommonSize();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
};