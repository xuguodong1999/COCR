#include "mol3d_builder.hpp"

#include "jmol.hpp"
#include "math_util.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

using xgd::MathUtil;
using std::fabs;

inline QVector3D getQVector3D(const xgd::JAtom &_atom) {
    return {_atom.xx, _atom.yy, _atom.zz};
}

inline QVector3D getQVector3D(const std::shared_ptr<xgd::JAtom> &_atom) {
    return getQVector3D(*_atom);
}


void Mol3DBuilder::prepare(std::shared_ptr<xgd::JMol> _mol, const QVector3D &_viewSize,
                           const QVector3D &_blankArea) {
    qDebug() << __FUNCTION__;
    mol = std::move(_mol);
    mol->norm3D(_viewSize.x(), _viewSize.y(), _viewSize.z(),
                _blankArea.x() / 2, _blankArea.y() / 2, _blankArea.z() / 2, true);
    emit on_mol_prepare_done();
}

void Mol3DBuilder::build() {
    qDebug() << __FUNCTION__;
    using namespace xgd;
    // 坐标系
//    QVector3D offset(-baseSize / 3, baseSize / 3, 0);
//    getSingleCylinderEntity(offset + zeroP, offset + axisX * 10, 0.2, Qt::blue);
//    getSingleCylinderEntity(offset + zeroP, offset + axisY * 10, 0.2, Qt::red);
//    getSingleCylinderEntity(offset + zeroP, offset + axisZ * 10, 0.2, Qt::green);

    // 添加3D原子球
    mol->loopAtomVec([&](xgd::JAtom &atom) {
        auto entity = std::make_shared<SphereEntity>(root);
        atoms[atom.getId()] = entity;
        qDebug() << getQVector3D(atom);
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
                entity->setColor(getColor(bond.getType()));
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
                entity->setDistance(3);
                entity->setRindsAndSlices(100, 100);
                entity->setScale(1);
                entity->setDirection(from, to, norm);
                entity->setColor(getColor(bond.getType()));
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
                entity->setDistance(3);
                entity->setRindsAndSlices(100, 100);
                entity->setScale(1);
                entity->setDirection(from, to, norm);
                entity->setColor(getColor(bond.getType()));
                break;
            }
            default: {
                exit(-1);
            }
        }
    });
    emit on_mol_build_done();
}
