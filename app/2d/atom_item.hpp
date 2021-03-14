#ifndef _ATOM_ITEM_HPP_
#define _ATOM_ITEM_HPP_

#include "base_item.hpp"
#include <QGraphicsTextItem>

class BondItem;
class AtomItem : public BaseItem {
    QGraphicsTextItem *mTextItem;
    QPointF mCenter;
    bool mLeftToRight;
    friend class BondItem;
    inline static float sCommonSize=10;
public:
    AtomItem(QGraphicsItem *parent = nullptr);

    void setHTML(const QString &_html);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;

    /**
     * 设置图元中心位置，仅在初始化时调用
     * @param _x
     * @param _y
     */
    void setPos2D(const qreal &_x, const qreal &_y);

    static float GetCommonSize();
};

#endif//_ATOM_ITEM_HPP_
