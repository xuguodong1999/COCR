#ifndef _JMOL_ADAPTER_HPP_
#define _JMOL_ADAPTER_HPP_

#include "jmol.hpp"
#include <unordered_map>
#include <functional>

namespace OpenBabel {
    class OBMol;

    class OBAtom;

    class OBBond;
}
namespace xgd {
    /**
     * 适配目标：
     * 1、使用 OpenBabel 实现三维标准化和其它需求
     * 2、使用 coordgenlibs 实现二维标准化
     */
    class JMolAdapter : public JMol {
        OpenBabel::OBMol *obMol;
        std::unordered_map<decltype(id), unsigned long> atomIdMap;
        std::unordered_map<decltype(id), unsigned long> bondIdMap;
        std::unordered_map<decltype(id), unsigned long> residueIdMap;
        std::unordered_map<unsigned long, decltype(id)> atomIdMap2;
        std::unordered_map<unsigned long, decltype(id)> bondIdMap2;
        std::unordered_map<unsigned long, decltype(id)> residueIdMap2;
        bool isOBMolLatest;

        bool runForcefield();

        void onMolUpdated();

        void syncAtoms(std::function<void(JAtom &, OpenBabel::OBAtom *)> _func);

        void sync3D();

        // 使用OpenBabel的加氢接口修改OBMol，向JMol同步数据
        void syncNewEntityFromOBMol();

        void checkOBMol();

        void resetOBMol();

        void addOBAtom(JAtom &_atom);

        void addOBBond(JBond &_bond);

        void addOBResidue(JResidue &_residue);

    public:
        JMolAdapter();

        ~JMolAdapter();

        JMolAdapter(const JMolAdapter &_jMolAdapter);

        JMolAdapter(JMolAdapter &&_jMolAdapter);

        void onExtraDataNeeded() override;

        JMolAdapter &operator=(const JMolAdapter &) = delete;

        // FIXME: 必须先调用 removeBond 再调用 removeAtom，因为 OBAtom 持有 OBBond，删除 OBBond 的时候会调用 OBAtom
        std::shared_ptr<JAtom> removeAtom(const size_t &_aid) override;

        std::shared_ptr<JBond> removeBond(const size_t &_bid) override;

        std::shared_ptr<JResidue> removeResidue(const size_t &_rid) override;

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0) override;

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y,
                const float &_z) override;

        std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2,
                                       const BondType &_type = BondType::SingleBond) override;

        std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0) override;

        std::shared_ptr<JAtom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z) override;

        std::string writeAsPDB() override;

        std::string writeAsSMI() override;

        std::string writeAs(const std::string &_formatSuffix) override;

        void readAsPDB(const std::string &_pdbBuffer) override;

        void readAsSMI(const std::string &_pdbBuffer) override;

        void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) override;

        bool generate2D() override;

        bool generate3D() override;
    };
}


#endif//_JMOL_ADAPTER_HPP_
