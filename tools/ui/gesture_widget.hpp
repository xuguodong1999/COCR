#pragma once
#include <QWidget>
#include <QGestureEvent>

class GestureWidget : public QWidget {
Q_OBJECT
public:
    explicit GestureWidget(QWidget *parent = nullptr);

    bool event(QEvent *e) override;

    virtual void zoom(const float &_k);

    virtual void translate(const QPointF &_dir);

    virtual void rotate(const float &_k);


protected:
    bool gestureEvent(QGestureEvent *e);

    void onPinchGesture(QPinchGesture *gesture);

};