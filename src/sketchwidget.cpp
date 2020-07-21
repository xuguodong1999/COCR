#include "sketchwidget.h"

#include "sketchview.h"
#include "sketchscene.h"

#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QToolButton>
#include <QDesktopWidget>
#include <QApplication>

SketchWidget::SketchWidget(QWidget *parent) : QWidget(parent) {
    penWidth = 5;

    view = new SketchView(this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRubberBandSelectionMode(Qt::ItemSelectionMode::ContainsItemBoundingRect);
    connect(view, &SketchView::hideFromTop, this, &SketchWidget::showScribbleArea);
    connect(view, &SketchView::showAtTop, this, &SketchWidget::showSketchView);

    currentScene = new SketchScene();
    auto backItem = currentScene->addPixmap(bufPixmap);
    backItem->setZValue(-1);
    backItem->setActive(false);
    scenes.push_back(currentScene);
    view->setScene(currentScene);
    view->hide();
}

SketchWidget::~SketchWidget() {
}

void SketchWidget::mousePressEvent(QMouseEvent *event) {

    QWidget::mousePressEvent(event);
    lastPos = event->pos();
    painter.begin(&bufPixmap);
    painter.setPen(QPen(Qt::black, penWidth));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawLine(lastPos, lastPos);
    updateRect(lastPos, lastPos);

    currentScene->addPathMoveTo(lastPos);
    currentScene->addPathLineTo(lastPos);
}

void SketchWidget::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    auto currentPos = event->pos();
    painter.drawLine(lastPos, currentPos);
    updateRect(currentPos, lastPos);
    lastPos = currentPos;
    currentScene->addPathLineTo(lastPos);
}

void SketchWidget::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    auto item = currentScene->commitPath();
    item->setFlags(QGraphicsItem::ItemIsMovable
                   | QGraphicsItem::ItemIsFocusable);
//                   | QGraphicsItem::ItemIsSelectable);
    item->setPen(QPen(Qt::black, penWidth));
    currentScene->clearPath();
    view->render(&painter, rect(), rect());
    painter.end();
    update();
}

void SketchWidget::paintEvent(QPaintEvent *event) {
    QRect rect = event->rect();
    QPainter bufPainter(this);
    bufPainter.setRenderHint(QPainter::Antialiasing);
    bufPainter.drawPixmap(rect, bufPixmap, rect);
}

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

void SketchWidget::showScribbleArea() {
    bufPixmap.fill(Qt::white);
    painter.begin(&bufPixmap);
    view->render(&painter, rect(), rect());
    painter.end();
    view->hide();
}

void SketchWidget::showSketchView() {
    view->show();
}

int SketchWidget::getPenWidth() const {
    return penWidth;
}

void SketchWidget::setPenWidth(int penWidth) {
    SketchWidget::penWidth = penWidth;
}

const QColor &SketchWidget::getPenColor() const {
    return penColor;
}

void SketchWidget::setPenColor(const QColor &penColor) {
    SketchWidget::penColor = penColor;
}
void SketchWidget::resizeEvent(QResizeEvent*event){
    // qDebug()<<"resize...";
    QWidget::resizeEvent(event);
    bufPixmap = QPixmap(size());
    bufPixmap.fill(Qt::white);
    
    currentScene->setSceneRect(0, 0, width(), height());
    view->resize(size());
    showScribbleArea();
    // showSketchView();
}