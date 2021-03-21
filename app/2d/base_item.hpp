#ifndef _BASE_ITEM_HPP_
#define _BASE_ITEM_HPP_

#include <QGraphicsItem>
#include <QObject>

class BaseItem : public QObject, public QGraphicsItem {
Q_OBJECT
public:
    BaseItem(QGraphicsItem *parent);

    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;

signals:

    void sig_mouse_move(const QPointF &);
};

#endif//_BASE_ITEM_HPP_
