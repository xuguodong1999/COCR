#ifndef _SKETCH_WIDGET_HPP_
#define _SKETCH_WIDGET_HPP_

#include "hw_script.hpp"

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

class SketchWidget : public QWidget {
Q_OBJECT
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;

    int penWidth;
    QColor penColor;
    QPen pen;

    void updateRect(const QPoint &p1, const QPoint &p2);

    std::vector<std::vector<QPoint>> ptsVec;

    void syncPtsVec();

public:
    HwScript getScript() const;

    void clear();

    SketchWidget(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
};


#endif//_SKETCH_WIDGET_HPP_
