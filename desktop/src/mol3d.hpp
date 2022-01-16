#ifndef _MOL3D_HPP_
#define _MOL3D_HPP_

#include "mol_holder.hpp"
#include <opencv2/core/types.hpp>

#include <unordered_map>

class Mol3D : public MolHolder {
protected:
    //3D 原子坐标
    std::unordered_map<size_t, cv::Point3f> atomPosMap3D;
public:
    Mol3D(std::shared_ptr<JMol> _mol);

    void normAtomPosMap3D(const float &_targetBondLength);

    const cv::Point3f &getAtomPos3DById(const size_t &_aid);

    void setAtomPos3DById(const size_t &_aid, const float &_x, const float &_y, const float &_z);

    bool calcCoord3D_addHs();
};


#endif//_MOL3D_HPP_
