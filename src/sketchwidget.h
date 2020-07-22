#ifndef _SKETCHWIDGET_H_
#define _SKETCHWIDGET_H_

#include "cocr.h"
#include <QPainter>
#include <QWidget>

class SketchView;

class SketchScene;

class SketchWidget : public QWidget {
Q_OBJECT
    int penWidth;
    QColor penColor;
    Q_PROPERTY(QColor penColor READ getPenColor WRITE setPenColor NOTIFY penColorChanged)
    Q_PROPERTY(int penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)

public:
    int getPenWidth() const;

    void setPenWidth(int penWidth);

    const QColor &getPenColor() const;

    void setPenColor(const QColor &penColor);

signals:

    void penColorChanged();

    void penWidthChanged();

    void focusStateChanged(bool focusIn);

    void closed();

public:
    explicit SketchWidget(QWidget *parent = nullptr);

    ~SketchWidget();

    void switchMode(bool);

    void clearScene();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

private:
    SketchScene *currentScene;
    SketchView *view;
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;

    void syncSketchView();

    void updateRect(const QPoint &p1, const QPoint &p2);

    void showScribbleArea();

    void showSketchView();
};

#endif
