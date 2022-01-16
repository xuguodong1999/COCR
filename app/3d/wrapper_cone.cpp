#include "wrapper_cone.hpp"
#include "entity_base.hpp"

#include "math_util.hpp"

#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QConeMesh>

using xgd::MathUtil;

ConeWrapper::ConeWrapper(Qt3DCore::QEntity *_root) : SingleWrapper(_root), cone(new Qt3DExtras::QConeMesh()) {
    entity->addComponent(cone);
    entity->setType(EntityType::ConeMesh);
}

void ConeWrapper::setRadius(const float &_radius) {
    cone->setTopRadius(0);
    cone->setBottomRadius(_radius);
}

void ConeWrapper::setHeight(const float &_height) {
    cone->setLength(_height);
}

void ConeWrapper::setDirection(const QVector3D &_from, const QVector3D &_to) {
    transform->setRotation(QQuaternion::rotationTo(MathUtil::getOneY3(), _to - _from));
    transform->setTranslation((_from + _to) / 2.0);
    float length = _from.distanceToPoint(_to);
    setHeight(length);
}

void ConeWrapper::setRindsAndSlices(const int &_rings, const int &_slices) {
    cone->setRings(_rings);
    cone->setSlices(_slices);
}