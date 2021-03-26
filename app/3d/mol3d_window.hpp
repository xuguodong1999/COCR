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
class Mol3DWindow : public Qt3DExtras::Qt3DWindow {
Q_OBJECT
    QPoint lastPos;
    bool isPressed;
    float activatedRadius;
    Qt3DCore::QTransform *lightTrans;// 操纵光源的位置
//    Qt3DRender::QScreenRayCaster *mScreenRayCaster;
//    Qt3DInput::QMouseHandler *mMouseHandler;
    Q_PROPERTY(float activatedRadius READ getActivatedRadius WRITE setActivatedRadius NOTIFY activatedRadiusChanged)
Q_SIGNALS:

    void spaceOrEnterPressed(bool isSpace);

    void forwardOrBackwardPressed(bool isForward);

    void activatedRadiusChanged(const float &newAadius);

public:
    QVector3D getViewSize() const;

    float getActivatedRadius() const;

    void setActivatedRadius(const float &_activatedRadius);

    explicit Mol3DWindow(Qt3DCore::QEntity *_root, QScreen *_screen = nullptr);

    ~Mol3DWindow() = default;

protected:
    bool event(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void touchEvent(QTouchEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
};

#endif//_XGD_MOL3D_WINDOW_HPP_
