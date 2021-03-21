#include "mol3d_window.hpp"
#include "math_util.hpp"
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QGuiApplication>
#include <cmath>

using xgd::MathUtil;


Mol3DWindow::Mol3DWindow(Qt3DCore::QEntity *_root, QScreen *_screen)
        : Qt3DWindow(_screen), isPressed(false) {
    auto lightEntity = new Qt3DCore::QEntity(_root);
    auto light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor(Qt::white);
    light->setIntensity(1);

    lightTrans = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTrans);

    setActivatedRadius(250);
    setRootEntity(_root);
    defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    // 禁用实时渲染
    renderSettings()->setRenderPolicy(
            Qt3DRender::QRenderSettings::OnDemand);
    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::PrimitivePicking);
}

bool Mol3DWindow::event(QEvent *event) {
//    qDebug() << event->type();
    return Qt3DWindow::event(event);
}

void Mol3DWindow::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = true;
    qDebug() << "mouse press@ " << lastPos;
    Qt3DWindow::mousePressEvent(event);
}

void Mol3DWindow::mouseReleaseEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = false;
    qDebug() << "mouse release@ " << lastPos;
    Qt3DWindow::mouseReleaseEvent(event);
}

void Mol3DWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isPressed) {
        QPoint delta = event->pos() - lastPos;
        lastPos = event->pos();
        float w = width(), h = height();
        w = delta.x() / w * 720 / M_PI;
        h = delta.y() / h * 720 / M_PI;
        auto cam = camera();
        cam->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(cam->upVector(), -w));
        // 用叉积拿到垂直于视线轴和头顶轴的向量
        cam->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(
                QVector3D::crossProduct(cam->viewVector(), cam->upVector()), -h));
        lightTrans->setTranslation(cam->position());
    }
    Qt3DWindow::mouseMoveEvent(event);
}

void Mol3DWindow::focusOutEvent(QFocusEvent *event) {
    Qt3DWindow::focusOutEvent(event);
    isPressed = false;
}

float Mol3DWindow::getActivatedRadius() const {
    return activatedRadius;
}

void Mol3DWindow::setActivatedRadius(const float &_activatedRadius) {
    activatedRadius = _activatedRadius;
    auto cam = camera();
    cam->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.001f,
                                          std::numeric_limits<float>::max());
    cam->setPosition(activatedRadius * (MathUtil::getOneZ3() + MathUtil::getOneY3() / 2));
    cam->setUpVector(MathUtil::getOneY3());
    cam->setViewCenter(MathUtil::getZero3());
    lightTrans->setTranslation(cam->position());
    emit activatedRadiusChanged(activatedRadius);
}

void Mol3DWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            emit spaceOrEnterPressed(false);
            break;
        case Qt::Key_Space:
            emit spaceOrEnterPressed(true);
            break;
        case Qt::Key_S:
        case Qt::Key_D:
        case Qt::Key_Down:
        case Qt::Key_Left:
            emit forwardOrBackwardPressed(false);
            break;
        case Qt::Key_W:
        case Qt::Key_A:
        case Qt::Key_Up:
        case Qt::Key_Right:
            emit forwardOrBackwardPressed(true);
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
        auto cam = camera();
        cam->setPosition(cam->position() + k * (cam->viewCenter() - cam->position()));
        lightTrans->setTranslation(cam->position());
    } else {
        auto cam = camera();
        cam->setPosition(cam->position() - k * (cam->viewCenter() - cam->position()));
        lightTrans->setTranslation(cam->position());
    }
    Qt3DWindow::wheelEvent(event);
}

void Mol3DWindow::touchEvent(QTouchEvent *e) {
    qDebug() << "touch @ " << e->touchPoints();
    qDebug() << "touch 4 " << e->touchPointStates();
    Qt3DWindow::touchEvent(e);
}

QVector3D Mol3DWindow::getViewSize() const {
    float w = getActivatedRadius();
    return {w, w, w};
}

