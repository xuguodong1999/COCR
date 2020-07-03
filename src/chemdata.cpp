#include "chemdata.h"
using namespace std;
const vector<float> massData =
        {0, 1.007947, 4.0026022, 6.9412, 9.0121823, 10.8117, 12.01078, 14.00672, 15.99943, 18.99840325,
         20.17976, 22.989769282, 24.30506, 26.98153868, 28.08553, 30.9737622, 32.0655, 35.4532, 39.9481,
         39.09831, 40.0784, 44.9559126, 47.8671, 50.94151, 51.99616, 54.9380455, 55.8452, 58.9331955, 58.69342,
         63.5463, 65.394, 69.7231, 72.641, 74.921602, 78.963, 79.9041, 83.7982, 85.46783, 87.621, 88.905852,
         91.2242, 92.906382, 95.942, 97.9072, 101.072, 102.905502, 106.421, 107.86822, 112.4118, 114.8183,
         118.7107, 121.7601, 127.603, 126.904473, 131.2936, 132.90545192, 137.3277, 137.3277, 140.1161,
         140.907652, 144.2423, 145, 150.362, 151.9641, 157.253, 158.925352, 162.5001, 164.930322, 167.2593,
         168.934212, 173.043, 174.9671, 178.492, 180.947882, 183.841, 186.2071, 190.233, 192.2173, 195.0849,
         196.9665694, 200.592, 204.38332, 207.21, 208.980401, 208.9824, 209.9871, 222.0176, 223, 226, 227,
         232.038062, 231.035882, 238.028913, 238.8486, 242.8798, 244.8594, 246.911, 248.9266, 252.9578,
         253.9656, 259.0046, 260.0124, 261.0202, 264.0436, 269.0826, 270.0904, 273.1138, 274.1216, 272.106,
         278.1528, 283.1918, 282.184, 287.223};

const vector<float> electronegativityData =
        {0, 2.20, 3.89, 0.98, 1.57, 2.04, 2.55, 3.04, 3.44, 3.98, 3.67, 0.93, 1.31, 1.61, 1.90, 2.19, 2.58,
         3.16, 3.3, 0.82, 1.00, 1.36, 1.54, 1.63, 1.66, 1.55, 1.83, 1.88, 1.91, 1.90, 1.65, 1.81, 2.01, 2.18,
         2.55, 2.96, 3.00, 0.82, 0.95, 1.22, 1.33, 1.6, 2.16, 1.9, 2.2, 2.28, 2.20, 1.93, 1.69, 1.78, 1.96,
         2.05, 2.1, 2.66, 2.67, 0.97, 0.89, 1.1, 1.12, 1.13, 1.14, 1.13, 1.17, 1.2, 1.2, 1.1, 1.22, 1.23, 1.24,
         1.25, 1.1, 1.27, 1.3, 1.5, 2.36, 1.9, 2.2, 2.20, 2.28, 2.54, 2.00, 1.62, 2.33, 2.02, 2.0, 2.2, 2.2,
         0.7, 0.9, 1.1, 1.3, 1.5, 1.38, 1.36, 1.28, 1.13, 1.28, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.291, 1.3, 1.5,
         2.36, 1.9, 2.2, 2.20, 2.28, 2.54, 2.00};

const vector <string> nameData =
        {"None", "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl",
         "Ar",
         "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br",
         "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te",
         "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm",
         "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn",
         "Fr", "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr",
         "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn"};
