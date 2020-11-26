#ifndef _QML_SKETCHITEM_HPP_
#define _QML_SKETCHITEM_HPP_

#include <QQuickPaintedItem>
#include <QMouseEvent>
#include <QPixmap>
#include <QPointF>
#include <QPainter>
#include <QColor>
#include <QPair>
#include <QList>
#include <QPen>

#include <memory>

class SketchItem : public QQuickPaintedItem {
Q_OBJECT
    Q_PROPERTY(int penWidth READ getPenWidth WRITE setPenWidth)
    Q_PROPERTY(QColor penColor READ getPenColor WRITE setPenColor)
public:
    inline static const char *uri = "SketchItem";
    inline static const char *qmlName = "SketchItem";

    SketchItem(QQuickItem *parent = nullptr);

    void setPenWidth(const int &_penWidth);

    void setPenColor(const QColor &_penColor);

    const int getPenWidth() const { return pen.width(); }

    const QColor getPenColor() const { return pen.color(); }

    void paint(QPainter *painter) override;

    Q_INVOKABLE void init(const int &_w, const int &_h);

    Q_INVOKABLE void invokePinchStarted(const qreal &_angle, const qreal &_scale);

    Q_INVOKABLE void invokePinchUpdated(const qreal &_angle, const qreal &_scale);

    Q_INVOKABLE void invokePinchFinished(const qreal &_angle, const qreal &_scale);

    Q_INVOKABLE void invokePressed(const qreal &_x, const qreal &_y);

    Q_INVOKABLE void invokePositionChanged(const qreal &_x, const qreal &_y);

    Q_INVOKABLE void invokeReleased(const qreal &_x, const qreal &_y);

private:
    QList<QPair<QPixmap, bool>> imgLayers;
    QPointF lastPos;
    QPen pen;
    std::shared_ptr<QPainter> bufPainter;

    void putLineAt(const QPointF &_p1, const QPointF &_p2);

    void putPointAt(const QPointF &_p);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // _QML_SKETCHITEM_HPP_
