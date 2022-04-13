#pragma once

#include "chem/mol_holder.hpp"

#include <optional>


class COCR_CHEM_EXPORT Mol2D : public MolHolder {
    //2D 原子坐标 <id,<isExplicit, position>>
    std::unordered_map<size_t, std::pair<bool, point2f>> atomPosMap2D;
public:
    Mol2D(std::shared_ptr<JMol> _mol);

    void calcCoord2D();

    float getAvgBondLength() const;

    const point2f &getPos2D(const size_t &_aid) const;

    bool isExplicit2D(const size_t &_aid) const;

    void setPos2D(const size_t &_aid, bool _isExplicit, const point2f &_pos);
};
