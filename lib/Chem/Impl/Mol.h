#pragma once

#include "XGD/Chem/Atom.h"
#include "XGD/Chem/Bond.h"
#include <unordered_set>
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace ckit_deprecated {
    class JMol_p;

    class JMol {
    protected:
        id_type id;
        id_type idBase;
        std::shared_ptr<JMol_p> _p;
        std::unordered_map<id_type, std::shared_ptr<Atom>> atomMap;
        std::unordered_map<id_type, std::shared_ptr<Bond>> bondMap;
        bool is3DInfoLatest, is2DInfoLatest;

        inline static std::unordered_set<std::string> sAvailableOutputFormat;
        inline static std::unordered_set<std::string> sAvailableInputFormat;

        std::shared_ptr<Atom> addAtom(const int &_atomicNumber);

        int getNumHydrogen(const id_type &_aid);

    public:
        static bool IsValidWritableFormat(const std::string &_suffix);

        void set2DInfoLatest(bool _is2DInfoLatest = true);

        void norm2D(const float &_w, const float &_h, const float &_x = 0, const float &_y = 0, bool keepRatio = true);

        void norm3D(const float &_xx, const float &_yy, const float &_zz,
                    const float &_x = 0, const float &_y = 0, const float &_z = 0, bool keepRatio = true);

        size_t getBondNum() const;

        size_t getAtomNum() const;

        float getAvgBondLength();

        float getAvgBondLength2D();

        JMol();

        virtual std::vector<std::shared_ptr<JMol>> split() = 0;

        void loopAtomVec(std::function<void(Atom &_atom)> _func);

        void loopCurrentAtomPtrVec(std::function<void(std::shared_ptr<Atom>)> _func);

        void loopBondVec(std::function<void(Bond &_bond)> _func);

        virtual void display();

        virtual std::shared_ptr<JMol> deepClone() const = 0;

        void addAllHydrogens();

        void setId(const id_type &_id);

        id_type getId();

        virtual std::shared_ptr<Atom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        // horizontal strings, tl, br, access point
        virtual std::shared_ptr<Atom> addSuperAtom(
                const std::string &_name, const float &_x0 = 0, const float &_y0 = 0,
                const float &_x1 = 0, const float &_y1 = 0);

        virtual std::shared_ptr<Atom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z);

        virtual std::shared_ptr<Bond> addBond(
                std::shared_ptr<Atom> _a1, std::shared_ptr<Atom> _a2,
                const BondType &_type = BondType::SingleBond, const float &_offset1 = 0.5, const float &_offset2 = 0.5);

        std::shared_ptr<Atom> getAtom(const id_type &_aid);

        std::shared_ptr<Bond> getBond(const id_type &_bid);

        /**
         * 添加使用Mol对象前的回调，设计用作同步适配器和适配者的数据
         * 场景举例：OpenBabel::OBMol::DeleteBond 会同步删除这个建邻接原子持有的键关联，
         * 如果先调用了 OpenBabel::OBMol::DeleteAtom，会出空指针错误或段错误
         * 解决方法是，一旦调用适配器关联删除的接口，让旧的适配者失效，使用对象前用这个回调重建适配者
         */
        virtual void onExtraDataNeeded();

        virtual void exceedAllData();

        virtual void rebuildAllData() = 0;

        virtual std::shared_ptr<Atom> removeAtom(const id_type &_aid);

        virtual std::shared_ptr<Bond> removeBond(const id_type &_bid);

        virtual std::string writeAsPDB() = 0;

        virtual std::string writeAsSMI() = 0;

        virtual std::string writeAs(const std::string &_formatSuffix) = 0;

        virtual void readAsPDB(const std::string &_buffer) = 0;

        virtual void readAsSMI(const std::string &_buffer) = 0;

        virtual void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) = 0;

        virtual bool generate2D() = 0;

        virtual bool generate3D() = 0;

        virtual bool tryExpand();

        virtual std::vector<std::vector<id_type>> getLSSR() = 0;

        virtual std::vector<std::vector<id_type>> getSSSR() = 0;

        void tryMarkDoubleBond(const id_type &_bid);
    };
}