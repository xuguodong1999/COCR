#pragma once

#include "jmol.h"
#include <unordered_map>
#include <functional>

namespace OpenBabel {
    class OBMol;

    class OBAtom;

    class OBBond;
}
namespace v1_1 {
    /**
     * 适配目标：
     * 1、使用 OpenBabel 实现三维标准化和其它需求
     * 2、使用 coordgenlibs 实现二维标准化
     */
    class JMolAdapter : public JMol {
        std::shared_ptr<OpenBabel::OBMol> obMol;
        std::unordered_map<id_type, unsigned long> atomIdMap;
        std::unordered_map<id_type, unsigned long> bondIdMap;
        std::unordered_map<unsigned long, id_type> atomIdMap2;
        std::unordered_map<unsigned long, id_type> bondIdMap2;
        bool isOBMolLatest;

        bool runForcefield();

        void onMolUpdated();

        void syncAtoms(std::function<void(Atom &, OpenBabel::OBAtom *)> _func);

        void syncBonds(std::function<void(Bond &, OpenBabel::OBBond *)> _func);

        void sync3D();

        // 使用OpenBabel的加氢接口修改OBMol，向JMol同步数据
        void syncNewEntityFromOBMol();

        void checkOBMol();

        void resetOBMol();

        void addOBAtom(Atom &_atom);

        void addOBBond(Bond &_bond);


    public:
        JMolAdapter();

        ~JMolAdapter();

        JMolAdapter(const JMolAdapter &_jMolAdapter);

        JMolAdapter(JMolAdapter &&_jMolAdapter);

        std::shared_ptr<JMol> deepClone() const override;

        void display() override;

        void exceedAllData() override;

        void rebuildAllData() override;

        void onExtraDataNeeded() override;

        JMolAdapter &operator=(const JMolAdapter &) = delete;

        // FIXME: 必须先调用 removeBond 再调用 removeAtom，因为 OBAtom 持有 OBBond，删除 OBBond 的时候会调用 OBAtom
        std::shared_ptr<Atom> removeAtom(const size_t &_aid) override;

        std::shared_ptr<Bond> removeBond(const size_t &_bid) override;


        std::shared_ptr<Bond> addBond(std::shared_ptr<Atom> _a1, std::shared_ptr<Atom> _a2,
                                       const BondType &_type = BondType::SingleBond,
                                       const float &_offset1 = 0.5, const float &_offset2 = 0.5) override;

        std::shared_ptr<Atom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0) override;

        std::shared_ptr<Atom> addSuperAtom(
                const std::string &_name, const float &_x0 = 0, const float &_y0 = 0,
                const float &_x1 = 0, const float &_y1 = 0) override;

        std::shared_ptr<Atom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z) override;

        std::string writeAsPDB() override;

        std::string writeAsSMI() override;

        std::string writeAs(const std::string &_formatSuffix) override;

        void readAsPDB(const std::string &_pdbBuffer) override;

        void readAsSMI(const std::string &_pdbBuffer) override;

        void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) override;

        bool generate2D() override;

        bool generate3D() override;

        std::vector<std::vector<id_type>> getLSSR() override;

        std::vector<std::vector<id_type>> getSSSR() override;

        std::vector<std::shared_ptr<JMol>> split() override;
    };
}