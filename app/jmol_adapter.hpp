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
        std::unordered_map<decltype(id), OpenBabel::OBAtom *> atomIdMap;
        std::unordered_map<decltype(id), OpenBabel::OBBond *> bondIdMap;
        std::unordered_map<OpenBabel::OBAtom *, decltype(id)> atomIdMap2;
        std::unordered_map<OpenBabel::OBBond *, decltype(id)> bondIdMap2;


        bool runForcefield();

        void onMolUpdated();

        void syncAtoms(std::function<void(JAtom &, OpenBabel::OBAtom *)> _func);

        void sync3D();

        // 使用OpenBabel的加氢接口修改OBMol，向JMol同步数据
        void syncNewEntityFromOBMol();

    public:
        JMolAdapter();

        ~JMolAdapter();

        JMolAdapter(const JMolAdapter &_jMolAdapter);

        JMolAdapter(JMolAdapter &&_jMolAdapter);

        JMolAdapter &operator=(const JMolAdapter &) = delete;

        // FIXME: 必须先调用 removeBond 再调用 removeAtom，因为 OBAtom 持有 OBBond，删除 OBBond 的时候会调用 OBAtom
        std::shared_ptr<JAtom> removeAtom(const size_t &_aid);

        std::shared_ptr<JBond> removeBond(const size_t &_bid);

        std::shared_ptr<JResidue> removeResidue(const size_t &_rid);

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z);

        std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2,
                                       const BondType &_type = BondType::SingleBond);

        std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JAtom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z);

        std::string writeAsPDB();

        std::string writeAsSMI();

        std::string writeAs(const std::string &_formatSuffix);

        void readAsPDB(const std::string &_pdbBuffer);

        void readAsSMI(const std::string &_pdbBuffer);

        void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix);

        bool generate2D();

        bool generate3D();
    };
}


#endif//_JMOL_ADAPTER_HPP_
