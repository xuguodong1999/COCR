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

    enum class TokenType {
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

    extern std::unordered_map<std::string, TokenType> SUPER_ATOM_MAP;
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

        int getDoubleBondNum(const id_type &_aid) const;

        void add_bond_order_for_atom(const id_type &_aid, const int &_order);

        void add_db_num_for_atom(const id_type &_aid, const int &_num = 1);

        void updateValenceMap();

        int get_atom_order(const id_type &_aid);

    private:
        /**
         * 构造缩写表达的超原子，将两侧裸露的原子返回
         * 如果只有一个原子，那么这个原子同时作为两侧
         * @param _abb 缩写枚举
         * @return 两侧的原子
         */
        std::pair<atom_t, atom_t> makeAbbType(const TokenType &_abb, atom_t a_beg = nullptr);

        std::pair<atom_t, atom_t> makeElementType(const ElementType &_ele, atom_t a_beg = nullptr);

        std::pair<atom_t, atom_t> makeAlkane(const int &_num, atom_t a_beg = nullptr);

        std::pair<atom_t, atom_t> makeAcyl(
                const ElementType &_acyl = ElementType::O, const ElementType &_root = ElementType::C,
                atom_t a_beg = nullptr);

        std::tuple<atom_t, atom_t, atom_t, atom_t, atom_t, atom_t> makeBenzene(atom_t a_beg = nullptr);

        using token_struct = std::tuple<std::vector<TokenType>,
                std::unordered_map<size_t, int>, std::unordered_map<size_t, ElementType>>;

        std::optional<token_struct> interpret(const std::string &inputName);

        std::pair<atom_t, atom_t>
        extractNoBracketTokens(token_struct &tokenStruct, size_t iBeg, size_t iEnd, atom_t a_beg = nullptr,
                               const int &suffix = 1);
    };


}
#endif//_XGD_JMOL_P_
