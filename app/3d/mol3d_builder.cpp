#include "mol3d_builder.hpp"
#include "entity_base.hpp"
#include "wrapper_cone.hpp"
#include "wrapper_sphere.hpp"
#include "wrapper_cylinder.hpp"
#include "wrapper_multi_cylinder.hpp"

#include "chem/jmol.hpp"

#include "math_util.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QDebug>

#include <optional>

using xgd::MathUtil;

inline QVector3D getQVector3D(const xgd::JAtom &_atom) {
    return {_atom.xx, _atom.yy, _atom.zz};
}

inline QVector3D getQVector3D(const std::shared_ptr<xgd::JAtom> &_atom) {
    return getQVector3D(*_atom);
}


void Mol3DBuilder::prepare(std::shared_ptr<xgd::JMol> _mol, const QVector3D &_viewSize, const QVector3D &_blankArea) {
    qDebug() << __FUNCTION__;
    mol = std::move(_mol);
    mol->norm3D(_viewSize.x(), _viewSize.y(), _viewSize.z(),
                _blankArea.x() / 2, _blankArea.y() / 2, _blankArea.z() / 2, true);
    emit sig_mol_prepare_done();
}

void Mol3DBuilder::build() {
    qDebug() << __FUNCTION__;
    using namespace xgd;
    // 坐标系
//    buildAxis(0, 0, 0, 100);
    resetMolRoot();
    float avgBondLength = mol->getAvgBondLength();
    if (avgBondLength < 1)avgBondLength = 20;
    // 添加3D原子球
    mol->loopAtomVec([&](xgd::JAtom &atom) {
//        return;
        auto wrapper = std::make_shared<SphereWrapper>(molRoot);
        atoms[atom.getId()] = wrapper;
//        qDebug() << getQVector3D(atom);
        wrapper->setId(atom.getId());
        wrapper->setTranslation(getQVector3D(atom));
        wrapper->setColor(xgd::getColor(atom.getType()));
        wrapper->setRadius(atom.getRadius() / atom.getDefaultRadius() * avgBondLength / 3);
        wrapper->setScale(1);
        wrapper->setRindsAndSlices(100, 100);
        wrapper->setObjectName(atom.getName().c_str() + QString(":%0").arg(atom.getId()));
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
        std::vector<QVector3D> poses;
        switch (bond.getType()) {
            case BondType::DoubleBond:
                poses = {getQVector3D(bond.getFrom()), getQVector3D(bond.getTo())};
                break;
            case BondType::TripleBond:
                break;
            default:
                return;
        }
        size_t from = bond.getFrom()->getId(), to = bond.getTo()->getId();
        std::unordered_set<size_t> aids = {from, to};
        auto collect_neb_pos = [&](const size_t &aid) {
            auto it = neighborMap.find(aid);
            if (it == neighborMap.end()) { return; }
            for (auto &neb_id:it->second) {
                auto nebIt = aids.find(neb_id);
                if (aids.end() != nebIt) { continue; }
                aids.insert(neb_id);
                auto atom = mol->getAtom(neb_id);
                poses.push_back(getQVector3D(atom));
            }
        };
        collect_neb_pos(from);
        collect_neb_pos(to);
        // 对于三键，存在直连原子不足的问题，找邻居的邻居
        if (poses.size() < 3) {
            auto collect_neb_2_pos = [&](const size_t &aid) {
                auto it = neighborMap.find(aid);
                if (it == neighborMap.end()) { return; }
                for (auto &neb_id:it->second) {
                    auto it2 = neighborMap.find(neb_id);
                    if (it2 == neighborMap.end()) { continue; }
                    for (auto &neb_2_id:it2->second) {
                        auto nebIt = aids.find(neb_2_id);
                        if (aids.end() != nebIt) { continue; }
                        aids.insert(neb_2_id);
                        auto atom = mol->getAtom(neb_2_id);
                        poses.push_back(getQVector3D(atom));
                    }
                }
            };
            collect_neb_2_pos(from);
            collect_neb_2_pos(to);
        }
//        qDebug() << "poses.size()=" << poses.size();
        if (poses.size() >= 3) {
//            qDebug() << "add with absent neb";
            normVecMap[bond.getId()] = QVector3D::crossProduct(poses[0] - poses[1], poses[1] - poses[2]);
        }
    });
    float bondRadius = avgBondLength / 30;
    mol->loopBondVec([&](JBond &bond) {
//        return;
        auto fromAtom = bond.getFrom(), toAtom = bond.getTo();
        QVector3D from = getQVector3D(fromAtom), to = getQVector3D(toAtom);
        std::shared_ptr<BaseWrapper> wrapper;
        switch (bond.getType()) {
            case BondType::SingleBond:
            case BondType::DelocalizedBond:
            case BondType::DownBond:
            case BondType::UpBond:
            case BondType::ImplicitBond: {
                auto cWrapper = std::make_shared<CylinderWrapper>(molRoot);
                cWrapper->setDirection(from, to);
                cWrapper->setRadius(bondRadius);
                wrapper = cWrapper;
                break;
            }
            case BondType::DoubleBond: {
                auto mcWrapper = std::make_shared<MultiCylinderWrapper>(molRoot, 2);
                std::optional<QVector3D> norm = std::nullopt;
                auto it = normVecMap.find(bond.getId());
                if (it != normVecMap.end()) {
                    norm = it->second;
                }
                mcWrapper->setDirection(from, to, norm);
                mcWrapper->setDistance(bondRadius * 2);
                mcWrapper->setRadius(bondRadius);
                wrapper = mcWrapper;
                break;
            }
            case BondType::TripleBond: {
                auto mcWrapper = std::make_shared<MultiCylinderWrapper>(molRoot, 3);
                std::optional<QVector3D> norm = std::nullopt;
                auto it = normVecMap.find(bond.getId());
                if (it != normVecMap.end()) {
                    norm = it->second;
                }
                mcWrapper->setDirection(from, to, norm);
                mcWrapper->setDistance(bondRadius * 3);
                mcWrapper->setRadius(bondRadius);
                wrapper = mcWrapper;
                break;
            }
            default: {
                qDebug() << "unknown bond type in Mol3DBuilder::build";
                break;
            }
        }
        wrapper->setRindsAndSlices(100, 100);
        wrapper->setScale(1);
        wrapper->setColor(getColor(bond.getType()));
        wrapper->setId(bond.getId());
        wrapper->setObjectName(QString("bond:%0").arg(bond.getId()));
        bonds[bond.getId()] = wrapper;
    });
    emit sig_mol_build_done();
}

