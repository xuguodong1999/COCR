#include "3d/wrapper_sphere.h"
#include "3d/entity_base.h"

#include <Qt3DExtras/QSphereMesh>

SphereWrapper::SphereWrapper(Qt3DCore::QEntity *_root) :
        SingleWrapper(_root), sphere(new Qt3DExtras::QSphereMesh(_root)) {
    entity->addComponent(sphere);
    entity->setType(EntityType::AtomSphere);
}

void SphereWrapper::setRadius(const float &_radius) {
    sphere->setRadius(_radius);
}

void SphereWrapper::setRindsAndSlices(const int &_rings, const int &_slices) {
    sphere->setRings(_rings);
    sphere->setSlices(_slices);
}
