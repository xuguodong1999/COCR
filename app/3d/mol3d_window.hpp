#ifndef _XGD_MOL3D_WINDOW_HPP_
#define _XGD_MOL3D_WINDOW_HPP_

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>

namespace Qt3DRender {
    class QScreenRayCaster;
}
namespace Qt3DInput {
    class QMouseHandler;
}
class QWidget;

/**
 * FIXME: QWindow 没有 QGesture 支持，sendEvent 到 QWidget 会报
 * FIXME: QGestureManager::deliverEvent: could not find the target for gesture 的警告
 * FIXME: 系 Qt 设计如此，无法在客户端处理掉这个问题。放弃继承 QWindow 重新实现一遍 QGesture 的做法，坑太多
 * TODO: 在 touchEvent 里面实现几个关键手势即可
 */
class Mol3DWindow : public Qt3DExtras::Qt3DWindow {
Q_OBJECT

private:
    inline static float sTransStep = 10;
    inline static float sYOffsetK = 10;
    QPoint lastPos;
    bool isPressed;
    float activatedRadius;
    Qt3DCore::QTransform *lightTrans,// 操纵光源的位置
    *molRootTrans;
    Qt3DCore::QEntity *axisRoot;
//    Qt3DRender::QScreenRayCaster *mScreenRayCaster;
//    Qt3DInput::QMouseHandler *mMouseHandler;
    Q_PROPERTY(float activatedRadius READ getActivatedRadius WRITE setActivatedRadius NOTIFY activatedRadiusChanged)
Q_SIGNALS:

    void spaceOrEnterPressed(bool isSpace);

    void forwardOrBackwardPressed(bool isForward);

    void activatedRadiusChanged(const float &newRadius);

public:
    void setMolRootTrans(Qt3DCore::QTransform *molRootTrans);

    void setAxisRoot(Qt3DCore::QEntity *axis);

    QVector3D getViewSize() const;

    float getActivatedRadius() const;

    void setActivatedRadius(const float &_activatedRadius);

    explicit Mol3DWindow(Qt3DCore::QEntity *_root, QScreen *_screen = nullptr);

    ~Mol3DWindow() = default;

    void reset();

protected:
    bool event(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void touchEvent(QTouchEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *) override;

    void focusOutEvent(QFocusEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    void zoom(const float &_k = 0.05);

    void translate(const QVector2D &_p);


    void rotateMolEntity(const QVector2D &_dir);

    void rotateCamera(const QVector2D &_dir);
};

#endif//_XGD_MOL3D_WINDOW_HPP_
