#include "WrapperCylinder.h"
#include "EntityBase.h"

#include "MathUtil.h"

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QTransform>


CylinderWrapper::CylinderWrapper(Qt3DCore::QEntity *_root) :
        SingleWrapper(_root), cylinder(new Qt3DExtras::QCylinderMesh(entity)) {
    entity->addComponent(cylinder);
    entity->setType(EntityType::BondCylinder);
}

void CylinderWrapper::setDirection(const QVector3D &_from, const QVector3D &_to) {
    // put the cylinder on Y axis, center on 0
    transform->setRotation(QQuaternion::rotationTo(MathUtil::getOneY3(), _from - _to));
    transform->setTranslation((_from + _to) / 2.0);
    float length = _from.distanceToPoint(_to);
    setLength(length);
}

void CylinderWrapper::setRindsAndSlices(const int &_rings, const int &_slices) {
    cylinder->setRings(_rings);
    cylinder->setSlices(_slices);
}

void CylinderWrapper::setLength(const float &_length) {
    cylinder->setLength(_length);
}

void CylinderWrapper::setRadius(const float &_radius) {
    cylinder->setRadius(_radius);
}