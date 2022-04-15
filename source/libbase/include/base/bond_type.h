#pragma once

enum class BondType {
    NoneBond = 0,// 错误类型
    SingleBond = 1,// 单键，常规
    DoubleBond = 2,// 双键
    TripleBond = 3,// 三键

    SolidWedgeBond, // 单键，实楔形
    DashWedgeBond,  // 单键，虚楔形
    WaveBond,       // 单键，波浪线

    DelocalizedBond,// 离域键

    CircleBond, // 特指苯环kekule表达方式中的圆，尽在绘图部分用到
};