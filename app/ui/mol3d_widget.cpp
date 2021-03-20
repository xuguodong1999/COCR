#include "mol3d_widget.hpp"
#include "mol3d_window.hpp"
#include "math_util.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QHBoxLayout>
#include <QDebug>

#include <optional>

using xgd::MathUtil;
using std::fabs;

inline QVector3D getQVector3D(const xgd::JAtom &_atom) {
    return {_atom.xx, _atom.yy, _atom.zz};
}

inline QVector3D getQVector3D(const std::shared_ptr<xgd::JAtom> &_atom) {
    return getQVector3D(*_atom);
}

class Mol3DBuilder : public QObject {
    Qt3DCore::QEntity *root;
    std::shared_ptr<xgd::JMol> mol;
    int sphereRings, sphereSlices, cylinderRings, cylinderSlices;
    float doubleBondSpaceScale, tripleBondSpaceScale;

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

        void setColor(const QColor &_color) {
            material->setDiffuse(_color);
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
        std::vector<Qt3DExtras::QPhongMaterial *>materials;
        inline static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy = {
                {2, {MathUtil::getOneZ3(), -MathUtil::getOneZ3()}},
                {3, {MathUtil::getOneZ3(), MathUtil::getZero3(), -MathUtil::getOneZ3()}}
        };
    public:
        MultiCylinderEntity(Qt3DCore::QEntity *_root, int _num = 1) : BaseEntity(_root) {
            for (int i = 0; i < _num; i++) {
                cylinders.emplace_back(entity);
            }
            setDistance(1);
        }
        void setColor(const QColor &_color) {
            for(auto&cylinder:cylinders) {
                cylinder.setColor(_color);
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
                cylinders[i].setTranslation(_distance * translations[i]);
            }
        }

        void setDirection(const QVector3D &_from, const QVector3D &_to,
                          const std::optional<QVector3D> &_norm = std::nullopt) {
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
        }

    private:
        void setTranslation(const QVector3D &_trans) {
            BaseEntity::setTranslation(_trans);
        }
    };

    std::unordered_map<size_t, std::shared_ptr<SphereEntity>> atoms;
    std::unordered_map<size_t, std::shared_ptr<BaseEntity>> bonds;
public:
    Mol3DBuilder(QObject *parent, Qt3DCore::QEntity *_root) : QObject(parent), root(_root) {}

    void build(std::shared_ptr<xgd::JMol> _mol, const QVector3D &_viewSize,
               const QVector3D &_blankArea = {5, 5, 5}) {
        using namespace xgd;
        qDebug()<<"build";
        mol = std::move(_mol);
        mol->norm3D(_viewSize.x(), _viewSize.y(), _viewSize.z(),
                    _blankArea.x() / 2, _blankArea.y() / 2, _blankArea.z() / 2, true);
        // 坐标系
//    QVector3D offset(-baseSize / 3, baseSize / 3, 0);
//    getSingleCylinderEntity(offset + zeroP, offset + axisX * 10, 0.2, Qt::blue);
//    getSingleCylinderEntity(offset + zeroP, offset + axisY * 10, 0.2, Qt::red);
//    getSingleCylinderEntity(offset + zeroP, offset + axisZ * 10, 0.2, Qt::green);

        // 添加3D原子球
        mol->loopAtomVec([&](xgd::JAtom &atom) {
            auto entity = std::make_shared<SphereEntity>(root);
            atoms[atom.getId()] = entity;
            qDebug()<<getQVector3D(atom);
            entity->setTranslation(getQVector3D(atom));
            entity->setColor(xgd::getColor(atom.getType()));
            entity->setRadius(10);
            entity->setScale(1);
            entity->setRindsAndSlices(100, 100);
        });
        // 统计共轭键的共面原子，计算用于修正双键的中轴旋转角度
        std::unordered_map<size_t, QVector3D> normVecMap;
        std::unordered_map<size_t, std::vector<size_t>> neighborMap;
        // 初始化原子的邻接原子
        mol->loopBondVec([&](JBond &bond) {
            auto from = bond.getFrom()->getId(), to = bond.getTo()->getId();
            neighborMap[from].push_back(to);
            neighborMap[to].push_back(from);
        });
        // 寻找需要共面的共轭键
        mol->loopBondVec([&](JBond &bond) {
            switch (bond.getType()) {
                case BondType::DoubleBond:
                case BondType::TripleBond:
                    break;
                default:
                    return;
            }
            size_t from = bond.getFrom()->getId(), to = bond.getTo()->getId();
            std::vector<QVector3D> poses;
            std::unordered_set<size_t> aids;
            auto update_pos = [&](const size_t &aid) {
                auto it = neighborMap.find(aid);
                if (it != neighborMap.end()) {
                    for (auto &neb_id:it->second) {
                        auto nebIt = aids.find(neb_id);
                        if (aids.end() == nebIt) {
                            aids.insert(neb_id);
                            auto atom = mol->getAtom(neb_id);
                            poses.push_back(getQVector3D(atom));
                        }
                    }
                }
            };
            update_pos(from);
            update_pos(to);
            if (poses.size() >= 3) {
                normVecMap[bond.getId()] = QVector3D::crossProduct(poses[0] - poses[1], poses[1] - poses[2]);
            }
        });
        mol->loopBondVec([&](JBond &bond) {
            auto fromAtom = bond.getFrom(), toAtom = bond.getTo();
            QVector3D from = getQVector3D(fromAtom), to = getQVector3D(toAtom);
            switch (bond.getType()) {
                case BondType::SingleBond:
                case BondType::DelocalizedBond:
                case BondType::DownBond:
                case BondType::UpBond:
                case BondType::ImplicitBond: {
                    auto entity = std::make_shared<CylinderEntity>(root);
                    bonds[bond.getId()] = entity;
                    entity->setDirection(from, to);
                    break;
                }
                case BondType::DoubleBond: {
                    auto entity = std::make_shared<MultiCylinderEntity>(root, 2);
                    bonds[bond.getId()] = entity;
                    std::optional<QVector3D> norm = std::nullopt;
                    auto it = normVecMap.find(bond.getId());
                    if (it != normVecMap.end()) {
                        norm = it->second;
                    }
                    entity->setDistance(0.1);
                    entity->setRindsAndSlices(100,100);
                    entity->setScale(1);
                    entity->setDirection(from, to, norm);
                    break;
                }
                case BondType::TripleBond: {
                    auto entity = std::make_shared<MultiCylinderEntity>(root, 3);
                    bonds[bond.getId()] = entity;
                    std::optional<QVector3D> norm = std::nullopt;
                    auto it = normVecMap.find(bond.getId());
                    if (it != normVecMap.end()) {
                        norm = it->second;
                    }
                    entity->setDistance(0.1);
                    entity->setRindsAndSlices(100,100);
                    entity->setScale(1);
                    entity->setDirection(from, to, norm);
                    break;
                }
                default: {
                    exit(-1);
                }
            }
        });
    }
};

Mol3DWidget::Mol3DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol) : QWidget(parent), mol(std::move(_mol)) {
    root = new Qt3DCore::QEntity();
    builder = new Mol3DBuilder(this, root);
    window = new Mol3DWindow(root);
    auto layout = new QHBoxLayout();
    layout->addWidget(QWidget::createWindowContainer(window));
    layout->setMargin(2);
    setLayout(layout);
}

void Mol3DWidget::syncMolToScene() {
    qDebug()<<"syncMolToScene:"<<window->getViewSize();
    builder->build(mol, window->getViewSize());
}
