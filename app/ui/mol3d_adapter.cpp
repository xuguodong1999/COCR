#include "mol3d_adapter.hpp"
#include <Qt3DCore/QEntity>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseEvent>
#include <QDebug>

using namespace xgd;

Mol3DAdapter::Mol3DAdapter(std::shared_ptr<JMol> _mol, Qt3DCore::QEntity *_root)
        : mol(std::move(_mol)), root(_root) {
    if (!root) {
        root = new Qt3DCore::QEntity(Q_NULLPTR);
    }
}

Mol3DAdapter::~Mol3DAdapter() {
    if (!root->parent()) {
        delete root;
        root = Q_NULLPTR;
    }
}

void Mol3DAdapter::onBondSelected(const size_t &_bid) {

}

Qt3DCore::QEntity *Mol3DAdapter::getSphereEntity(
        const size_t &_aid, const QVector3D &_center, const float &_radius, const QColor &_color) {
    auto entity = new Qt3DCore::QEntity(root);
    auto handler = new Qt3DInput::QMouseHandler(entity);
    connect(handler, &Qt3DInput::QMouseHandler::clicked, this, [&](Qt3DInput::QMouseEvent *mouse) {
        qDebug() << "clicked@ " << mouse->x() << "," << mouse->y();
        entityAtomMap[entity] = _aid;
    });
    return nullptr;
}

Qt3DCore::QEntity *
Mol3DAdapter::getSingleCylinderEntity(const QVector3D &_from, const QVector3D &_to, const float &_radius,
                                      const QColor &_color) {
    return nullptr;
}

Qt3DCore::QEntity *
Mol3DAdapter::getDoubleCylinderEntity(const QVector3D &_from, const QVector3D &_to, const float &_radius,
                                      const QColor &_color, const std::optional<QVector3D> &_normVec) {
    return nullptr;
}

Qt3DCore::QEntity *
Mol3DAdapter::getTripleCylinderEntity(const QVector3D &_from, const QVector3D &_to, const float &_radius,
                                      const QColor &_color, const std::optional<QVector3D> &_normVec) {
    return nullptr;
}
