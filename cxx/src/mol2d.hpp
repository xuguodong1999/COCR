#ifndef _MOL2D_HPP_
#define _MOL2D_HPP_

#include "mol_holder.hpp"

#include <opencv2/core/types.hpp>

#include <optional>

class Mol2D :virtual public MolHolder{
    //2D 原子坐标 <id,<isExplicit, position>>
    std::unordered_map<size_t, std::pair<bool, cv::Point2f>> atomPosMap2D;
public:
    Mol2D(std::shared_ptr<JMol> _mol);

    std::unordered_map<size_t, cv::Point2f> calcCoord2D(bool _updateAtomPosMap2D = false);

    float calcAvgBondLength() const;

    void insertAtomPos2D(const size_t &_aid, bool _isExplicit, const cv::Point2f &_pos);
};

#endif//_MOL2D_HPP_
