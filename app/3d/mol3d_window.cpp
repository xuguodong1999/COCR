#include "mol3d_window.hpp"
#include "math_util.hpp"
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRayCaster>
#include <Qt3DRender/QScreenRayCaster>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseDevice>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>
#include <QApplication>
#include <QWidget>
#include <cmath>

using xgd::MathUtil;

enum class SimpleGesture {
    GestureNone,
    GestureZoomIn,
    GestureZoomOut
};

// https://www.it610.com/article/1281929365570994176.htm
inline static SimpleGesture handleSimpleTouch(
        const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4) {
    if (p1 == p2 || p3 == p4) {
        return SimpleGesture::GestureNone;
    }
    double delta1 = QLineF(p1, p2).length();
    double delta2 = QLineF(p3, p4).length();
    double delta = delta1;
    if (delta1 < delta2) {
        delta = delta1;
    } else {
        delta = delta2;
    }
    double x1 = p2.x() - p1.x();
    double y1 = p2.y() - p1.y();
    double x2 = p4.x() - p3.x();
    double y2 = p4.y() - p3.y();
    double aaa = x1 * x2 + y1 * y2;
    double bbb = sqrt((pow(x1, 2) + pow(y1, 2)) * (pow(x2, 2) + pow(y2, 2)));
    double cosA = aaa / bbb;
    if (cosA < 0) {
        //通过向量算出两者之间的夹角的余弦值，因为范围是（0，180，所以大于90度，余弦值必定小于0
        double endL = QLineF(p2, p4).length();
        double startL = QLineF(p1, p3).length();
        //然后来判断是放大还是缩小操作算出两指起始点以及终止点之间的距离，如果起始点间的距离大于
        //终止点间的距离则是缩小，反正放大
        if (endL > startL) {
            return SimpleGesture::GestureZoomOut;
        } else {
            return SimpleGesture::GestureZoomIn;
        }
    }
    return SimpleGesture::GestureNone;
}

Mol3DWindow::Mol3DWindow(Qt3DCore::QEntity *_root, QScreen *_screen)
        : Qt3DWindow(_screen), isPressed(false), axisRoot(nullptr) {
    _root->setObjectName("RootEntity");
    auto lightEntity = new Qt3DCore::QEntity(_root);
    auto light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor(Qt::white);
    light->setIntensity(1);
    light->setObjectName("Light");

    lightTrans = new Qt3DCore::QTransform(lightEntity);
    lightTrans->setObjectName("LightTrans");
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTrans);
    lightEntity->setObjectName("LightEntity");

    setActivatedRadius(320);
    // 背景颜色
    defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    // 禁用实时渲染
    renderSettings()->setRenderPolicy(
            Qt3DRender::QRenderSettings::OnDemand);
    // 按三角面片求交
    auto settings = renderSettings()->pickingSettings();
    settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    settings->setPickMethod(Qt3DRender::QPickingSettings::PickMethod::TrianglePicking);
    settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPriorityPick);
    setRootEntity(_root);
}

bool Mol3DWindow::event(QEvent *event) {
    return Qt3DWindow::event(event);
}

void Mol3DWindow::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = true;
    Qt3DWindow::mousePressEvent(event);
}

void Mol3DWindow::mouseReleaseEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = false;
    Qt3DWindow::mouseReleaseEvent(event);
}

void Mol3DWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isPressed) {
        QPoint delta = event->pos() - lastPos;
        lastPos = event->pos();
        float w = width(), h = height();
        w = delta.x() / w * 720 / M_PI;
        h = delta.y() / h * 720 / M_PI;
        switch (event->buttons()) {
            case Qt::MouseButton::LeftButton:
                rotateMolEntity({w, h});
                break;
            case Qt::MouseButton::RightButton:
                rotateCamera({w, h});
                break;
            default:
                break;
        }
    }
    Qt3DWindow::mouseMoveEvent(event);
}

void Mol3DWindow::focusOutEvent(QFocusEvent *event) {
    Qt3DWindow::focusOutEvent(event);
    isPressed = false;
}

void Mol3DWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            reset();
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            translate({0, -sTransStep});
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            translate({-sTransStep, 0});
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            translate({0, sTransStep});
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            translate({sTransStep, 0});
            break;
        default:
            break;
    }
    Qt3DWindow::keyReleaseEvent(event);
}

void Mol3DWindow::wheelEvent(QWheelEvent *event) {
    float k = 0.05;
    if (qApp->keyboardModifiers() == Qt::ControlModifier) {
        k = 0.2;
    }
    if (event->angleDelta().y() > 0) {
        zoom(k);
    } else {
        zoom(-k);
    }
    Qt3DWindow::wheelEvent(event);
}

