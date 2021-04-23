#ifndef _2D_BASE_ITEM_HPP_
#define _2D_BASE_ITEM_HPP_

#include <QGraphicsItem>
#include <QObject>

class BaseItem : public QObject, public QGraphicsItem {
Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    bool isPressed;
public:
    BaseItem(QGraphicsItem *parent);

    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;

signals:

    void sig_mouse_move(const QPointF &);

    void sig_mouse_click(const QPointF &);
};

#endif//_2D_BASE_ITEM_HPP_
