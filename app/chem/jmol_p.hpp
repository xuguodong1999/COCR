#ifndef _XGD_JMOL_P_
#define _XGD_JMOL_P_

#include "jmol.hpp"
#include <unordered_map>
#include <memory>
#include <optional>
#include <queue>

namespace xgd {
    class JAtom;

    using atom_t = std::shared_ptr<JAtom>;

    enum class AbbType {
        // 空白语义
        None,
        // 数字语义
        Number,
        // 元素语义
        Element,
        // 化学键语义
        Single,
        Double,
        Triple,
        // 电荷语义
        Neg,
        Pos,
        // 括号语义
        Left, Right,
        // 需要硬编码的缩写语义
        Me,
        Et,
        iPr, nPr,
        iBu, tBu, nBu,
        Am,
        NO2,
        NO,
        SO3,
        SO2,
        SO,
        CO2,
        CO,
        Ph,
        Bz,
        CSO,
        COS,
        // TODO:
        Bn, Cbz, OCN, NCS, Ace, THPO, NHZ, Ms
    };
    extern std::unordered_map<std::string, AbbType> SUPER_ATOM_MAP;
    extern size_t MAX_SUPER_ATOM_LENGTH;

    void initSuperAtomMap();

    class JMol_p {
        bool isValenceDataLatest;
        std::unordered_map<id_type, int> atomTotalBondOrderMap, atomDoubleBondNum;
        JMol &mol;
    public:
        JMol_p(JMol &_mol);

        void exceedValence();

        void confirmValence();

        bool IsValenceDataLatest() const;

        bool tryExpand(const id_type &_aid);

        void add_bond_order_for_atom(const id_type &_aid, const int &_order);

        void add_db_num_for_atom(const id_type &_aid, const int &_num = 1);

        int getDoubleBondNum(const id_type &_aid) const;

        int get_atom_order(const id_type &_aid);

        void updateValenceMap();

        /**
         * 构造缩写表达的超原子，将两侧裸露的原子返回
         * 如果只有一个原子，那么这个原子同时作为两侧
         * @param _abb 缩写枚举
         * @return 两侧的原子
         */
        std::pair<atom_t, atom_t> makeAbbType(const AbbType &_abb);

        std::pair<atom_t, atom_t> makeElementType(const ElementType &_ele);

        std::pair<atom_t, atom_t> makeAlkane(const int &_num);

        std::pair<atom_t, atom_t> makeAcyl(
                const ElementType &_acyl = ElementType::O, const ElementType &_root = ElementType::C);

        std::tuple<atom_t, atom_t, atom_t, atom_t, atom_t, atom_t> makeBenzene();

        std::optional<std::pair<std::queue<AbbType>, std::queue<int>>>
        interpret(const std::string &inputName);
    };


}
#endif//_XGD_JMOL_P_
