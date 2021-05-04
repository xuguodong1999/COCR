#ifndef _XGD_JMOL_P_
#define _XGD_JMOL_P_

#include "jmol.hpp"
#include <unordered_map>
#include <memory>
#include <optional>
#include <queue>

namespace xgd {
    class JAtom;

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
        Bn,
        Bz,
        CSO,
        COS,
        N2H4,
        N2H5,
        Cbz,
        SCN,
        CN,
        // TODO:
          OCN, NCS, Ace, THPO, NHZ, Ms
    };

    extern std::unordered_map<std::string, TokenType> SUPER_ATOM_MAP;
    extern size_t MAX_SUPER_ATOM_LENGTH;

    void initSuperAtomMap();

    class JMol_p {
        using atom_t = std::shared_ptr<JAtom>;

        using token_struct = std::tuple<std::vector<TokenType>,
                std::unordered_map<size_t, int>, std::unordered_map<size_t, ElementType>>;

        bool isValenceDataLatest;
        std::unordered_map<id_type, int> atomTotalBondOrderMap, atomDoubleBondNum;
        JMol &mol;
        /**
         * 在任何地方使用过 last_holder 后，应该调用 clearLastHolder 清除之
         */
        atom_t last_holder;
    public:
        /**
         * 绑定要求原位修改的起始原子
         * @param atom
         */
        void bindLastHolder(atom_t atom);

        /**
         * 消除原位修改的语义
         */
        void clearLastHolder();

        JMol_p(JMol &_mol);

        void exceedValence();

        void confirmValence();

        bool IsValenceDataLatest() const;

        bool tryExpand(const id_type &_aid);

        int getDoubleBondNum(const id_type &_aid) const;

        void addBondOrder4Atom(const id_type &_aid, const int &_order);

        void addDoubleBondNum4Atom(const id_type &_aid, const int &_num = 1);

        void updateValenceMap();

        int getAtomOrder(const id_type &_aid);

    private:
        /**
         * 构造缩写表达的超原子，将两侧裸露的原子返回
         * 如果只有一个原子，那么这个原子同时作为两侧
         * @param _abb 缩写枚举
         * @return 两侧的原子
         */
        std::pair<atom_t, atom_t> makeAbbType(const TokenType &_abb);

        std::pair<atom_t, atom_t> makeElementType(const ElementType &_ele);

        std::pair<atom_t, atom_t> makeElementType(const ElementType &_ele, atom_t parent, int num);

        std::pair<atom_t, atom_t> makeAlkane(const int &_num,const ElementType&_ele=ElementType::C);

        std::pair<atom_t, atom_t> makeAcyl(
                const ElementType &_acyl = ElementType::O, const ElementType &_root = ElementType::C);

        std::tuple<atom_t, atom_t, atom_t, atom_t, atom_t, atom_t> makeBenzene();

        std::optional<token_struct> interpret(const std::string &inputName);

        /**
         * 如果挂载前缀，那么返回前缀，不需要调用点关注；
         * 如果不挂载前缀，那么返回新建子图的两端原子，需要调用点将子图合入原图
         * @param tokenStruct
         * @param iBeg
         * @param iEnd
         * @param suffix
         * @param parent 如果 parent 不为空，那么括号内的所有子图将挂到 parent 上；否则顺序连接所有子图
         * @return
         */
        std::pair<atom_t, atom_t>
        extractNoBracketTokens(token_struct &tokenStruct, size_t iBeg, size_t iEnd, int suffix = 1,
                               atom_t parent = nullptr);
    };


}
#endif//_XGD_JMOL_P_
