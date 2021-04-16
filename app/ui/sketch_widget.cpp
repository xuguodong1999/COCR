#include "sketch_widget.hpp"
#include "application.hpp"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

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
    update(QRect(QPoint(xmin - mPen->width(), ymin - mPen->width()),
                 QPoint(xmax + mPen->width(), ymax + mPen->width())));
}

Q_DECLARE_METATYPE(ui_script_type)

SketchWidget::SketchWidget(QWidget *parent, QPen *_pen, QColor *_bgColor)
        : QWidget(parent), mPen(_pen), mBgColor(_bgColor), isLatest(false) {
    qRegisterMetaTypeStreamOperators<ui_script_type>("QList<QList<QPointF>>");
    ptsList = leafxyApp->getSettings().value("sketch_widget/last_script", QVariant::fromValue(
            ptsList)).value<ui_script_type>();
//    if (!ptsList.empty()) {
//        QTimer::singleShot(50, [&]() {
//            auto result = QMessageBox::information(
//                    nullptr, "hint", "history drawing found. continue from last drawing?",
//                    QMessageBox::Yes | QMessageBox::No);
//            switch (result) {
//                case QMessageBox::No:
//                    ptsList.clear();
//                    leafxyApp->getSettings().setValue("sketch_widget/last_script", QVariant::fromValue(ptsList));
//                    sync();
//                    break;
//                default:
//                    break;
//            }
//        });
//    }
}

void SketchWidget::paintEvent(QPaintEvent *event) {
    QPainter bufPainter(this);
    bufPainter.setRenderHint(QPainter::Antialiasing);
    QRect rect = event->rect();
    bufPainter.drawPixmap(rect, bufPixmap, rect);
}

void SketchWidget::mousePressEvent(QMouseEvent *event) {
//    qDebug() << "SketchWidget::mousePressEvent";
    QWidget::mousePressEvent(event);
    lastPos = event->pos();
    beginPenDraw();
    painter.drawLine(lastPos, lastPos);
    updateRect(lastPos, lastPos);
    ptsList.push_back(QList<QPointF>({lastPos}));
    isLatest = false;
    event->ignore();
}

void SketchWidget::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "SketchWidget::mouseMoveEvent: " << event->pos();
    QWidget::mouseMoveEvent(event);
    auto currentPos = event->pos();
    painter.drawLine(lastPos, currentPos);
    updateRect(currentPos, lastPos);
    lastPos = currentPos;
    ptsList.back().push_back(lastPos);
    isLatest = false;
    event->ignore();
}

void SketchWidget::mouseReleaseEvent(QMouseEvent *event) {
//    qDebug() << "SketchWidget::mouseReleaseEvent";
    QWidget::mouseReleaseEvent(event);
    endPenDraw();
    isLatest = false;
    leafxyApp->getSettings().setValue("sketch_widget/last_script", QVariant::fromValue(ptsList));
    event->ignore();
}

void SketchWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    sync();
}

void SketchWidget::sync() {
    bufPixmap = QPixmap(size());
    bufPixmap.fill(*mBgColor);
    if (!ptsList.empty()) {
        beginPenDraw();
        for (auto &pts : ptsList) {
            if (pts.size() > 1) {
                for (size_t i = 1; i < pts.size(); i++) {
                    painter.drawLine(pts[i - 1], pts[i]);
                }
            } else if (pts.size() == 1) {
                painter.drawPoint(pts[0]);
            }
        }
        endPenDraw();
    }
    update();
}

void SketchWidget::beginPenDraw() {
    painter.begin(&bufPixmap);
    painter.setPen(*mPen);
    painter.setRenderHint(QPainter::Antialiasing);
}

void SketchWidget::endPenDraw() {
    painter.end();
}

void SketchWidget::reset() {
    ptsList.clear();
    sync();
}

SketchWidget::~SketchWidget() {
}

const ui_script_type &SketchWidget::getScript() const {
    return ptsList;
}

bool SketchWidget::IsLatest() const {
    return isLatest;
}

void SketchWidget::setIsLatest(bool isLatest) {
    SketchWidget::isLatest = isLatest;
}

