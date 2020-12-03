#include "mol3d.hpp"

#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

Mol3D::Mol3D(Qt3DCore::QEntity *rootEntity)
        : mRootEntity(rootEntity) {
    // Cylinder shape data
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(1);
    cylinder->setLength(3);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    // CylinderMesh Transform
    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.5f);
    cylinderTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
    cylinderTransform->setTranslation(QVector3D(-5.0f, 4.0f, -1.5));

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(QColor(QRgb(0x928327)));

    // Cylinder
    m_cylinderEntity = new Qt3DCore::QEntity(mRootEntity);
    m_cylinderEntity->addComponent(cylinder);
    m_cylinderEntity->addComponent(cylinderMaterial);
    m_cylinderEntity->addComponent(cylinderTransform);


    // Sphere
    m_sphereEntity = getSphereEntity(
            QVector3D(-5.0f, -4.0f, 0.0f), 2, QColor(QRgb(0xa69929)));
}

Mol3D::~Mol3D() {
}


void Mol3D::enableCylinder(bool enabled) {
    m_cylinderEntity->setEnabled(enabled);
}

void Mol3D::enableSphere(bool enabled) {
    m_sphereEntity->setEnabled(enabled);
}

Qt3DCore::QEntity *Mol3D::getSphereEntity(
        const QVector3D &_center, const float &_radius, const QColor &_color) {
    auto *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    sphereMesh->setRadius(_radius);

    auto *sphereTransform = new Qt3DCore::QTransform();
    sphereTransform->setScale(1.0f);
    sphereTransform->setTranslation(_center);

    auto *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(_color);

    auto sphereEntity = new Qt3DCore::QEntity(mRootEntity);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereMaterial);
    sphereEntity->addComponent(sphereTransform);
    return sphereEntity;
}

Qt3DCore::QEntity *
Mol3D::getCylinderEntity(const QVector3D &_from, const QVector3D &_to,
                         const float &_radius, const QColor &_color) {
    return nullptr;
}
