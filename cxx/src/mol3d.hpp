#ifndef _MOL3D_HPP_
#define _MOL3D_HPP_

#include "mol.hpp"
#include <unordered_map>
class Mol3D{
    std::shared_ptr<JMol> mol,mol3d;
    //3D 原子坐标
    std::unordered_map<size_t, cv::Point3f> atomPosMap3D;
public:
    Mol3D(std::shared_ptr<JMol>& _mol);
    std::unordered_map<size_t, cv::Point3f> calcCoord3D(bool _updateAtomPosMap2D = false);
};


#endif//_MOL3D_HPP_
