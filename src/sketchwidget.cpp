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
    setFocusPolicy(Qt::StrongFocus);
    view = new SketchView(this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRubberBandSelectionMode(Qt::ItemSelectionMode::ContainsItemBoundingRect);
    currentScene = new SketchScene();
    view->setScene(currentScene);

    connect(this,&SketchWidget::penWidthChanged,this,[&](){
        pen.setWidth(penWidth);
    });
    connect(this,&SketchWidget::penColorChanged,this,[&]{
        pen.setColor(penColor);
    });
    view->hide();
}

SketchWidget::~SketchWidget() {
}

void SketchWidget::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if (!view->isHidden()) {
        return;
    }
    lastPos = event->pos();
    painter.begin(&bufPixmap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawLine(lastPos, lastPos);
    updateRect(lastPos, lastPos);

    currentScene->addPathMoveTo(lastPos);
    currentScene->addPathLineTo(lastPos);
}

void SketchWidget::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    if (!view->isHidden()) {
        return;
    }
    auto currentPos = event->pos();
    painter.drawLine(lastPos, currentPos);
    updateRect(currentPos, lastPos);
    lastPos = currentPos;
    currentScene->addPathLineTo(lastPos);
}

void SketchWidget::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    if (!view->isHidden()) {
        return;
    }
    auto item = currentScene->commitPath();
    item->setFlags(QGraphicsItem::ItemIsMovable
                   | QGraphicsItem::ItemIsFocusable
                   | QGraphicsItem::ItemIsSelectable);
    item->setPen(pen);
    currentScene->clearPath();
//    view->render(&painter, rect(), rect());
    painter.end();
//    update();
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
    view->hide();
    currentScene->clearSelection();
    syncSketchView();
    update();
}

void SketchWidget::showSketchView() {
    view->show();
}

int SketchWidget::getPenWidth() const {
    return penWidth;
}

void SketchWidget::setPenWidth(int penWidth) {
    SketchWidget::penWidth = penWidth;
    emit penWidthChanged(penWidth);
}

const QColor &SketchWidget::getPenColor() const {
    return penColor;
}

void SketchWidget::setPenColor(const QColor &penColor) {
    SketchWidget::penColor = penColor;
    emit penColorChanged(penColor);
}

void SketchWidget::resizeEvent(QResizeEvent *event) {
    // qDebug()<<"resize...";
    QWidget::resizeEvent(event);
    view->resize(size());// 必须先更新view，否则scene的坐标系更新不会被view感知到
    currentScene->setSceneRect(0, 0, width(), height());
    currentScene->addBackGround(size());
//    view->resize(size());// FIXME: 导致缩放后出现错位重绘
    syncSketchView();
}

void SketchWidget::switchMode(bool _showSketchView) {
    if (_showSketchView) {
        showSketchView();
    } else {
        showScribbleArea();
    }
}

void SketchWidget::syncSketchView() {
    bufPixmap = QPixmap(size());
    bufPixmap.fill(Qt::white);
    if (bufPixmap.width() == 0 || bufPixmap.height() == 0) { return; }
    painter.begin(&bufPixmap);
    view->render(&painter, rect(), rect());
    painter.end();
}

void SketchWidget::clearScene() {
//    qDebug() << "clearScene()";
    if (currentScene != nullptr) {
        currentScene->clearAll();
        currentScene->addBackGround(size());
        syncSketchView();
    }
}

void SketchWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    emit focusStateChanged(true);
}

void SketchWidget::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
    emit closed();
}

QPen SketchWidget::getPen() const {
    return pen;
}

void SketchWidget::setPen(const QPen &pen) {
    SketchWidget::pen = pen;
    emit penChanged(pen);
}
