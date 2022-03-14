#include "3d/wrapper_cylinder.hpp"
#include "3d/entity_base.hpp"

#include "math_util.hpp"

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QTransform>

using cocr::MathUtil;

CylinderWrapper::CylinderWrapper(Qt3DCore::QEntity *_root) :
        SingleWrapper(_root), cylinder(new Qt3DExtras::QCylinderMesh(entity)) {
    entity->addComponent(cylinder);
    entity->setType(EntityType::BondCylinder);
}

void CylinderWrapper::setDirection(const QVector3D &_from, const QVector3D &_to) {
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
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