#ifndef DEMOWIDGET_2020_1213_H
#define DEMOWIDGET_2020_1213_H

#include <QPainter>
#include <QPixmap>

#include <QWidget>
#include <QToolButton>

#include <QHBoxLayout>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <vector>

class ScribbleArea : public QWidget {
Q_OBJECT
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;

    int penWidth;
    QColor penColor;
    QPen pen;

    void updateRect(const QPoint &p1, const QPoint &p2);

    std::vector<std::vector<QPoint>> ptsVec;
public:
    const std::vector<std::vector<QPoint>> &getPtsVec() const;

private:
    void syncPtsVec();

public:
    void clear();

    ScribbleArea(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
};

class DemoWidget_2020_1213 : public QWidget {
Q_OBJECT
    // 底部 panel
    QHBoxLayout *panelLayout;
    QToolButton *drawBtn, *view2dBtn, *view3dBtn, *saveBtn, *clsBtn;
    QWidget *panelWidget;

    // 画板
    ScribbleArea *scribbleArea;
    // 3D 窗口容器
    QWidget *view3dContainer;
    // 2D 窗口容器
    QWidget *view2dContainer;
    enum class ViewMode {
        ScribbleMode, View2DMode, View3DMode
    } viewMode;

public:
    DemoWidget_2020_1213(QWidget *parent = nullptr);

    ~DemoWidget_2020_1213();

    void resizeEvent(QResizeEvent *event) override;
};

#endif // DEMOWIDGET_2020_1213_H
