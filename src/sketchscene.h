#ifndef _SKETCHSCENE_H_
#define _SKETCHSCENE_H_

#include "cocr.h"
#include <QGraphicsScene>
#include <QPainterPath>

class SketchScene : public QGraphicsScene {
Q_OBJECT
public:
    explicit SketchScene(QObject *parent = nullptr);

    explicit SketchScene(const QRectF &sceneRect, QObject *parent = nullptr);

    explicit SketchScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);

    ~SketchScene();

    QGraphicsPathItem *commitPath();

    void addPathMoveTo(const QPointF &p);

    void addPathLineTo(const QPointF &p);

    void clearPath();

    void addBackGround(const QSize &size);

    void clearAll();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPainterPath currentPath;
    QGraphicsPixmapItem *bgPixmapItem;

};

#endif
