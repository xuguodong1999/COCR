#include "mol3d.hpp"
#include "qt_util.hpp"
#include "std_util.hpp"
#include "bond_type.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"
#include "mol_op.hpp"

void Mol3D::calcCoord3D_addHs() {
    // 要求更新3D坐标的时候必须加氢
    auto op = std::make_shared<MolOp>(mol);
    auto add_all_hydrogen = [&](const size_t &_aid) {
        op->addHs(_aid);
    };
    mol->safeTraverseAtoms(add_all_hydrogen);

    std::unordered_map<size_t, cv::Point3f> depAtomPosMap;

}


Mol3D::Mol3D(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {
}
