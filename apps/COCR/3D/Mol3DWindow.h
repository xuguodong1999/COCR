#pragma once

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
 * QWindow has no QGesture support: QGestureManager::deliverEvent: could not find the target for gesture
 */
class Mol3DWindow : public Qt3DExtras::Qt3DWindow {
Q_OBJECT

private:
    inline static float sTransStep = 10;
    inline static float sYOffsetK = 10;
    QPoint lastPos;
    bool isPressed;
    float activatedRadius;
    Qt3DCore::QTransform *lightTrans,// manipulate light position
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