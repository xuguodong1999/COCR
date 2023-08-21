#include "WrapperSingle.h"

#include "EntityBase.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>

SingleWrapper::SingleWrapper(Qt3DCore::QEntity *_root) :
        BaseWrapper(_root), material(new Qt3DExtras::QPhongMaterial(_root)) {
    entity->addComponent(material);
}

void SingleWrapper::setColor(const QColor &_color) {
    material->setDiffuse(_color);
}
