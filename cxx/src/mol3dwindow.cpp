#include "std_util.hpp"
#include "mol3dwindow.hpp"
#include "qt_util.hpp"

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DCore/QTransform>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>

Mol3DWindow::Mol3DWindow(Qt3DCore::QEntity *_rootEntity, QScreen *_screen)
        : Qt3DWindow(_screen), isPressed(false) {
    auto lightEntity = new Qt3DCore::QEntity(_rootEntity);
    auto light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor(Qt::white);
    light->setIntensity(1);

    lightTrans = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTrans);

    setActivatedRadius(250);
    setRootEntity(_rootEntity);
    defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
}

void Mol3DWindow::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = true;
}

void Mol3DWindow::mouseReleaseEvent(QMouseEvent *event) {
    lastPos = event->pos();
    isPressed = false;
}

void Mol3DWindow::mouseMoveEvent(QMouseEvent *event) {
    if (!isPressed) {
        return;
    }
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

void Mol3DWindow::focusOutEvent(QFocusEvent *event) {
    QWindow::focusOutEvent(event);
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
    cam->setPosition(activatedRadius * (axisZ + axisY / 2));
    cam->setUpVector(axisY);
    cam->setViewCenter(zeroP);
    lightTrans->setTranslation(cam->position());
    emit activatedRadiusChanged(activatedRadius);
}

void Mol3DWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Space:
            emit spaceOrEnterPressed(event->key() == Qt::Key_Space);
            break;
        default:
            Qt3DExtras::Qt3DWindow::keyReleaseEvent(event);
    }
}

void Mol3DWindow::wheelEvent(QWheelEvent *) {
}