const vector <string> formulaData =
        {"CCl4", "CH3Cl", "C2H5OC2H5", "KO2", "CuCl2", "CHCl3", "CH3COONa", "CH3COOCH3", "CH3COOC2H5",
         "CH3CH2Br", "CH2Cl2", "C2H5ONa", "(C2H5O)2Mg", "C2H5ONO2", "CH3CH2OSO3H", "(C2H5O)3Al", "CH3CH2OH",
         "C11H22O11", "Ba(NO3)2", "Al2S3", "NaAlO2", "NH3·H2O", "AgOH", "Ca(ClO)2", "Ca(H2PO4)2", "CH4", "C2H6",
         "C2H4", "C2H2", "Fe(NO3)2", "CaC2", "CH3CHO", "C2H5OH", "Fe(OH)2", "Ca(HCO3)2", "CH3OH", "Ag(NH3)2OH",
         "HCHO", "HClO", "K2MnO4", "NaH", "Mg(HCO3)2", "Mg3N2", "MgSiO3", "NO2", "NO", "N2O4", "Na2SO4",
         "Cu(OH)2", "H4SiO4", "H2SiO3", "NaBr", "NaHS", "NaHSO3", "NaHSO4", "Cu3P", "HPO3", "PCl5", "PCl3",
         "C6H5SO3H", "C6H12", "C6H6", "C6H5Br", "C6H5Cl", "CS2", "K2S", "Ca(HSO3)2", "CaSO3", "CaSiO3",
         "ZrOCl2", "ZrO2", "Zr(SO4)2", "Zr(NO3)4", "Zr", "ZnSO4", "ZnS", "ZnO", "ZnCl2", "ZnCO3", "ZnC2O4",
         "Zn2P2O7", "Zn(NO3)2", "Zn", "WO3", "W", "VO2", "V2O5", "V", "UO3", "UF4", "UCl4", "U3O8", "U",
         "TiOSO4", "TiO2", "TiCl4", "TiCl3", "Ti", "ThCl4", "Th(SO4)2", "Th(NO3)4", "Th(C2O4)2", "Th", "SrSO4",
         "SrO", "SrCl2", "SrCO3", "SrC2O4", "Sr3(PO4)2", "Sr(NO3)2", "Sr", "SnS2", "SnS", "SnO2", "SnCl2", "Sn",
         "SiO2", "SiF4", "SiCl4", "Si", "SbCl5", "SbCl3", "Sb2O5", "Sb2O3", "Sb", "SO3", "SO2", "S", "PbbI2",
         "PbSO4", "PbS", "PbO2", "PbO", "PbCrO4", "PbCl2", "PbCO3", "PbC2O4", "Pb3O4", "Pb3(PO4)2", "Pb2O3",
         "Pb(NO3)2", "Pb(IO3)2", "Pb", "P2O5", "P", "O3", "O2", "O", "NiSO4", "NiS", "NiO", "NiCl2", "Ni(NO3)2",
         "Ni", "NaOH", "NaNO3", "NaNO2", "NaI", "NaHCO3", "NaH2PO4", "NaH", "NaClO", "NaCl", "NaCO3", "NaCN",
         "NaBrO3", "NaBiO3", "NaAsO4", "Na3PO4", "Na3AsO4", "Na3AsO3", "Na2SO4", "Na2SiO3", "Na2SO3", "Na2S2O3",
         "Na2S", "Na2O2", "Na2O", "Na2HPO4", "Na2HAsO3", "Na2CrO4", "Na2CO3", "Na2C2O4", "Na2B4O7", "Na",
         "NH4VO3", "NH4NO3", "NH4HCO3", "NH4H2PO4", "NH4Cl", "NH3", "N2", "N", "MnSO4", "MnS", "MnO2", "MnO",
         "MnCl2", "MnCO3", "Mn3O4", "Mn2P2O7", "Mn2O3", "Mn(NO3)2", "Mn", "MgSO4", "MgO", "MgNH4PO4",
         "MgNH4AsO4", "MgH2", "MgCl2", "MgCO3", "MgC2O4", "Mg2P2O7", "Mg(OH)2", "Mg(NO3)2", "Mg", "LiOH", "LiH",
         "LiCl", "Li2O", "Li2CO3", "Li", "KOH", "KNaC4O6H4", "KNO3", "KNO2", "KMnO4", "KIO3", "KI", "KHSO4",
         "KHC8O4H8", "KHC4O6H4", "KHC2O4", "KH", "KFe(SO4)2", "KF", "KClO4", "KClO3", "KCl", "KBrO3", "KBr",
         "KAl(SO4)2", "K4Fe(CN)6", "K3PO4", "K3Fe(CN)6", "K3AsO4", "K2SO4", "K2S2O7", "K2PtCl6", "K2O",
         "K2CrO4", "K2Cr2O7", "K2CO3", "K", "I2", "I", "HgSO4", "HgS", "HgO", "HgI2", "HgCl2", "Hg2SO4",
         "Hg2I2", "Hg2Cl2", "Hg2Br2", "Hg2(NO3)2", "Hg(NO3)2", "Hg(CN)2", "Hg", "HNO3", "HNO2", "HIO3", "HI",
         "HF", "HClO4", "HCl", "HCOONa", "HCOOH", "HCN", "HBr", "H3PO4", "H3PO3", "H3BO3", "H3AsO4", "H3AsO3",
         "H2SO4", "H2SO3", "H2S", "H2O2", "H2O", "H2CO3", "H2C2O4", "H2", "H", "FeSO4", "FeS2", "FeS", "FeO",
         "FeCl3", "FeCl2", "FeCO3", "Fe3O4", "Fe2O3", "Fe2(SO4)3", "Fe(OH)3", "Fe(NO3)3", "Fe", "F2", "F",
         "CuSO4", "CuSCN", "CuS", "CuO", "CuCl2CuI", "CuCl", "Cu2S", "Cu2O", "Cu2(OH)2CO3", "Cu(NO3)2", "Cu",
         "CsOH", "Cs", "CrCl3", "Cr2O3", "Cr2(SO4)3", "Cr", "CoSO4", "CoS", "CoCl2", "Co3O4", "Co2O3",
         "Co(NO3)2", "Co", "Cl2", "Cl", "CeO2", "Ce(SO4)2", "Ce", "CdS", "CdCl2", "CdCO3", "Cd", "CaSO4", "CaO",
         "CaCl2", "CaCO3", "CaC2O4", "Ca3(PO4)2", "Ca(OH)2", "Ca", "CS(NH2)2", "CO2", "CO(NH2)2", "CO",
         "CH3COONH4", "CH3COOH", "C6H5SO3H", "C6H5ONa", "C6H5OH", "C6H5NO2", "C6H5Cl", "C6H5COOH", "C6H5Br",
         "C6H12", "C", "Br2", "Br", "BiONO3", "BiOHCO3", "BiOCl", "BiCl3", "Bi2S3", "Bi2O3", "Bi(NO3)3", "Bi",
         "BeO", "Be", "BaSO4", "BaO", "BaCrO4", "BaCl2", "BaCO3", "BaBr2", "Ba3(AsO4)2", "Ba(OH)2", "Ba",
         "B2O3", "B", "As2S3", "As2O5", "As2O3", "As", "AlCl3", "AlBr3", "Al2O3", "Al2(SO4)3", "Al(OH)3",
         "Al(NO3)3", "Al", "AgSCN", "AgNO3", "AgI", "AgCl", "AgCN", "AgBrO3", "AgBr", "Ag3AsO4", "Ag2SO4",
         "Ag2CrO4", "Ag", "(NH4)3PO4", "(NH4)2SO4", "(NH4)2S", "(NH4)2PtCl6", "(NH4)2MoO4", "(NH4)2HPO4",
         "(NH4)2Fe(SO4)2", "(NH4)2CO3", "(NH4)2C2O4", "(CH3COO)2UO2", "(CH3COO)2Pb", "(CH3CO)2O",
         "(CH2COO)2Zn"};

