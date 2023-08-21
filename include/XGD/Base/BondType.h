#pragma once

enum class BondType {
    NoneBond = 0,// ErrorType
    SingleBond = 1,
    DoubleBond = 2,
    TripleBond = 3,

    SolidWedgeBond,
    DashWedgeBond,
    WaveBond,

    DelocalizedBond,

    CircleBond, // kekule benzene ring, only for render
};