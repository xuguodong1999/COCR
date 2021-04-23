#include "wrapper_base.hpp"
#include "entity_base.hpp"
#include <Qt3DCore/QTransform>

BaseWrapper::BaseWrapper(Qt3DCore::QEntity *_root) : root(_root), transform(new Qt3DCore::QTransform(_root)) {
    entity = new BaseEntity(root);
    entity->addComponent(transform);
}

void BaseWrapper::setScale(const float &_scale) {
    transform->setScale(_scale);
}

void BaseWrapper::setTranslation(const QVector3D &_trans) {
    transform->setTranslation(_trans);
}

void BaseWrapper::setId(const size_t &_id) {
    entity->setId(_id);
}

void BaseWrapper::setObjectName(const QString &_name) {
    QObject::setObjectName(_name);
    entity->setObjectName(_name);
}

BaseWrapper::~BaseWrapper() {
//    qDebug() << "~BaseWrapper()";
}