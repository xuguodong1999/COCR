#pragma once

#include "MolHolder.h"
#include "XGD/Base/Vec3.h"
#include <unordered_map>

namespace data_deprecated {
    class Mol3D : public MolHolder {
    protected:
        std::unordered_map<size_t, point3f> atomPosMap3D;
    public:
        Mol3D(std::shared_ptr<JMol> _mol);

        void normAtomPosMap3D(const float &_targetBondLength);

        const point3f &getAtomPos3DById(const size_t &_aid);

        void setAtomPos3DById(const size_t &_aid, const float &_x, const float &_y, const float &_z);

        bool calcCoord3D_addHs();
    };
}