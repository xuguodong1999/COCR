#pragma once

#include <QGestureEvent>
#include <QGraphicsView>

class GestureView : public QGraphicsView {
Q_OBJECT

    float curAngle, curX, curY, curScale, sumAngle, sumScale;
    inline static float sTransStep = 100;
public:
    explicit GestureView(QWidget *parent = nullptr);

    bool event(QEvent *event) override;

    virtual void zoom(const float &_k);

    virtual void translate(const QPointF &_dir);

    virtual void rotate(const float &_k);

protected:
    void wheelEvent(QWheelEvent *event) override;

    bool gestureEvent(QGestureEvent *event);

    void onPinchGesture(QPinchGesture *gesture);

    void keyReleaseEvent(QKeyEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *) override;

    void reset();
};