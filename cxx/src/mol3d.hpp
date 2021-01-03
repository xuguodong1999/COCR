#ifndef _MOL3D_HPP_
#define _MOL3D_HPP_

#include "mol_base.hpp"
#include <opencv2/core/types.hpp>

#include <unordered_map>

class Mol3D : virtual public MolBase{
    //3D 原子坐标
    std::unordered_map<size_t, cv::Point3f> atomPosMap3D;
public:
    Mol3D(std::shared_ptr<JMol> _mol);
    Mol3D(std::shared_ptr<JMol> _originMol,std::shared_ptr<JMol> _molWithHydrogens);
    std::unordered_map<size_t, cv::Point3f> calcCoord3D(bool _updateAtomPosMap2D = false);
};


#endif//_MOL3D_HPP_
