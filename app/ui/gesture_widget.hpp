#ifndef _XGD_GESTURE_WIDGET_HPP_
#define _XGD_GESTURE_WIDGET_HPP_

#include <QWidget>
#include <QGestureEvent>
#include <QGraphicsView>

class GestureWidget : public QWidget {
Q_OBJECT
public:
    explicit GestureWidget(QWidget *parent = nullptr);

    bool event(QEvent *e) override;

    virtual void zoom(const float &_k);

    virtual void translateBy(const QPointF &_dir);

    virtual void rotate(const float &_k);


protected:
    bool gestureEvent(QGestureEvent *e);

    void onPinchGesture(QPinchGesture *gesture);

};

class GestureView : public QGraphicsView {
Q_OBJECT
public:
    explicit GestureView(QWidget *parent = nullptr);

    bool event(QEvent *event) override;

    virtual void zoom(const float &_k);

    virtual void translateBy(const QPointF &_dir);

    virtual void rotate(const float &_k);

protected:
    bool gestureEvent(QGestureEvent *event);

    void onPinchGesture(QPinchGesture *gesture);
};

#endif//_XGD_GESTURE_WIDGET_HPP_