const vector <string> valenceData =
        {"CCl4", "4 1", "-1 1", "CH3Cl", "-2 1", "-1 1", "1 1", "C2H5OC2H5", "-2 1", "1 1", "-2 1", "KO2",
         "1 1", "-1 2", "CuCl2", "-1 1", "2 1", "CHCl3", "2 1", "-1 1", "1 1", "CH3Cl", "-2 1", "-1 1", "1 1",
         "CH3COONa", "0 1", "1 1", "1 1", "-2 1", "CH3COOCH3", "-2 3", "1 1", "-2 1", "CH3COOC2H5", "-1 1",
         "1 1", "-2 1", "CH3CH2Br", "-1 1", "-2 1", "1 1", "CH2Cl2", "0 1", "-1 1", "1 1", "C2H5ONa", "-2 1",
         "1 1", "1 1", "-2 1", "(C2H5O)2Mg", "-2 1", "1 1", "2 1", "-2 1", "C2H5ONO2", "-2 1", "1 1", "5 1",
         "-2 1", "CH3CH2OSO3H", "-2 1", "1 1", "-2 1", "6 1", "(C2H5O)3Al", "3 1", "-2 1", "1 1", "-2 1",
         "CH3CH2OH", "-2 1", "1 1", "-2 1", "C11H22O11", "0 1", "1 1", "-2 1", "Ba(NO3)2", "2 1", "5 1", "-2 1",
         "Al2S3", "3 1", "-2 1", "NaAlO2", "3 1", "1 1", "-2 1", "NH3·H2O", "1 1", "-3 1", "-2 1", "AgOH",
         "1 1", "1 1", "-2 1", "Ca(ClO)2", "2 1", "1 1", "-2 1", "Ca(H2PO4)2", "2 1", "1 1", "-2 1", "5 1",
         "CH4", "-4 1", "1 1", "C2H6", "-3 1", "1 1", "C2H4", "-2 1", "1 1", "C2H2", "-1 1", "1 1", "Fe(NO3)2",
         "2 1", "5 1", "-2 1", "CaC2", "-1 1", "2 1", "CH3CHO", "-1 1", "1 1", "-2 1", "C2H5OH", "-2 1", "1 1",
         "-2 1", "Fe(OH)2", "2 1", "1 1", "-2 1", "Ca(HCO3)2", "4 1", "2 1", "1 1", "-2 1", "CH3OH", "-2 1",
         "1 1", "-2 1", "Ag(NH3)2OH", "1 1", "1 1", "-3 1", "-2 1", "HCHO", "0 1", "1 1", "-2 1", "HClO", "1 1",
         "1 1", "-2 1", "MgH2", "-1 1", "2 1", "K2MnO4", "1 1", "6 1", "-2 1", "KH", "-1 1", "1 1", "LiH",
         "-1 1", "1 1", "NaH", "-1 1", "1 1", "Cs", "0 1", "CsOH", "1 1", "1 1", "-2 1", "KF", "-1 1", "1 1",
         "Mg(HCO3)2", "4 1", "1 1", "2 1", "-2 1", "Mg3N2", "2 1", "-3 1", "MgSiO3", "2 1", "-2 1", "4 1",
         "N2O4", "4 1", "-2 1", "NO", "2 1", "-2 1", "NO2", "4 1", "-2 1", "Na2SO4", "1 1", "-2 1", "6 1",
         "Cu(OH)2", "2 1", "1 1", "-2 1", "H4SiO4", "1 1", "-2 1", "4 1", "H2SiO3", "1 1", "-2 1", "4 1",
         "NaBr", "-1 1", "1 1", "NaHS", "1 1", "1 1", "-2 1", "NaHSO3", "1 1", "1 1", "-2 1", "4 1", "NaHSO4",
         "1 1", "1 1", "-2 1", "6 1", "Cu3P", "1 1", "-3 1", "HPO3", "1 1", "-2 1", "5 1", "PCl5", "-1 1",
         "5 1", "PCl3", "-1 1", "3 1", "C6H5SO3H", "-2 3", "1 1", "-2 1", "4 1", "C6H12", "-2 1", "1 1",
         "C6H5NO2", "-2 3", "1 1", "3 1", "-2 1", "C6H6", "-1 1", "1 1", "C6H5Br", "-2 3", "-1 1", "1 1",
         "C6H5Cl", "-2 3", "-1 1", "1 1", "C6H5OH", "-2 3", "1 1", "-2 1", "C6H5ONa", "-2 3", "1 1", "1 1",
         "-2 1", "CS2", "4 1", "-2 1", "K2S", "1 1", "-2 1", "Ca(HSO3)2", "2 1", "1 1", "-2 1", "4 1", "CaSO3",
         "2 1", "-2 1", "4 1", "CaSiO3", "2 1", "-2 1", "4 1", "Na2SiO3", "1 1", "-2 1", "4 1", "Ag", "0 1",
         "AgBr", "1 1", "-1 1", "AgBrO3", "1 1", "5 1", "-2 1", "AgCl", "1 1", "-1 1", "AgI", "1 1", "-1 1",
         "AgNO3", "1 1", "5 1", "-2 1", "Ag2CrO4", "1 1", "6 1", "-2 1", "Ag2SO4", "1 1", "-2 1", "6 1",
         "Ag3AsO4", "1 1", "5 1", "-2 1", "Al", "0 1", "AlBr3", "3 1", "-1 1", "AlCl3", "3 1", "-1 1",
         "Al(NO3)3", "3 1", "5 1", "-2 1", "Al2O3", "3 1", "-2 1", "Al(OH)3", "3 1", "1 1", "-2 1", "Al2(SO4)3",
         "3 1", "-2 1", "6 1", "As", "0 1", "As2O3", "3 1", "-2 1", "As2O5", "5 1", "-2 1", "H3AsO4", "5 1",
         "1 1", "-2 1", "B", "0 1", "B2O3", "3 1", "-2 1", "Ba", "0 1", "BaBr2", "2 1", "-1 1", "BaCO3", "2 1",
         "4 1", "-2 1", "BaCl2", "2 1", "-1 1", "BaCrO4", "2 1", "6 1", "-2 1", "BaO", "2 1", "-2 1", "Ba(OH)2",
         "2 1", "1 1", "-2 1", "BaSO4", "2 1", "-2 1", "6 1", "Ba3(AsO4)2", "5 1", "2 1", "-2 1", "Be", "0 1",
         "BeO", "2 1", "-2 1", "Bi", "0 1", "Bi2O3", "3 1", "-2 1", "Br", "0 1", "NaBrO3", "5 1", "1 1", "-2 1",
         "Br2", "0 1", "C", "0 1", "CH3COOH", "0 1", "1 1", "-2 1", "(CH3CO)2O", "0 1", "1 1", "-2 1", "NaAsO4",
         "7 1", "1 1", "-2 1", "CO", "2 1", "-2 1", "CO2", "4 1", "-2 1", "Na2C2O4", "3 1", "1 1", "-2 1", "Ca",
         "0 1", "CaCl2", "2 1", "-1 1", "CaCO3", "4 1", "2 1", "-2 1", "CaC2O4", "3 1", "2 1", "-2 1", "CaO",
         "2 1", "-2 1", "Ca(OH)2", "2 1", "1 1", "-2 1", "CaSO4", "2 1", "-2 1", "6 1", "Ca3(PO4)2", "2 1",
         "-2 1", "5 1", "Cd", "0 1", "Ce", "0 1", "CeO2", "4 1", "-2 1", "Cl2", "0 1", "Cl", "0 1", "Co", "0 1",
         "Cr", "0 1", "Na2CrO4", "6 1", "1 1", "-2 1", "Cr2O3", "3 1", "-2 1", "Cu", "0 1", "CuO", "2 1",
         "-2 1", "Cu2O", "1 1", "-2 1", "F", "0 1", "F2", "0 1", "Fe", "0 1", "FeO", "2 1", "-2 1", "Fe(OH)3",
         "3 1", "1 1", "-2 1", "Fe2O3", "3 1", "-2 1", "Fe3O4", "8 3", "-2 1", "H", "0 1", "HBr", "-1 1", "1 1",
         "HCOOH", "2 1", "1 1", "-2 1", "CH3COOH", "0 1", "1 1", "-2 1", "C6H5COOH", "-2 7", "1 1", "-2 1",
         "HCl", "-1 1", "1 1", "HClO4", "7 1", "1 1", "-2 1", "HF", "-1 1", "1 1", "HI", "1 1", "-1 1", "HIO3",
         "1 1", "5 1", "-2 1", "HNO2", "1 1", "3 1", "-2 1", "H2", "0 1", "H2CO3", "4 1", "1 1", "-2 1",
         "H2C2O4", "3 1", "1 1", "-2 1", "H2O", "1 1", "-2 1", "H2S", "1 1", "-2 1", "H2SO3", "1 1", "-2 1",
         "4 1", "H2SO4", "1 1", "-2 1", "6 1", "H3AsO3", "3 1", "1 1", "-2 1", "H3AsO4", "5 1", "1 1", "-2 1",
         "H3BO3", "3 1", "1 1", "-2 1", "H3PO3", "1 1", "-2 1", "3 1", "H3PO4", "1 1", "-2 1", "5 1", "Hg",
         "0 1", "HgO", "2 1", "-2 1", "I", "0 1", "I2", "0 1", "K", "0 1", "KAl(SO4)2", "3 1", "1 1", "-2 1",
         "6 1", "KBr", "-1 1", "1 1", "KBrO3", "5 1", "1 1", "-2 1", "KCl", "-1 1", "1 1", "KClO3", "5 1",
         "1 1", "-2 1", "KClO4", "7 1", "1 1", "-2 1", "KHC2O4", "3 1", "1 1", "1 1", "-2 1", "KHC4O6H4", "3 2",
         "1 1", "1 1", "-2 1", "KHC8O4H8", "1 -4", "1 1", "1 1", "-2 1", "KHSO4", "1 1", "1 1", "-2 1", "6 1",
         "KI", "-1 1", "1 1", "KIO3", "5 1", "1 1", "-2 1", "KMnO4", "1 1", "7 1", "-2 1", "KNO2", "1 1", "3 1",
         "-2 1", "KNO3", "1 1", "5 1", "-2 1", "KNaC4O6H4", "3 2", "1 1", "1 1", "1 1", "-2 1", "KOH", "1 1",
         "1 1", "-2 1", "K2CO3", "4 1", "1 1", "-2 1", "K2CrO4", "6 1", "1 1", "-2 1", "K2Cr2O7", "6 1", "1 1",
         "-2 1", "K2SO4", "1 1", "-2 1", "6 1", "K2S2O7", "1 1", "-2 1", "6 1", "K3AsO4", "5 1", "1 1", "-2 1",
         "K3PO4", "1 1", "-2 1", "5 1", "Li", "0 1", "LiCl", "-1 1", "1 1", "LiOH", "1 1", "1 1", "-2 1",
         "Li2CO3", "4 1", "1 1", "-2 1", "Li2O", "1 1", "-2 1", "Mg", "0 1", "MgCO3", "4 1", "2 1", "-2 1",
         "MgC2O4", "3 1", "2 1", "-2 1", "MgCl2", "-1 1", "2 1", "Mg(NO3)2", "2 1", "5 1", "-2 1", "MgO", "2 1",
         "-2 1", "Mg(OH)2", "1 1", "2 1", "-2 1", "MgSO4", "2 1", "-2 1", "6 1", "Mg2P2O7", "2 1", "-2 1",
         "5 1", "Mn", "0 1", "MnO", "2 1", "-2 1", "MnO2", "4 1", "-2 1", "Mn2O3", "3 1", "-2 1", "Mn3O4",
         "8 3", "-2 1", "N", "0 1", "N2", "0 1", "NH3", "1 1", "-3 1", "HNO3", "1 1", "5 1", "-2 1", "NaNO3",
         "5 1", "1 1", "-2 1", "Na", "0 1", "NaBiO3", "5 1", "1 1", "-2 1", "Na", "0 1", "NaBrO3", "5 1", "1 1",
         "-2 1", "HCOONa", "2 1", "1 1", "1 1", "-2 1", "NaCl", "-1 1", "1 1", "NaClO", "1 1", "1 1", "-2 1",
         "NaHCO3", "4 1", "1 1", "1 1", "-2 1", "NaH2PO4", "1 1", "1 1", "-2 1", "5 1", "NaI", "-1 1", "1 1",
         "NaNO2", "3 1", "1 1", "-2 1", "NaNO3", "5 1", "1 1", "-2 1", "NaOH", "1 1", "1 1", "-2 1", "Na2B4O7",
         "3 1", "1 1", "-2 1", "Na2CO3", "4 1", "1 1", "-2 1", "Na2C2O4", "3 1", "1 1", "-2 1", "Na2HAsO3",
         "3 1", "1 1", "1 1", "-2 1", "Na2HPO4", "1 1", "1 1", "-2 1", "5 1", "Na2O", "1 1", "-2 1", "Na2S",
         "1 1", "-2 1", "Na2SO3", "1 1", "-2 1", "4 1", "Na2SO4", "1 1", "-2 1", "6 1", "Na2S2O3", "1 1",
         "-2 1", "2 1", "Na3AsO3", "3 1", "1 1", "-2 1", "Na3AsO4", "5 1", "1 1", "-2 1", "Na3PO4", "1 1",
         "-2 1", "5 1", "Ni", "0 1", "NiO", "2 1", "-2 1", "O", "0 1", "H2O", "1 1", "-2 1", "O2", "0 1", "O3",
         "0 1", "P", "0 1", "H3PO4", "1 1", "-2 1", "5 1", "P2O5", "-2 1", "5 1", "Pb", "0 1", "PbO", "-2 1",
         "2 1", "PbO2", "-2 1", "4 1", "Pb2O3", "-2 1", "3 1", "Pb3O4", "-2 1", "8 3", "S", "0 1", "SO2",
         "-2 1", "4 1", "SO3", "-2 1", "6 1", "Na2SO4", "1 1", "-2 1", "6 1", "Sb", "0 1", "Sb2O3", "-2 1",
         "3 1", "Sb2O5", "-2 1", "5 1", "Si", "0 1", "SiF4", "-1 1", "4 1", "SiO2", "-2 1", "4 1", "Sn", "0 1",
         "SnO2", "-2 1", "4 1", "Sr", "0 1", "SrO", "-2 1", "2 1", "Th", "0 1", "Ti", "0 1", "TiO2", "-2 1",
         "4 1", "U", "0 1", "UF4", "-1 1", "4 1", "UO3", "-2 1", "6 1", "U3O8", "-2 1", "16 3", "V", "0 1",
         "VO2", "-2 1", "4 1", "V2O5", "-2 1", "5 1", "W", "0 1", "WO3", "-2 1", "6 1", "Zn", "0 1", "ZnCO3",
         "4 1", "-2 1", "2 1", "ZnC2O4", "3 1", "-2 1", "2 1", "(CH2COO)2Zn", "1 2", "1 1", "-2 1", "2 1",
         "ZnCl2", "-1 1", "2 1", "Zn(NO3)2", "5 1", "-2 1", "2 1", "ZnO", "-2 1", "2 1", "ZnS", "-2 1", "2 1",
         "ZnSO4", "-2 1", "6 1", "2 1", "Zn2P2O7", "-2 1", "5 1", "2 1", "Zr", "0 1", "ZrO2", "-2 1", "4 1",
         "KAl(SO4)2", "3 1", "1 1", "-2 1", "6 1", "BiCl3", "3 1", "-1 1", "Bi(NO3)3", "3 1", "5 1", "-2 1",
         "BiOCl", "3 1", "-1 1", "-2 1", "BiONO3", "3 1", "5 1", "-2 1", "NaCO3", "4 1", "1 1", "-2 1",
         "CS(NH2)2", "4 1", "1 1", "-3 1", "-2 1", "CdS", "2 1", "-2 1", "CoCl2", "-1 1", "2 1", "CdCl2", "2 1",
         "-1 1", "Co(NO3)2", "2 1", "5 1", "-2 1", "CoSO4", "2 1", "-2 1", "6 1", "Co2O3", "3 1", "-2 1",
         "Co3O4", "8 3", "-2 1", "CrCl3", "-1 1", "3 1", "CuCl", "-1 1", "1 1", "CuCl2", "-1 1", "2 1", "CuI",
         "1 1", "-1 1", "Cu(NO3)2", "2 1", "5 1", "-2 1", "CuSO4", "2 1", "-2 1", "6 1", "Cu2S", "1 1", "-2 1",
         "FeCO3", "4 1", "2 1", "-2 1", "FeCl2", "-1 1", "2 1", "FeCl3", "-1 1", "3 1", "Fe(NO3)3", "3 1",
         "5 1", "-2 1", "FeSO4", "2 1", "-2 1", "6 1", "Fe2(SO4)3", "3 1", "-2 1", "6 1", "HgCl2", "-1 1",
         "2 1", "HgI2", "2 1", "-1 1", "Hg(NO3)2", "2 1", "5 1", "-2 1", "HgSO4", "2 1", "-2 1", "6 1",
         "Hg2Br2", "-1 1", "1 1", "Hg2Cl2", "-1 1", "1 1", "Hg2I2", "1 1", "-1 1", "Hg2(NO3)2", "1 1", "5 1",
         "-2 1", "Hg2SO4", "1 1", "-2 1", "6 1", "KFe(SO4)2", "3 1", "1 1", "-2 1", "6 1", "K3Fe(CN)6", "2 1",
         "3 1", "1 1", "-3 1", "K4Fe(CN)6", "2 1", "2 1", "1 1", "-3 1", "MnCO3", "4 1", "2 1", "-2 1", "MnCl2",
         "-1 1", "2 1", "Mn(NO3)2", "2 1", "5 1", "-2 1", "MnSO4", "2 1", "-2 1", "6 1", "NiCl2", "-1 1", "2 1",
         "Ni(NO3)2", "5 1", "2 1", "-2 1", "NiSO4", "2 1", "-2 1", "6 1", "PbCO3", "4 1", "-2 1", "2 1",
         "PbC2O4", "3 1", "-2 1", "2 1", "PbCl2", "-1 1", "2 1", "PbI2", "-1 1", "2 1", "Pb(NO3)2", "5 1",
         "-2 1", "2 1", "PbSO4", "-2 1", "2 1", "6 1", "Pb3(PO4)2", "-2 1", "5 1", "2 1", "SbCl3", "-1 1",
         "3 1", "SbCl5", "-1 1", "5 1", "SiCl4", "-1 1", "4 1", "SnCl2", "-1 1", "2 1", "SrCO3", "4 1", "-2 1",
         "2 1", "SrCl2", "-1 1", "2 1", "Sr(NO3)2", "5 1", "-2 1", "2 1", "SrSO4", "-2 1", "6 1", "2 1",
         "Sr3(PO4)2", "-2 1", "5 1", "2 1", "ThCl4", "-1 1", "4 1", "Th(NO3)4", "5 1", "-2 1", "4 1",
         "Th(SO4)2", "-2 1", "6 1", "4 1", "TiCl3", "-1 1", "3 1", "TiCl4", "-1 1", "4 1", "TiOSO4", "-2 1",
         "6 1", "4 1", "UCl4", "-1 1", "4 1", "Zr(NO3)4", "5 1", "-2 1", "4 1", "ZrOCl2", "-1 1", "-2 1", "4 1",
         "Zr(SO4)2", "-2 1", "6 1", "4 1", "BiOHCO3", "3 1", "4 1", "1 1", "-2 1", "CdCO3", "4 1", "2 1",
         "-2 1", "Ce(SO4)2", "4 1", "-2 1", "6 1", "Cr2(SO4)3", "3 1", "-2 1", "6 1", "Cu2(OH)2CO3", "4 1",
         "2 1", "1 1", "-2 1", "Mn2P2O7", "2 1", "-2 1", "5 1", "PbCrO4", "6 1", "-2 1", "2 1", "AgCN", "1 1",
         "2 1", "-3 1", "AgSCN", "1 1", "4 1", "-3 1", "-2 1", "Bi2S3", "3 1", "-2 1", "HCN", "2 1", "1 1",
         "-3 1", "NaCN", "2 1", "-3 1", "1 1", "CO(NH2)2", "4 1", "1 1", "-3 1", "-2 1", "CoS", "2 1", "-2 1",
         "CuS", "2 1", "-2 1", "(NH4)2Fe(SO4)2", "2 1", "1 1", "-3 1", "-2 1", "6 1", "FeS", "2 1", "-2 1",
         "FeS2", "4 1", "-2 1", "HCN", "2 1", "1 1", "-3 1", "HgS", "2 1", "-2 1", "K2PtCl6", "-1 1", "1 1",
         "4 1", "MgNH4AsO4", "5 1", "1 1", "2 1", "-3 1", "-2 1", "MgNH4PO4", "1 1", "2 1", "-3 1", "-2 1",
         "5 1", "MnS", "2 1", "-2 1", "CH3COONH4", "0 1", "1 1", "-3 1", "-2 1", "NH4Cl", "-1 1", "1 1", "-3 1",
         "NH4HCO3", "4 1", "1 1", "-3 1", "-2 1", "NH4H2PO4", "1 1", "-3 1", "-2 1", "5 1", "NH4NO3", "1 1",
         "1 1", "-2 1", "NH4VO3", "1 1", "-3 1", "-2 1", "5 1", "(NH4)2CO3", "4 1", "1 1", "-3 1", "-2 1",
         "(NH4)2C2O4", "3 1", "1 1", "-3 1", "-2 1", "(NH4)2HPO4", "1 1", "-3 1", "-2 1", "5 1", "(NH4)2MoO4",
         "1 1", "6 1", "-3 1", "-2 1", "(NH4)2PtCl6", "-1 1", "1 1", "-3 1", "4 1", "(NH4)2S", "1 1", "-3 1",
         "-2 1", "(NH4)2SO4", "1 1", "-3 1", "-2 1", "-2 1", "(NH4)3PO4", "1 1", "-3 1", "-2 1", "5 1", "NiS",
         "2 1", "-2 1", "PbS", "2 1", "-2 1", "SnS", "-2 1", "2 1", "SnS2", "-2 1", "4 1", "SrC2O4", "3 1",
         "-2 1", "2 1", "(CH3COO)2Pb", "0 1", "1 1", "-2 1", "2 1", "As2S3", "3 1", "-2 1", "Pb(IO3)2", "5 1",
         "2 1", "-2 1", "CuSCN", "4 1", "1 1", "-3 1", "-2 1", "Hg(CN)2", "2 1", "2 1", "-3 1", "Hg2(CN)2",
         "2 1", "1 1", "-3 1", "Th(C2O4)2", "3 1", "-2 1", "4 1", "(CH3COO)2UO2", "0 1", "1 1", "-2 1", "6 1",
         "H2O2", "1 1", "-1 1", "K2O", "1 1", "-2 1", "KO2", "1 1", "-1 2", "BaO2", "2 1", "-1 1", "K2O2",
         "1 1", "-1 1", "Na2O2", "1 1", "-1 1"};
