#pragma once
#include <QGraphicsItem>
#include <QObject>

class View2DWidget;

class BaseItem : public QObject, public QGraphicsItem {
Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    bool isPressed;
protected:
    inline static View2DWidget *view2DWidget = nullptr;
public:
    static void SetView2DWidget(View2DWidget *_view2DWidget);

    BaseItem(QGraphicsItem *parent);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;

signals:

    void sig_mouse_move(const QPointF &);

    void sig_mouse_click(const QPointF &);
};