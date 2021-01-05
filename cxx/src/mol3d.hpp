#ifndef _MOL3D_HPP_
#define _MOL3D_HPP_

#include "mol_holder.hpp"
#include <opencv2/core/types.hpp>

#include <unordered_map>

class Mol3D : public MolHolder{
protected:
    //3D 原子坐标
    std::unordered_map<size_t, cv::Point3f> atomPosMap3D;
public:
    Mol3D(std::shared_ptr<JMol> _mol);
    void calcCoord3D();
};


#endif//_MOL3D_HPP_
