#include "mol3d.h"
#include "base/std_util.h"
#include "base/bond_type.h"
#include "base/cocr_types.h"
#include "mol_op.h"
#include "openbabel_util.h"

using namespace data_deprecated;
extern std::shared_ptr<MolUtil> molUtil;

bool Mol3D::calcCoord3D_addHs() {
    // 要求更新3D坐标的时候必须加氢
    auto op = std::make_shared<MolOp>(mol);
    auto add_all_hydrogen = [&](const size_t &_aid) {
        op->addHs(_aid);
    };
    mol->safeTraverseAtoms(add_all_hydrogen);
    return molUtil->getCoord3D(*this);
}


Mol3D::Mol3D(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {
}

void Mol3D::normAtomPosMap3D(const float &_targetBondLength) {
    switch (atomPosMap3D.size()) {
        case 0:
            return;
        case 1: {
            atomPosMap3D.begin()->second = {0, 0, 0};
            return;
        }
    }
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::max();
    maxx = maxy = maxz = std::numeric_limits<float>::lowest();
    for (auto&[_, p]: atomPosMap3D) {
        const auto&[x, y, z]=p;
        minx = (std::min)(minx, x);
        miny = (std::min)(miny, y);
        minz = (std::min)(minz, z);
        maxx = (std::max)(maxx, x);
        maxy = (std::max)(maxy, y);
        maxz = (std::max)(maxz, z);
    }
    point3f centOffset((minx + maxx) / 2, (miny + maxy) / 2, (minz + maxz) / 2);
    float k = _targetBondLength /
              (0.001 + std::abs((std::max)((std::max)(maxx - minx, maxy - miny), maxz - minz)));
    for (auto&[_, p]: atomPosMap3D) {
        p -= centOffset;
        p *= k;
    }
}

const point3f &Mol3D::getAtomPos3DById(const size_t &_aid) {
    return atomPosMap3D[_aid];
}

void Mol3D::setAtomPos3DById(const size_t &_aid, const float &_x, const float &_y, const float &_z) {
    atomPosMap3D[_aid] = {_x, _y, _z};
}