const vector <string> state =
        {
                "ZnSO4", "r", "ZnCl2", "r", "Zn(NO3)2", "r", "Zn", "m", "X", "x", "WO3", "s", "W", "m", "V2O5",
                "s", "V", "m", "SiO2", "s", "SiF4", "g", "Si", "s", "SO3", "l", "SO2", "g", "S", "s", "PhSO3H",
                "o", "PhONa", "r", "PhOH", "s", "PhNO2", "o", "PhCl", "o", "PhBr", "o", "Ph", "o", "PbS", "s",
                "PbAc2", "r", "PCl5", "s", "PCl3", "s", "P2O5", "s", "P", "s", "O2", "g", "NaSO4", "r", "NaOH",
                "r", "NaNO3", "r", "NaNO2", "r", "NaHSO4", "r", "NaHSO3", "r", "NaHS", "r", "NaHCO3", "r",
                "NaClO", "r", "NaCl", "r", "NaBr", "r", "NaAlO2", "r", "Na2SiO3", "r", "Na2SO4", "r", "Na2SO3",
                "r", "Na2S", "r", "Na2O2", "s", "Na2O", "s", "Na2CO3", "r", "Na", "m", "NO2", "g", "NO", "g",
                "NH4NO3", "r", "NH4HCO3", "r", "NH4Cl", "r", "NH3·H2O", "l", "NH3", "g", "N2O4", "g", "N2", "g",
                "MnSO4", "r", "MnO2", "s", "MnO", "s", "MnCl2", "r", "Mn", "m", "MgSiO3", "s", "MgSO4", "r",
                "MgO", "s", "MgCl2", "r", "MgCO3", "s", "Mg3N2", "s", "Mg(OH)2", "s", "Mg(NO3)2", "r",
                "Mg(HCO3)2", "r", "Mg", "m", "MX", "r", "MOH", "r", "MH", "s", "M", "m", "Li2O", "s", "Li", "m",
                "KOH", "r", "KO2", "s", "KNO3", "r", "KMnO4", "r", "KI", "r", "KClO3", "r", "KCl", "r", "KBr",
                "r", "K2SO4", "r", "K2S", "r", "K2MnO4", "r", "KAl(SO4)2", "r", "K2Cr2O7", "r", "K", "m", "I2",
                "s", "HgO", "s", "Hg(NO3)2", "r", "Hg", "s", "HPO3", "s", "HNO3", "r", "HI", "r", "HF", "r",
                "HClO", "l", "HCl", "r", "HCIO", "l", "HCHO", "o", "HBr", "r", "HAc", "l", "H4SiO4", "s",
                "H3PO4", "l", "H2SiO3", "s", "H2SO4", "r", "H2SO3", "l", "H2S", "g", "H2O2", "l", "H2O", "l",
                "H2CO3", "l", "H2", "g", "FeSO4", "r", "FeS2", "s", "FeS", "s", "FeO", "s", "FeCl3", "l",
                "FeCl2", "r", "Fe3O4", "s", "Fe2O3", "s", "Fe2(SO4)3", "r", "FeSO4", "r", "Fe(OH)3", "s",
                "Fe(OH)2", "s", "Fe(NO3)2", "r", "Fe", "m", "F2", "g", "CuSO4", "r", "CuS", "s", "CuO", "s",
                "CuCl2", "r", "Cu3P", "s", "Cu2S", "s", "Cu2O", "s", "Cu(OH)2", "s", "Cu(NO3)2", "r", "Cu", "s",
                "Cr2O3", "s", "Cr2(SO4)3", "r", "Cr", "m", "Cl2", "g", "CaSiO3", "s", "CaSO4", "s", "CaSO3",
                "s", "CaO", "s", "CaCl2", "r", "CaCO3", "s", "CaC2", "s", "Ca3(PO4)2", "s", "Ca(OH)2", "r",
                "Ca(HSO3)2", "r", "Ca(HCO3)2", "r", "Ca(H2PO4)2", "r", "Ca(ClO)2", "r", "CS2", "l", "CO2", "g",
                "CO", "g", "CHCl3", "l", "CH4", "g", "CH3OH", "l", "CH3Cl", "g", "CH3COONa", "r", "CH3COONH4",
                "r", "CH3COOH", "l", "CH3COOCH3", "l", "CH3COOC2H5", "l", "CH3CHO", "l", "CH3CH2OSO3H", "o",
                "CH3CH2OH", "l", "CH3CH2Br", "o", "CH2Cl2", "o", "CCl4", "l", "C6H6", "l", "C2H6", "g",
                "C2H5ONa", "r", "C2H5ONO2", "o", "C2H5OH", "l", "C2H5OC2H5", "o", "C2H4", "g", "C2H2", "g",
                "C11H22O11", "s", "C", "s", "Br2", "l", "BaSO4", "s", "BaCl2", "r", "BaCO3", "s", "Ba(NO3)2",
                "r", "AlCl3", "r", "Al2S3", "s", "Al2O3", "s", "Al2(SO4)3", "r", "Al(OH)3", "s", "Al(NO3)3",
                "r", "Al", "m", "AgOH", "s", "AgNO3", "r", "AgI", "s", "AgCl", "s", "AgBr", "s", "Ag(NH3)2OH",
                "r", "Ag", "s", "(NH4)2SO4", "r", "(CH3COO)2Pb", "r", "(C2H5O)3Al", "r", "(C2H5O)2Mg", "r"
        };
namespace cocr {
    ChemistryData::ChemistryData() :
            mass(massData),
            electronegativity(electronegativityData),
            name(nameData),
            formula(formulaData),
            valence(valenceData) {}

}