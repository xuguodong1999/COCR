#ifndef _XGD_JMOL_HPP_
#define _XGD_JMOL_HPP_

#include "jatom.hpp"
#include "jbond.hpp"
#include "jresidue.hpp"
#include <unordered_set>
#include <vector>
#include <memory>
#include <functional>

namespace xgd {
    class JMol {
    protected:
        size_t id;
        std::vector<std::shared_ptr<JAtom>> atomVec;
        std::vector<std::shared_ptr<JBond>> bondVec;
        std::vector<std::shared_ptr<JResidue>> residueVec;
        bool is3DInfoLatest, is2DInfoLatest;
        inline static std::unordered_set<std::string> sAvailableOutputFormat, sAvailableInputFormat;

        std::shared_ptr<JAtom> addAtom(const int &_atomicNumber);

    public:
        void norm2D(const float &_w, const float &_h, const float &_x = 0, const float &_y = 0, bool keepRatio = true);

        void norm3D(const float &_xx, const float &_yy, const float &_zz,
                    const float &_x = 0, const float &_y = 0,const float &_z = 0, bool keepRatio = true);

        JMol();

        void loopAtomVec(std::function<void(JAtom &_atom)> _func);

        void loopBondVec(std::function<void(JBond &_bond)> _func);

        virtual void display();

        void setId(const size_t &_id);

        size_t getId();

        virtual std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        virtual std::shared_ptr<JAtom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z);

        virtual std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2,
                                               const BondType &_type = BondType::SingleBond);

        std::shared_ptr<JBond> addBond(const size_t &_aid1, const size_t &_aid2,
                                       const BondType &_type = BondType::SingleBond);

        virtual std::shared_ptr<JResidue>
        addResidue(const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);

        virtual std::shared_ptr<JResidue>
        addResidue(const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z);

        std::shared_ptr<JAtom> getAtom(const size_t &_aid);

        std::shared_ptr<JBond> getBond(const size_t &_bid);

        std::shared_ptr<JResidue> getResidue(const size_t &_rid);

        /**
         * 添加使用Mol对象前的回调，设计用作同步适配器和适配者的数据
         * 场景举例：OpenBabel::OBMol::DeleteBond 会同步删除这个建邻接原子持有的键关联，
         * 如果先调用了 OpenBabel::OBMol::DeleteAtom，会出空指针错误或段错误
         * 解决方法是，一旦调用适配器关联删除的接口，让旧的适配者失效，使用对象前用这个回调重建适配者
         */
        virtual void onExtraDataNeeded();

        virtual std::shared_ptr<JAtom> removeAtom(const size_t &_aid);

        virtual std::shared_ptr<JBond> removeBond(const size_t &_bid);

        virtual std::shared_ptr<JResidue> removeResidue(const size_t &_rid);

        virtual std::string writeAsPDB() = 0;

        virtual std::string writeAsSMI() = 0;

        virtual std::string writeAs(const std::string &_formatSuffix) = 0;

        virtual void readAsPDB(const std::string &_pdbBuffer) = 0;

        virtual void readAsSMI(const std::string &_pdbBuffer) = 0;

        virtual void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) = 0;

        virtual bool generate2D() = 0;

        virtual bool generate3D() = 0;
    };
}
#endif//_XGD_JMOL_HPP_
