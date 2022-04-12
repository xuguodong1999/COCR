#pragma once

#include <cocr_base_export.h>
#include "base/fraction.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

using hash_type = uint64_t;
using node_type = unsigned char;

enum class DetectorClasses {
    ItemEmpty = 0,
    ItemSingleBond,
    ItemDoubleBond,
    ItemTripleBond,
    ItemSolidWedgeBond,
    ItemDashWedgeBond,
    ItemWaveBond,
    ItemCircleBond,
    ItemC,
    ItemH,
    ItemO,
    ItemN,
    ItemP,
    ItemS,
    ItemF,
    ItemCl,
    ItemBr,
    ItemI,
    ItemB,
    ItemHorizontalStr
};

// 约定：元素类型 0 表示 OCRA 所谓的超原子、即未解析的字符串
enum class ElementType : size_t {
    SA = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
    Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
    Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr,
    Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe,
    Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au,
    Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th, Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No, Lr, Rf,
    Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
};

enum class AtomStereo {
    None = 0,
    R,
    S
};

enum class ColorName : size_t {
    rgbSnow = 0, rgbGhostWhite, rgbWhiteSmoke, rgbGainsboro, rgbFloralWhite,
    rgbOldLace, rgbLinen, rgbAntiqueWhite, rgbPapayaWhip, rgbBlanchedAlmond, rgbBisque, rgbPeachPuff,
    rgbNavajoWhite, rgbMoccasin, rgbCornsilk, rgbIvory, rgbLemonChiffon, rgbSeashell, rgbHoneydew,
    rgbMintCream, rgbAzure, rgbAliceBlue, rgblavender, rgbLavenderBlush, rgbMistyRose, rgbWhite,
    rgbBlack, rgbDarkSlateGray, rgbDimGrey, rgbSlateGrey, rgbLightSlateGray, rgbGrey, rgbLightGray,
    rgbMidnightBlue, rgbNavyBlue, rgbCornflowerBlue, rgbDarkSlateBlue, rgbSlateBlue,
    rgbMediumSlateBlue, rgbLightSlateBlue, rgbMediumBlue, rgbRoyalBlue, rgbBlue, rgbDodgerBlue,
    rgbDeepSkyBlue, rgbSkyBlue, rgbLightSkyBlue, rgbSteelBlue, rgbLightSteelBlue, rgbLightBlue,
    rgbPowderBlue, rgbPaleTurquoise, rgbDarkTurquoise, rgbMediumTurquoise, rgbTurquoise, rgbCyan,
    rgbLightCyan, rgbCadetBlue, rgbMediumAquamarine, rgbAquamarine, rgbDarkGreen, rgbDarkOliveGreen,
    rgbDarkSeaGreen, rgbSeaGreen, rgbMediumSeaGreen, rgbLightSeaGreen, rgbPaleGreen, rgbSpringGreen,
    rgbLawnGreen, rgbGreen, rgbChartreuse, rgbMedSpringGreen, rgbGreenYellow, rgbLimeGreen,
    rgbYellowGreen, rgbForestGreen, rgbOliveDrab, rgbDarkKhaki, rgbPaleGoldenrod, rgbLtGoldenrodYello,
    rgbLightYellow, rgbYellow, rgbGold, rgbLightGoldenrod, rgbgoldenrod, rgbDarkGoldenrod,
    rgbRosyBrown, rgbIndianRed, rgbSaddleBrown, rgbSienna, rgbPeru, rgbBurlywood, rgbBeige, rgbWheat,
    rgbSandyBrown, rgbTan, rgbChocolate, rgbFirebrick, rgbBrown, rgbDarkSalmon, rgbSalmon,
    rgbLightSalmon, rgbOrange, rgbDarkOrange, rgbCoral, rgbLightCoral, rgbTomato, rgbOrangeRed, rgbRed,
    rgbHotPink, rgbDeepPink, rgbPink, rgbLightPink, rgbPaleVioletRed, rgbMaroon, rgbMediumVioletRed,
    rgbVioletRed, rgbMagenta, rgbViolet, rgbPlum, rgbOrchid, rgbMediumOrchid, rgbDarkOrchid,
    rgbDarkViolet, rgbBlueViolet, rgbPurple, rgbMediumPurple, rgbThistle, rgbSnow1, rgbSnow2, rgbSnow3,
    rgbSnow4, rgbSeashell1, rgbSeashell2, rgbSeashell3, rgbSeashell4, rgbAntiqueWhite1,
    rgbAntiqueWhite2, rgbAntiqueWhite3, rgbAntiqueWhite4, rgbBisque1, rgbBisque2, rgbBisque3,
    rgbBisque4, rgbPeachPuff1, rgbPeachPuff2, rgbPeachPuff3, rgbPeachPuff4, rgbNavajoWhite1,
    rgbNavajoWhite2, rgbNavajoWhite3, rgbNavajoWhite4, rgbLemonChiffon1, rgbLemonChiffon2,
    rgbLemonChiffon3, rgbLemonChiffon4, rgbCornsilk1, rgbCornsilk2, rgbCornsilk3, rgbCornsilk4,
    rgbIvory1, rgbIvory2, rgbIvory3, rgbIvory4, rgbHoneydew1, rgbHoneydew2, rgbHoneydew3, rgbHoneydew4,
    rgbLavenderBlush1, rgbLavenderBlush2, rgbLavenderBlush3, rgbLavenderBlush4, rgbMistyRose1,
    rgbMistyRose2, rgbMistyRose3, rgbMistyRose4, rgbAzure1, rgbAzure2, rgbAzure3, rgbAzure4,
    rgbSlateBlue1, rgbSlateBlue2, rgbSlateBlue3, rgbSlateBlue4, rgbRoyalBlue1, rgbRoyalBlue2,
    rgbRoyalBlue3, rgbRoyalBlue4, rgbBlue1, rgbBlue2, rgbBlue3, rgbBlue4, rgbDodgerBlue1,
    rgbDodgerBlue2, rgbDodgerBlue3, rgbDodgerBlue4, rgbSteelBlue1, rgbSteelBlue2, rgbSteelBlue3,
    rgbSteelBlue4, rgbDeepSkyBlue1, rgbDeepSkyBlue2, rgbDeepSkyBlue3, rgbDeepSkyBlue4, rgbSkyBlue1,
    rgbSkyBlue2, rgbSkyBlue3, rgbSkyBlue4, rgbLightSkyBlue1, rgbLightSkyBlue2, rgbLightSkyBlue3,
    rgbLightSkyBlue4, rgbSlateGray1, rgbSlateGray2, rgbSlateGray3, rgbSlateGray4, rgbLightSteelBlue1,
    rgbLightSteelBlue2, rgbLightSteelBlue3, rgbLightSteelBlue4, rgbLightBlue1, rgbLightBlue2,
    rgbLightBlue3, rgbLightBlue4, rgbLightCyan1, rgbLightCyan2, rgbLightCyan3, rgbLightCyan4,
    rgbPaleTurquoise1, rgbPaleTurquoise2, rgbPaleTurquoise3, rgbPaleTurquoise4, rgbCadetBlue1,
    rgbCadetBlue2, rgbCadetBlue3, rgbCadetBlue4, rgbTurquoise1, rgbTurquoise2, rgbTurquoise3,
    rgbTurquoise4, rgbCyan1, rgbCyan2, rgbCyan3, rgbCyan4, rgbDarkSlateGray1, rgbDarkSlateGray2,
    rgbDarkSlateGray3, rgbDarkSlateGray4, rgbAquamarine1, rgbAquamarine2, rgbAquamarine3,
    rgbAquamarine4, rgbDarkSeaGreen1, rgbDarkSeaGreen2, rgbDarkSeaGreen3, rgbDarkSeaGreen4,
    rgbSeaGreen1, rgbSeaGreen2, rgbSeaGreen3, rgbSeaGreen4, rgbPaleGreen1, rgbPaleGreen2,
    rgbPaleGreen3, rgbPaleGreen4, rgbSpringGreen1, rgbSpringGreen2, rgbSpringGreen3, rgbSpringGreen4,
    rgbGreen1, rgbGreen2, rgbGreen3, rgbGreen4, rgbChartreuse1, rgbChartreuse2, rgbChartreuse3,
    rgbChartreuse4, rgbOliveDrab1, rgbOliveDrab2, rgbOliveDrab3, rgbOliveDrab4, rgbDarkOliveGreen1,
    rgbDarkOliveGreen2, rgbDarkOliveGreen3, rgbDarkOliveGreen4, rgbKhaki1, rgbKhaki2, rgbKhaki3,
    rgbKhaki4, rgbLightGoldenrod1, rgbLightGoldenrod2, rgbLightGoldenrod3, rgbLightGoldenrod4,
    rgbLightYellow1, rgbLightYellow2, rgbLightYellow3, rgbLightYellow4, rgbYellow1, rgbYellow2,
    rgbYellow3, rgbYellow4, rgbGold1, rgbGold2, rgbGold3, rgbGold4, rgbGoldenrod1, rgbGoldenrod2,
    rgbGoldenrod3, rgbGoldenrod4, rgbDarkGoldenrod1, rgbDarkGoldenrod2, rgbDarkGoldenrod3,
    rgbDarkGoldenrod4, rgbRosyBrown1, rgbRosyBrown2, rgbRosyBrown3, rgbRosyBrown4, rgbIndianRed1,
    rgbIndianRed2, rgbIndianRed3, rgbIndianRed4, rgbSienna1, rgbSienna2, rgbSienna3, rgbSienna4,
    rgbBurlywood1, rgbBurlywood2, rgbBurlywood3, rgbBurlywood4, rgbWheat1, rgbWheat2, rgbWheat3,
    rgbWheat4, rgbTan1, rgbTan2, rgbTan3, rgbTan4, rgbChocolate1, rgbChocolate2, rgbChocolate3,
    rgbChocolate4, rgbFirebrick1, rgbFirebrick2, rgbFirebrick3, rgbFirebrick4, rgbBrown1, rgbBrown2,
    rgbBrown3, rgbBrown4, rgbSalmon1, rgbSalmon2, rgbSalmon3, rgbSalmon4, rgbLightSalmon1,
    rgbLightSalmon2, rgbLightSalmon3, rgbLightSalmon4, rgbOrange1, rgbOrange2, rgbOrange3, rgbOrange4,
    rgbDarkOrange1, rgbDarkOrange2, rgbDarkOrange3, rgbDarkOrange4, rgbCoral1, rgbCoral2, rgbCoral3,
    rgbCoral4, rgbTomato1, rgbTomato2, rgbTomato3, rgbTomato4, rgbOrangeRed1, rgbOrangeRed2,
    rgbOrangeRed3, rgbOrangeRed4, rgbRed1, rgbRed2, rgbRed3, rgbRed4, rgbDeepPink1, rgbDeepPink2,
    rgbDeepPink3, rgbDeepPink4, rgbHotPink1, rgbHotPink2, rgbHotPink3, rgbHotPink4, rgbPink1, rgbPink2,
    rgbPink3, rgbPink4, rgbLightPink1, rgbLightPink2, rgbLightPink3, rgbLightPink4, rgbPaleVioletRed1,
    rgbPaleVioletRed2, rgbPaleVioletRed3, rgbPaleVioletRed4, rgbMaroon1, rgbMaroon2, rgbMaroon3,
    rgbMaroon4, rgbVioletRed1, rgbVioletRed2, rgbVioletRed3, rgbVioletRed4, rgbMagenta1, rgbMagenta2,
    rgbMagenta3, rgbMagenta4, rgbOrchid1, rgbOrchid2, rgbOrchid3, rgbOrchid4, rgbPlum1, rgbPlum2,
    rgbPlum3, rgbPlum4, rgbMediumOrchid1, rgbMediumOrchid2, rgbMediumOrchid3, rgbMediumOrchid4,
    rgbDarkOrchid1, rgbDarkOrchid2, rgbDarkOrchid3, rgbDarkOrchid4, rgbPurple1, rgbPurple2, rgbPurple3,
    rgbPurple4, rgbMediumPurple1, rgbMediumPurple2, rgbMediumPurple3, rgbMediumPurple4, rgbThistle1,
    rgbThistle2, rgbThistle3, rgbThistle4, rgbgrey11, rgbgrey21, rgbgrey31, rgbgrey41, rgbgrey51,
    rgbgrey61, rgbgrey71, rgbgray81, rgbgray91, rgbDarkGrey, rgbDarkBlue, rgbDarkCyan, rgbDarkMagenta,
    rgbDarkRed, rgbLightGreen = 454
};

COCR_BASE_EXPORT extern std::vector<std::string> ElementsData;
COCR_BASE_EXPORT extern std::unordered_map<ElementType, frac> ElementValenceData;
COCR_BASE_EXPORT extern std::string POS_CHARGE_TEXT, NEG_CHARGE_TEXT;
COCR_BASE_EXPORT extern const unsigned char predefinedColors[][3];

COCR_BASE_EXPORT DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType);

COCR_BASE_EXPORT const std::string &convertElementTypeToString(const ElementType &_elementType);

// 返回建议比例，以碳为基准
COCR_BASE_EXPORT float atomRadius(const ElementType &_element);