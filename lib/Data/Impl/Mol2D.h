#pragma once

#include "MolHolder.h"
#include "XGD/Base/Vec2.h"

#include <optional>

namespace data_deprecated {
    class Mol2D : public MolHolder {
        // <id,<isExplicit, position>>
        std::unordered_map<size_t, std::pair<bool, point2f>> atomPosMap2D;
    public:
        Mol2D(std::shared_ptr<JMol> _mol);

        void calcCoord2D();

        float getAvgBondLength() const;

        const point2f &getPos2D(const size_t &_aid) const;

        bool isExplicit2D(const size_t &_aid) const;

        void setPos2D(const size_t &_aid, bool _isExplicit, const point2f &_pos);
    };
}