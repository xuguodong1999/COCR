#include "mol3d.hpp"
#include "qt_util.hpp"

#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

Mol3D::Mol3D(JMol &_mol, Qt3DCore::QEntity *_rootEntity)
        : mol(_mol), mRootEntity(_rootEntity) {
    // 坐标系
    QVector3D offset(-30, 30, 0);
    getCylinderEntity(offset + zeroP, offset + axisX * 10, 1, Qt::blue);
    getCylinderEntity(offset + zeroP, offset + axisY * 10, 1, Qt::red);
    getCylinderEntity(offset + zeroP, offset + axisZ * 10, 1, Qt::green);

//    getSphereEntity(zeroP2, Qt::black);
    auto pos3DMap = mol.get3DCoordinates(true);
    std::cout << mol.toSMILES(false) << std::endl;
//    std::cout<<mol.getAtomsMap().size()<<std::endl;
//    std::cout<<mol.getBondsMap().size()<<std::endl;
    auto convert = [](const cv::Point3f &cvPts) -> QVector3D {
        return QVector3D(cvPts.x, cvPts.y, cvPts.z);
    };
    auto addAtomEntity = [&](const size_t &_aid) -> void {
        cv::Point3f pos = 5 * pos3DMap[_aid];
        qDebug() << convert(pos);
        getSphereEntity(convert(pos), 3, Qt::darkYellow);
    };
    mol.safeTraverseAtoms(addAtomEntity);
    auto addBondEntity = [&](const size_t &_bid) -> void {
        auto &bond = mol.getBondsMap().at(_bid);
        cv::Point3f from = 5 * pos3DMap[bond->getAtomFrom()];
        cv::Point3f to = 5 * pos3DMap[bond->getAtomTo()];
        getCylinderEntity(convert(from), convert(to), 0.5, Qt::darkRed);
    };
    mol.safeTraverseBonds(addBondEntity);
}

Mol3D::~Mol3D() {
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

Qt3DCore::QEntity *Mol3D::getCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color) {
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(_from.distanceToPoint(_to));
    cylinder->setRings(100);
    cylinder->setSlices(20);

    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation((_from + _to) / 2.0);

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
    return cylinderEntity;
}
