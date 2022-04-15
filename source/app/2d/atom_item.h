#pragma once

#include "2d/base_item.h"
#include "ckit/atom.h"
#include <QGraphicsTextItem>

class BondItem;

    class Atom;

class AtomItem : public BaseItem {
    QGraphicsTextItem *mTextItem;

    friend class BondItem;

    inline static float sCommonSize = 10;
    id_type aid;
public:

    AtomItem(const id_type &_aid, QGraphicsItem *parent = nullptr);

    void setHTML(const QString &_html);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;

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