void Mol3DWindow::touchEvent(QTouchEvent *e) {
    QList touchPoints = e->touchPoints();
    if (touchPoints.count() == 2) {
        isPressed = false;
        switch (handleSimpleTouch(touchPoints[0].startPos(), touchPoints[0].lastPos(),
                                  touchPoints[1].startPos(), touchPoints[1].lastPos())) {
            case SimpleGesture::GestureZoomIn:
                zoom(-0.025);
                break;
            case SimpleGesture::GestureZoomOut:
                zoom(0.025);
                break;
            default:
                break;
        }
        // FIXME: 优化以后再打开
//        auto d0 = touchPoints[0].lastPos() - touchPoints[0].startPos();
//        auto d1 = touchPoints[1].lastPos() - touchPoints[1].startPos();
//        if (d0.x() * d1.x() > 0 && d0.y() * d1.y() > 0) {
//            auto dir = QVector2D((d0 + d1) / 2).normalized();
//            dir.setY(-dir.y());
//            translate(dir);
//        }
    }
    Qt3DWindow::touchEvent(e);
}

float Mol3DWindow::getActivatedRadius() const {
    return activatedRadius;
}

void Mol3DWindow::setActivatedRadius(const float &_activatedRadius) {
    activatedRadius = _activatedRadius;
    auto cam = camera();
    cam->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.001f,
                                          activatedRadius * 100);
    cam->setPosition(activatedRadius * (MathUtil::getOneZ3() + MathUtil::getOneY3() / sYOffsetK));
    cam->setUpVector(MathUtil::getOneY3());
    cam->setViewCenter(MathUtil::getZero3());
    lightTrans->setTranslation(cam->position());
    emit activatedRadiusChanged(activatedRadius);
}

QVector3D Mol3DWindow::getViewSize() const {
    float w = getActivatedRadius();
    return {w, w, w};
}

void Mol3DWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    Qt3DWindow::mouseDoubleClickEvent(event);
    if (axisRoot) { axisRoot->setEnabled(!axisRoot->isEnabled()); }
}


void Mol3DWindow::zoom(const float &_k) {
    auto cam = camera();
    auto pos = cam->position() + _k * (cam->viewCenter() - cam->position());
    if (pos.length() < 10)return;
    cam->setPosition(pos);
    lightTrans->setTranslation(cam->position());
}


void Mol3DWindow::translate(const QVector2D &_p) {
    molRootTrans->setTranslation(molRootTrans->translation() + _p.toVector3D());
}

void Mol3DWindow::reset() {
    auto cam = camera();
    cam->setPosition(activatedRadius * (MathUtil::getOneZ3() + MathUtil::getOneY3() / sYOffsetK));
    cam->setUpVector(MathUtil::getOneY3());
    cam->setViewCenter(MathUtil::getZero3());
    lightTrans->setTranslation(cam->position());
    molRootTrans->setTranslation({0, 0, 0});
    molRootTrans->setRotation(QQuaternion::fromAxisAndAngle({0, 0, 0}, 0));
}

void Mol3DWindow::setMolRootTrans(Qt3DCore::QTransform *molRootTrans) {
    Mol3DWindow::molRootTrans = molRootTrans;
}

void Mol3DWindow::rotateMolEntity(const QVector2D &_dir) {
    // 相机不转物体转，以相机为参考标定旋转角度
    auto cam = camera();
    auto q1 = QQuaternion::fromAxisAndAngle(cam->upVector(), _dir.x());
    // 用叉积拿到垂直于视线轴和头顶轴的向量
    auto q2 = QQuaternion::fromAxisAndAngle(QVector3D::crossProduct(
            cam->viewVector(), cam->upVector()), _dir.y());
    // 四元数旋转变换合成
    molRootTrans->setRotation(q2 * q1 * molRootTrans->rotation());
}

void Mol3DWindow::rotateCamera(const QVector2D &_dir) {
    auto cam = camera();
    cam->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(cam->upVector(), -_dir.x()));
    // 用叉积拿到垂直于视线轴和头顶轴的向量
    cam->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(
            QVector3D::crossProduct(cam->viewVector(), cam->upVector()), -_dir.y()));
    lightTrans->setTranslation(cam->position());
}

void Mol3DWindow::resizeEvent(QResizeEvent *event) {
    Qt3DWindow::resizeEvent(event);
    // 视场粗略大小
    auto s = event->size();
    setActivatedRadius(std::min(s.width(), s.height()) / 4);
}

void Mol3DWindow::setAxisRoot(Qt3DCore::QEntity *axis) {
    axisRoot = axis;
}

