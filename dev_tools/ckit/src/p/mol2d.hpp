#pragma once

#include "chem/mol_holder.hpp"

#include <optional>

template<typename Number>
using point2 = std::pair<Number, Number>;

using point2f = std::pair<float, float>;

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

template<typename Number>
Number getDistance2D(const point2<Number> &p1, const point2<Number> &p2) {
    const auto&[x1, y1]=p1;
    const auto&[x2, y2]=p2;
    const auto dx = x1 - x2;
    const auto dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}