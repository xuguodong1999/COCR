#ifndef _JBOND_HPP_
#define _JBOND_HPP_
namespace xgd {
    enum class BondType : int {
        ImplicitBond = 0,
        SingleBond = 1,
        DoubleBond = 2,
        TripleBond = 3,
        UpBond = 4,
        DownBond = 5,
        DelocalizedBond = 6
    };
}
#endif//_JBOND_HPP_
