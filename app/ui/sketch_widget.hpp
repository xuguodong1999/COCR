#ifndef _XGD_SKETCH_WIDGET_HPP_
#define _XGD_SKETCH_WIDGET_HPP_

#include <QResizeEvent>
#include <QCloseEvent>
#include <QMoveEvent>
#include <QWheelEvent>
#include <QMouseEvent>

#include <QWidget>
#include <QLabel>
#include <QToolButton>

#include <QPen>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

using ui_script_type = QList<QList<QPointF>>;

class SketchWidget : public QWidget {
Q_OBJECT
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;
    QPen *mPen;
    QColor *mBgColor;
    ui_script_type ptsList;
public:

    SketchWidget(QWidget *parent, QPen *_pen, QColor *_bgColor);

    ~SketchWidget();

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void sync();

public slots:

    void reset();

private:
    void updateRect(const QPoint &p1, const QPoint &p2);

    void beginPenDraw();

    void endPenDraw();
};

#endif//_XGD_SKETCH_WIDGET_HPP_
