#include "qt_sketchwidget.hpp"
#include <QPainter>
#include <QDebug>

void SketchWidget::updateRect(const QPoint &p1, const QPoint &p2) {
    int xmin, ymin, xmax, ymax;
    if (p2.x() < p1.x()) {
        xmin = p2.x();
        xmax = p1.x();
    } else {
        xmax = p2.x();
        xmin = p1.x();
    }
    if (p2.y() < p1.y()) {
        ymin = p2.y();
        ymax = p1.y();
    } else {
        ymax = p2.y();
        ymin = p1.y();
    }
    update(QRect(QPoint(xmin - penWidth, ymin - penWidth),
                 QPoint(xmax + penWidth, ymax + penWidth)));
}

SketchWidget::SketchWidget(QWidget *parent) : QWidget(parent) {
    penWidth = 3;
    penColor = Qt::black;
    pen.setWidth(penWidth);
    pen.setColor(penColor);
}

void SketchWidget::paintEvent(QPaintEvent *event) {
    QRect rect = event->rect();
    QPainter bufPainter(this);
    bufPainter.setRenderHint(QPainter::Antialiasing);
    bufPainter.drawPixmap(rect, bufPixmap, rect);
}

void SketchWidget::mousePressEvent(QMouseEvent *event) {
//    qDebug() << "SketchWidget::mousePressEvent";
    QWidget::mousePressEvent(event);
    lastPos = event->pos();
    painter.begin(&bufPixmap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawLine(lastPos, lastPos);
    updateRect(lastPos, lastPos);
    ptsVec.push_back(std::vector<QPoint>());
    ptsVec.back().push_back(lastPos);
}

void SketchWidget::mouseMoveEvent(QMouseEvent *event) {
//    qDebug() << "SketchWidget::mouseMoveEvent";
    QWidget::mouseMoveEvent(event);
    auto currentPos = event->pos();
    painter.drawLine(lastPos, currentPos);
    updateRect(currentPos, lastPos);
    lastPos = currentPos;
    ptsVec.back().push_back(lastPos);
}

void SketchWidget::mouseReleaseEvent(QMouseEvent *event) {
//    qDebug() << "SketchWidget::mouseReleaseEvent";
    QWidget::mouseReleaseEvent(event);
    painter.end();
}

void SketchWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    syncPtsVec();
}

void SketchWidget::syncPtsVec() {
    bufPixmap = QPixmap(size());
    bufPixmap.fill(Qt::white);
    if (bufPixmap.width() == 0 || bufPixmap.height() == 0) {
        return;
    }
    painter.begin(&bufPixmap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    for (auto &pts : ptsVec) {
        if (pts.size() == 0) {
            continue;
        } else if (pts.size() == 1) {
            painter.drawPoint(pts[0]);
            continue;
        }
        for (size_t i = 1; i < pts.size(); i++) {
            painter.drawLine(pts[i - 1], pts[i]);
        }
    }
    painter.end();
    update();
}

void SketchWidget::clear() {
    ptsVec.clear();
    syncPtsVec();
}

HwScript SketchWidget::getScript() const {
    HwScript script;
    for (auto &stroke:ptsVec) {
        if (stroke.empty())continue;
        HwStroke hs;
        for (auto &p:stroke) {
            hs.push_back(cv::Point2f(p.x(), p.y()));
        }
        script.push_back(hs);
    }
    return std::move(script);
}

void SketchWidget::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
}

void SketchWidget::wheelEvent(QWheelEvent *event) {
    QWidget::wheelEvent(event);
}
