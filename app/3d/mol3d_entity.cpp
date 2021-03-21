#include "mol3d_entity.hpp"

#include "math_util.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <QDebug>
#include <cmath>

using xgd::MathUtil;
using std::fabs;

std::unordered_map<decltype(MultiCylinderEntity::cylinders.size()), std::vector<QVector3D>>
        MultiCylinderEntity::sTransStrategy = {
        {2, {MathUtil::getOneZ3(), -MathUtil::getOneZ3()}},
        {3, {MathUtil::getOneZ3(), MathUtil::getZero3(), -MathUtil::getOneZ3()}}
};

MultiCylinderEntity::MultiCylinderEntity(Qt3DCore::QEntity *_root, int _num) : BaseEntity(_root) {
    for (int i = 0; i < _num; i++) {
        cylinders.push_back(std::make_shared<CylinderEntity>(entity));
    }
    setDistance(1);
}

void MultiCylinderEntity::setColor(const QColor &_color) {
    for (auto &cylinder:cylinders) {
        cylinder->setColor(_color);
    }
}

void MultiCylinderEntity::setRindsAndSlices(const int &_rings, const int &_slices) {
    for (auto &cylinder:cylinders) {
        cylinder->setRindsAndSlices(_rings, _slices);
    }
    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &MultiCylinderEntity::onEntityClicked);
}

void MultiCylinderEntity::setDistance(const float &_distance) {
    auto it = sTransStrategy.find(cylinders.size());
    if (sTransStrategy.end() == it) {
        return;
    }
    auto &translations = it->second;
    if (translations.size() != cylinders.size()) { return; }
    for (size_t i = 0; i < cylinders.size(); i++) {
        cylinders[i]->setTranslation(_distance * translations[i]);
    }
}

void MultiCylinderEntity::setDirection(const QVector3D &_from, const QVector3D &_to,
                                       const std::optional<QVector3D> &_norm) {
    // 将平行圆柱体整体平移、旋转到目标位置
    // 四元数旋转的合成：依次旋转q1、q2 推导为 q2 x q1
    // 键角度调整策略：遍历获取所有需要调整的键，为每个键从三个共面原子计算一个法向量
    QQuaternion rotation = QQuaternion::rotationTo(MathUtil::getOneY3(), _from - _to);
    if (_norm) {
        QVector3D originVec = MathUtil::getOneX3(),
                normVec = (rotation.inverted() * _norm.value()).normalized();
        float angle = 180.0 / M_PI * acos(QVector3D::dotProduct(normVec, originVec));
        auto preRotation = QQuaternion::fromAxisAndAngle(MathUtil::getOneY3(), angle);
        QVector3D rotatedVec = (preRotation * originVec).normalized();
        const float thresh = 0.05;
        auto is_rough_eq = [&](const QVector3D &_v1, const QVector3D &_v2) -> bool {
            return fabs(_v1.x() - _v2.x()) < thresh &&
                   fabs(_v1.y() - _v2.y()) < thresh &&
                   fabs(_v1.z() - _v2.z()) < thresh;
        };
        if (!is_rough_eq(rotatedVec, normVec) && !is_rough_eq(rotatedVec, -normVec)) {
            rotation *= QQuaternion::fromAxisAndAngle(MathUtil::getOneY3(), -angle);
        } else {
            rotation *= preRotation;
        }
    }
    transform->setRotation(rotation);
    setTranslation((_from + _to) / 2.0);
    float length = _from.distanceToPoint(_to);
    for (auto &cylinder:cylinders) {
        cylinder->setLength(length);
    }
}


void MultiCylinderEntity::setTranslation(const QVector3D &_trans) {
    BaseEntity::setTranslation(_trans);
}

void MultiCylinderEntity::setRadius(const float &_radius) {
    for (auto &cylinder:cylinders) {
        cylinder->setRadius(_radius);
    }
}

void MultiCylinderEntity::onEntityClicked() {
    qDebug() << "bond multi cylinder " << id << " clicked";
    emit sig_bond_picked(id);
}


CylinderEntity::CylinderEntity(Qt3DCore::QEntity *_root) :
        BaseSingleEntity(_root), cylinder(new Qt3DExtras::QCylinderMesh()) {
    entity->addComponent(cylinder);
    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &CylinderEntity::onEntityClicked);
}

void CylinderEntity::setDirection(const QVector3D &_from, const QVector3D &_to) {
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    transform->setRotation(QQuaternion::rotationTo(MathUtil::getOneY3(), _from - _to));
    transform->setTranslation((_from + _to) / 2.0);
    float length = _from.distanceToPoint(_to);
    setLength(length);
}

void CylinderEntity::setRindsAndSlices(const int &_rings, const int &_slices) {
    cylinder->setRings(_rings);
    cylinder->setSlices(_slices);
}

void CylinderEntity::setLength(const float &_length) {
    cylinder->setLength(_length);
}

void CylinderEntity::setRadius(const float &_radius) {
    cylinder->setRadius(_radius);
}

void CylinderEntity::onEntityClicked() {
    qDebug() << "bond cylinder " << id << " clicked";
    emit sig_bond_picked(id);
}


SphereEntity::SphereEntity(Qt3DCore::QEntity *_root) :
        BaseSingleEntity(_root) {
    sphere = new Qt3DExtras::QSphereMesh(entity);
    entity->addComponent(sphere);
    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &SphereEntity::onEntityClicked);
//    connect(mPicker, &Qt3DRender::QObjectPicker::clicked, [&](Qt3DRender::QPickEvent*e){
//        qDebug()<<"fuck";
//    });
}

void SphereEntity::setRadius(const float &_radius) {
    sphere->setRadius(_radius);
}

void SphereEntity::setRindsAndSlices(const int &_rings, const int &_slices) {
    sphere->setRings(_rings);
    sphere->setSlices(_slices);
}

void SphereEntity::setColor(const QColor &_color) {
    material->setDiffuse(_color);
}

void SphereEntity::onEntityClicked() {
    qDebug() << "atom sphere " << id << " clicked";
    emit sig_atom_picked(id);
}

BaseSingleEntity::BaseSingleEntity(Qt3DCore::QEntity *_root) :
        BaseEntity(_root), material(new Qt3DExtras::QPhongMaterial()) {
    entity->addComponent(material);
}

void BaseSingleEntity::setColor(const QColor &_color) {
    material->setDiffuse(_color);
}

BaseEntity::BaseEntity(Qt3DCore::QEntity *_root) :
        root(_root),
        mMouseDevice(new Qt3DInput::QMouseDevice()),
        mMouseHandler(new Qt3DInput::QMouseHandler()) {
    mMouseHandler->setSourceDevice(mMouseDevice);

    entity = new Qt3DCore::QEntity(root);
    transform = new Qt3DCore::QTransform(root);

    mPicker = new Qt3DRender::QObjectPicker(entity);
    mPicker->setHoverEnabled(false);
    mPicker->setDragEnabled(false);

    entity->addComponent(transform);
    entity->addComponent(mPicker);
//    entity->addComponent(mMouseHandler);
}

void BaseEntity::setScale(const float &_scale) {
    transform->setScale(_scale);
}

void BaseEntity::setTranslation(const QVector3D &_trans) {
    transform->setTranslation(_trans);
}

void BaseEntity::setId(const size_t &_id) {
    id = _id;
}
