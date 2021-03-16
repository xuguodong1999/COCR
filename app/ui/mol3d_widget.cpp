#include "mol3d_widget.hpp"
#include "mol3d_window.hpp"
#include "math_util.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QHBoxLayout>

#include <optional>

using xgd::MathUtil;

class Mol3DBuilder : public QObject {
    Qt3DCore::QEntity *root;
    int sphereRings, sphereSlices, cylinderRings, cylinderSlices;
    float doubleBondSpaceScale, tripleBondSpaceScale;
    std::unordered_map<size_t, Qt3DCore::QEntity *> mAtomEntities;
    std::unordered_multimap<size_t, Qt3DCore::QEntity *> mBondEntities;

    class BaseEntity {
    protected:
        Qt3DCore::QEntity *root;
        Qt3DCore::QEntity *entity;
        Qt3DCore::QTransform *transform;
    public:
        BaseEntity(Qt3DCore::QEntity *_root) : root(_root) {
            entity = new Qt3DCore::QEntity(root);
            transform = new Qt3DCore::QTransform();
            entity->addComponent(transform);
        }

        void setScale(const float &_scale) {
            transform->setScale(_scale);
        }

        void setTranslation(const QVector3D &_trans) {
            transform->setTranslation(_trans);
        }

        virtual void setRindsAndSlices(const int &_rings, const int &_slices) = 0;
    };

    class BaseSingleEntity : public BaseEntity {
    protected:
        Qt3DExtras::QPhongMaterial *material;
    public:
        BaseSingleEntity(Qt3DCore::QEntity *_root) : BaseEntity(_root) {
            material = new Qt3DExtras::QPhongMaterial();
            entity->addComponent(material);
        }

        void setColor(const QColor &_color) {
            material->setDiffuse(_color);
        }
    };

    class SphereEntity : public BaseSingleEntity {
        Qt3DExtras::QSphereMesh *sphere;
    public:
        SphereEntity(Qt3DCore::QEntity *_root) : BaseSingleEntity(_root) {
            sphere = new Qt3DExtras::QSphereMesh();
            entity->addComponent(sphere);
        }

        void setRadius(const float &_radius) {
            sphere->setRadius(_radius);
        }

        void setRindsAndSlices(const int &_rings, const int &_slices) override {
            sphere->setRings(_rings);
            sphere->setSlices(_slices);
        }
    };

    class CylinderEntity : public BaseSingleEntity {
        Qt3DExtras::QCylinderMesh *cylinder;
    public:
        CylinderEntity(Qt3DCore::QEntity *_root) : BaseSingleEntity(_root) {
            cylinder = new Qt3DExtras::QCylinderMesh();
            entity->addComponent(cylinder);
        }

        void setDirection(const QVector3D &_from, const QVector3D &_to) {
            // 内置圆柱中轴线在y轴上，重心位于坐标原点
            transform->setRotation(QQuaternion::rotationTo(MathUtil::getOneY3(), _from - _to));
            transform->setTranslation((_from + _to) / 2.0);
        }

        void setRindsAndSlices(const int &_rings, const int &_slices) override {
            cylinder->setRings(_rings);
            cylinder->setSlices(_slices);
        }

        void setLength(const float &_length) {
            cylinder->setLength(_length);
        }

        void setRadius(const float &_radius) {
            cylinder->setRadius(_radius);
        }
    };

    class MultiCylinderEntity : public BaseEntity {
        std::vector<CylinderEntity> cylinders;
        inline static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy = {
                {2, {MathUtil::getOneZ3(), -MathUtil::getOneZ3()}},
                {3, {MathUtil::getOneZ3(), MathUtil::getZero3(), -MathUtil::getOneZ3()}}
        };
    public:
        MultiCylinderEntity(Qt3DCore::QEntity *_root, int _num = 1) : BaseEntity(_root) {
            for (int i = 0; i < _num; i++) {
                cylinders.emplace_back(entity);
            }
        }

        void setRindsAndSlices(const int &_rings, const int &_slices) override {
            for (auto &cylinder:cylinders) {
                cylinder.setRindsAndSlices(_rings, _slices);
            }
        }

        void setDistance(const float &_distance) {
            auto it = sTransStrategy.find(cylinders.size());
            if (sTransStrategy.end() == it) {
                return;
            }
            auto &translations = it->second;
            if (translations.size() != cylinders.size()) { return; }
            for (size_t i = 0; i < cylinders.size(); i++) {
                cylinders[i].setTranslation(translations[i]);
            }
        }

        void setDirection(const QVector3D &_from, const QVector3D &_to,
                          const std::optional<QVector3D> &_norm = std::nullopt) {
            // 将平行圆柱体整体平移、旋转到目标位置
            // 四元数旋转的合成：依次旋转q1、q2 推导为 q2 x q1
            // 键角度调整策略：遍历获取所有需要调整的键，为每个键从三个共面原子计算一个法向量
            QQuaternion rotation = QQuaternion::rotationTo(MathUtil::getOneY3(), _from - _to);
            if (_norm) {
                auto originVec = MathUtil::getOneX3();
                auto normVec = (rotation.inverted() * _norm.value()).normalized();
                float angle = 180.0 / M_PI * acos(QVector3D::dotProduct(normVec, originVec));
                auto preRotation = QQuaternion::fromAxisAndAngle(MathUtil::getOneY3(), angle);
                auto roVec = (preRotation * originVec).normalized();
                const float thresh = 0.05;
                auto cmp_vec3d = [&](const QVector3D &_v1, const QVector3D &_v2) -> bool {
                    return fabs(_v1.x() - _v2.x()) < thresh &&
                           fabs(_v1.y() - _v2.y()) < thresh &&
                           fabs(_v1.z() - _v2.z()) < thresh;
                };
                if (!cmp_vec3d(roVec, normVec) && !cmp_vec3d(roVec, -normVec)) {
                    rotation *= QQuaternion::fromAxisAndAngle(MathUtil::getOneY3(), -angle);
                } else {
                    rotation *= preRotation;
                }
            }
            transform->setRotation(rotation);
            transform->setTranslation((_from + _to) / 2.0);
        }
    };

public:
    Mol3DBuilder(QObject *parent, Qt3DCore::QEntity *_root) : QObject(parent), root(_root) {}

    void build(const xgd::JMol &_mol) {

    }
};

Mol3DWidget::Mol3DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol) : QWidget(parent), mol(std::move(_mol)) {
    builder = new Mol3DBuilder(this, root);
    root = new Qt3DCore::QEntity();
    window = new Mol3DWindow(root);
    auto layout = new QHBoxLayout();
    layout->addWidget(QWidget::createWindowContainer(window));
    layout->setMargin(2);
    setLayout(layout);
}

void Mol3DWidget::syncMolToScene() {
    builder->build(*mol);
}
