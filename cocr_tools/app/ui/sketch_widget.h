#pragma once

#include <QResizeEvent>
#include <QCloseEvent>
#include <QMoveEvent>
#include <QWheelEvent>
#include <QMouseEvent>

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QStack>
#include <QPen>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

using ui_stroke_type = QList<QPointF>;
using ui_script_type = QList<ui_stroke_type>;

class SketchWidget : public QWidget {
Q_OBJECT
public:
    SketchWidget(QWidget *parent, QPen *_pen, QColor *_bgColor);

    ~SketchWidget();

    const ui_script_type &getScript() const;

    /**
     * 利用记录的笔画和颜色模式刷新当前界面
     */
    void sync();

    void saveLastScript();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *e) override;

private:
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;
    QPen *mPen;
    QColor *mBgColor;
    ui_script_type currentScript;
    QStack<ui_stroke_type> historyStrokeStack;
public slots:

    void undo();

    void redo();

    void reset();

signals:

    void sig_modified();

private:

    void updateRect(const QPoint &p1, const QPoint &p2);

    void beginPenDraw();

    void endPenDraw();
};