void Mol3DBuilder::buildAxis(const float &_x, const float &_y, const float &_z, const float &_l) {
    const float capRadius = std::min(1.f, _l / 10), cylinderRadius = 0.3;
    float cylinderLength = std::max(10.f, _l * 0.28f);
    auto make_axis = [&](const QVector3D &_dir, const QColor &_color, const QString &_name) {
        auto norm = _dir.normalized();
        auto axis = std::make_shared<CylinderWrapper>(axisRoot);
        axis->setDirection(MathUtil::getZero3(), norm * cylinderLength);
        axis->setRadius(cylinderRadius);
        axis->setColor(_color);
        axis->setObjectName("axis " + _name);
        axis->setId(BaseEntity::sAxisId);

        auto axisCap = std::make_shared<ConeWrapper>(axisRoot);
        axisCap->setDirection(norm * cylinderLength, norm * _l * 0.3f);
        axisCap->setRadius(capRadius);
        axisCap->setColor(_color);
        axisCap->setObjectName("cap " + _name);
        axisCap->setId(BaseEntity::sAxisId);
    };
    make_axis(MathUtil::getOneX3(), Qt::blue, "X");
    make_axis(MathUtil::getOneY3(), Qt::red, "Y");
    make_axis(MathUtil::getOneZ3(), Qt::green, "Z");
    auto originSphere = std::make_shared<SphereWrapper>(axisRoot);
    originSphere->setTranslation({_x, _y, _z});
    originSphere->setRindsAndSlices(100, 100);
    originSphere->setColor(Qt::white);
    originSphere->setRadius(capRadius);
    originSphere->setObjectName("point O");
    originSphere->setId(BaseEntity::sAxisId);
}

Qt3DCore::QEntity *Mol3DBuilder::getMolRoot() const {
    return molRoot;
}

Mol3DBuilder::Mol3DBuilder(QObject *parent, Qt3DCore::QEntity *_root) : QObject(parent), root(_root), molRoot(nullptr) {
    axisRoot = new Qt3DCore::QEntity(root);
    molRootTrans = new Qt3DCore::QTransform();
    resetMolRoot();
}

Qt3DCore::QTransform *Mol3DBuilder::getMolRootTrans() const {
    return molRootTrans;
}

Qt3DCore::QEntity *Mol3DBuilder::getAxisRoot() const {
    return axisRoot;
}

void Mol3DBuilder::resetMolRoot() {
    if (molRoot) {
        molRoot->removeComponent(molRootTrans);
        molRoot->deleteLater();
    }
    molRoot = new Qt3DCore::QEntity(root);
    molRootTrans->setParent(molRoot);
    molRootTrans->setTranslation({0, 0, 0});
    molRoot->addComponent(molRootTrans);
}
