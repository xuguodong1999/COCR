#include "mol3d_entity.hpp"

#include "math_util.hpp"

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
#include <QPickingSettings>

using xgd::MathUtil;
using std::fabs;

std::unordered_map<decltype(MultiCylinderWrapper::cylinders.size()), std::vector<QVector3D>>
        MultiCylinderWrapper::sTransStrategy = {
        {2, {MathUtil::getOneZ3(), -MathUtil::getOneZ3()}},
        {3, {MathUtil::getOneZ3(), MathUtil::getZero3(), -MathUtil::getOneZ3()}}
};

MultiCylinderWrapper::MultiCylinderWrapper(Qt3DCore::QEntity *_root, int _num) : BaseWrapper(_root) {
    for (int i = 0; i < _num; i++) {
        cylinders.push_back(std::make_shared<CylinderWrapper>(entity));
    }
    setDistance(1);
    entity->setType(EntityType::BondCylinder);
}

void MultiCylinderWrapper::setColor(const QColor &_color) {
    for (auto &cylinder:cylinders) {
        cylinder->setColor(_color);
    }
}

void MultiCylinderWrapper::setRindsAndSlices(const int &_rings, const int &_slices) {
    for (auto &cylinder:cylinders) {
        cylinder->setRindsAndSlices(_rings, _slices);
    }
//    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &MultiCylinderWrapper::onEntityClicked);
}

void MultiCylinderWrapper::setDistance(const float &_distance) {
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

void MultiCylinderWrapper::setDirection(const QVector3D &_from, const QVector3D &_to,
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


void MultiCylinderWrapper::setTranslation(const QVector3D &_trans) {
    BaseWrapper::setTranslation(_trans);
}

void MultiCylinderWrapper::setRadius(const float &_radius) {
    for (auto &cylinder:cylinders) {
        cylinder->setRadius(_radius);
    }
}

CylinderWrapper::CylinderWrapper(Qt3DCore::QEntity *_root) :
        SingleWrapper(_root), cylinder(new Qt3DExtras::QCylinderMesh()) {
    entity->addComponent(cylinder);
    entity->setType(EntityType::BondCylinder);
//    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &CylinderWrapper::onEntityClicked);
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

SphereWrapper::SphereWrapper(Qt3DCore::QEntity *_root) :
        SingleWrapper(_root) {
    sphere = new Qt3DExtras::QSphereMesh(entity);
    entity->addComponent(sphere);
    entity->setType(EntityType::AtomSphere);
//    connect(mMouseHandler, &Qt3DInput::QMouseHandler::clicked, this, &SphereWrapper::onEntityClicked);
//    connect(mPicker, &Qt3DRender::QObjectPicker::pressed, [&](Qt3DRender::QPickEvent *e) {
//        qDebug() << "fuck";
//    });
}

void SphereWrapper::setRadius(const float &_radius) {
    sphere->setRadius(_radius);
}

void SphereWrapper::setRindsAndSlices(const int &_rings, const int &_slices) {
    sphere->setRings(_rings);
    sphere->setSlices(_slices);
}

void SphereWrapper::setColor(const QColor &_color) {
    material->setDiffuse(_color);
}

SingleWrapper::SingleWrapper(Qt3DCore::QEntity *_root) :
        BaseWrapper(_root), material(new Qt3DExtras::QPhongMaterial()) {
    entity->addComponent(material);
}

void SingleWrapper::setColor(const QColor &_color) {
    material->setDiffuse(_color);
}

BaseWrapper::BaseWrapper(Qt3DCore::QEntity *_root) : root(_root) {
    entity = new BaseEntity(root);
    transform = new Qt3DCore::QTransform(entity);
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

BaseEntity::BaseEntity(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent) {
    mPicker = new Qt3DRender::QObjectPicker(this);
    mPicker->setHoverEnabled(false);
    mPicker->setDragEnabled(false);
    mPicker->setEnabled(true);
    addComponent(mPicker);
    connect(mPicker, &Qt3DRender::QObjectPicker::clicked, this, &BaseEntity::onPicked);

    Qt3DRender::QPickingSettings *settings = new Qt3DRender::QPickingSettings(mPicker);
    settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
}

void BaseEntity::onPicked(Qt3DRender::QPickEvent *event) {
    switch (type) {
        case EntityType::AtomSphere: {
            qDebug() << "atom " << id << " picked";
            emit sig_atom_picked(id);
            break;
        }
        case EntityType::BondCylinder: {
            qDebug() << "bond " << id << " picked";
            emit sig_bond_picked(id);
            break;
        }
        default:
            break;
    }
}

void BaseEntity::setType(const EntityType &_type) {
    type = _type;
}

void BaseEntity::setId(const size_t &_id) {
    id = _id;
}
