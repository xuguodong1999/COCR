#include "MaterialStateInfo.h"

const std::unordered_map<std::string, std::string> KNOWN_STATE_INFO{
        {"ZnSO4",       "r"},
        {"ZnCl2",       "r"},
        {"Zn(NO3)2",    "r"},
        {"Zn",          "m"},
        {"x",           "x"},
        {"WO3",         "s"},
        {"W",           "m"},
        {"V2O5",        "s"},
        {"V",           "m"},
        {"SiO2",        "s"},
        {"SiF4",        "g"},
        {"Si",          "s"},
        {"SO3",         "l"},
        {"SO2",         "g"},
        {"s",           "s"},
        {"PhSO3H",      "o"},
        {"PhONa",       "r"},
        {"PhOH",        "s"},
        {"PhNO2",       "o"},
        {"PhCl",        "o"},
        {"PhBr",        "o"},
        {"Ph",          "o"},
        {"PbS",         "s"},
        {"PbAc2",       "r"},
        {"PCl5",        "s"},
        {"PCl3",        "s"},
        {"P2O5",        "s"},
        {"P",           "s"},
        {"O2",          "g"},
        {"NaSO4",       "r"},
        {"NaOH",        "r"},
        {"NaNO3",       "r"},
        {"NaNO2",       "r"},
        {"NaHSO4",      "r"},
        {"NaHSO3",      "r"},
        {"NaHS",        "r"},
        {"NaHCO3",      "r"},
        {"NaClO",       "r"},
        {"NaCl",        "r"},
        {"NaBr",        "r"},
        {"NaAlO2",      "r"},
        {"Na2SiO3",     "r"},
        {"Na2SO4",      "r"},
        {"Na2SO3",      "r"},
        {"Na2S",        "r"},
        {"Na2O2",       "s"},
        {"Na2O",        "s"},
        {"Na2CO3",      "r"},
        {"Na",          "m"},
        {"NO2",         "g"},
        {"NO",          "g"},
        {"NH4NO3",      "r"},
        {"NH4HCO3",     "r"},
        {"NH4Cl",       "r"},
        {"NH3.H2O",     "l"},
        {"NH3",         "g"},
        {"N2O4",        "g"},
        {"N2",          "g"},
        {"MnSO4",       "r"},
        {"MnO2",        "s"},
        {"MnO",         "s"},
        {"MnCl2",       "r"},
        {"Mn",          "m"},
        {"MgSiO3",      "s"},
        {"MgSO4",       "r"},
        {"MgO",         "s"},
        {"MgCl2",       "r"},
        {"MgCO3",       "s"},
        {"Mg3N2",       "s"},
        {"Mg(OH)2",     "s"},
        {"Mg(NO3)2",    "r"},
        {"Mg(HCO3)2",   "r"},
        {"Mg",          "m"},
        {"MX",          "r"},
        {"MOH",         "r"},
        {"MH",          "s"},
        {"m",           "m"},
        {"Li2O",        "s"},
        {"Li",          "m"},
        {"KOH",         "r"},
        {"KO2",         "s"},
        {"KNO3",        "r"},
        {"KMnO4",       "r"},
        {"KI",          "r"},
        {"KClO3",       "r"},
        {"KCl",         "r"},
        {"KBr",         "r"},
        {"K2SO4",       "r"},
        {"K2S",         "r"},
        {"K2MnO4",      "r"},
        {"KAl(SO4)2",   "r"},
        {"K2Cr2O7",     "r"},
        {"K",           "m"},
        {"I2",          "s"},
        {"HgO",         "s"},
        {"Hg(NO3)2",    "r"},
        {"Hg",          "s"},
        {"HPO3",        "s"},
        {"HNO3",        "r"},
        {"HI",          "r"},
        {"HF",          "r"},
        {"HClO",        "l"},
        {"HCl",         "r"},
        {"HCIO",        "l"},
        {"HCHO",        "o"},
        {"HBr",         "r"},
        {"HAc",         "l"},
        {"H4SiO4",      "s"},
        {"H3PO4",       "l"},
        {"H2SiO3",      "s"},
        {"H2SO4",       "r"},
        {"H2SO3",       "l"},
        {"H2S",         "g"},
        {"H2O2",        "l"},
        {"H2O",         "l"},
        {"H2CO3",       "l"},
        {"H2",          "g"},
        {"FeSO4",       "r"},
        {"FeS2",        "s"},
        {"FeS",         "s"},
        {"FeO",         "s"},
        {"FeCl3",       "l"},
        {"FeCl2",       "r"},
        {"Fe3O4",       "s"},
        {"Fe2O3",       "s"},
        {"Fe2(SO4)3",   "r"},
        {"FeSO4",       "r"},
        {"Fe(OH)3",     "s"},
        {"Fe(OH)2",     "s"},
        {"Fe(NO3)2",    "r"},
        {"Fe",          "m"},
        {"F2",          "g"},
        {"CuSO4",       "r"},
        {"CuS",         "s"},
        {"CuO",         "s"},
        {"CuCl2",       "r"},
        {"Cu3P",        "s"},
        {"Cu2S",        "s"},
        {"Cu2O",        "s"},
        {"Cu(OH)2",     "s"},
        {"Cu(NO3)2",    "r"},
        {"Cu",          "s"},
        {"Cr2O3",       "s"},
        {"Cr2(SO4)3",   "r"},
        {"Cr",          "m"},
        {"Cl2",         "g"},
        {"CaSiO3",      "s"},
        {"CaSO4",       "s"},
        {"CaSO3",       "s"},
        {"CaO",         "s"},
        {"CaCl2",       "r"},
        {"CaCO3",       "s"},
        {"CaC2",        "s"},
        {"Ca3(PO4)2",   "s"},
        {"Ca(OH)2",     "r"},
        {"Ca(HSO3)2",   "r"},
        {"Ca(HCO3)2",   "r"},
        {"Ca(H2PO4)2",  "r"},
        {"Ca(ClO)2",    "r"},
        {"CS2",         "l"},
        {"CO2",         "g"},
        {"CO",          "g"},
        {"CHCl3",       "l"},
        {"CH4",         "g"},
        {"CH3OH",       "l"},
        {"CH3Cl",       "g"},
        {"CH3COONa",    "r"},
        {"CH3COONH4",   "r"},
        {"CH3COOH",     "l"},
        {"CH3COOCH3",   "l"},
        {"CH3COOC2H5",  "l"},
        {"CH3CHO",      "l"},
        {"CH3CH2OSO3H", "o"},
        {"CH3CH2OH",    "l"},
        {"CH3CH2Br",    "o"},
        {"CH2Cl2",      "o"},
        {"CCl4",        "l"},
        {"C6H6",        "l"},
        {"C2H6",        "g"},
        {"C2H5ONa",     "r"},
        {"C2H5ONO2",    "o"},
        {"C2H5OH",      "l"},
        {"C2H5OC2H5",   "o"},
        {"C2H4",        "g"},
        {"C2H2",        "g"},
        {"C11H22O11",   "s"},
        {"C",           "s"},
        {"Br2",         "l"},
        {"BaSO4",       "s"},
        {"BaCl2",       "r"},
        {"BaCO3",       "s"},
        {"Ba(NO3)2",    "r"},
        {"AlCl3",       "r"},
        {"Al2S3",       "s"},
        {"Al2O3",       "s"},
        {"Al2(SO4)3",   "r"},
        {"Al(OH)3",     "s"},
        {"Al(NO3)3",    "r"},
        {"Al",          "m"},
        {"AgOH",        "s"},
        {"AgNO3",       "r"},
        {"AgI",         "s"},
        {"AgCl",        "s"},
        {"AgBr",        "s"},
        {"Ag(NH3)2OH",  "r"},
        {"Ag",          "s"},
        {"(NH4)2SO4",   "r"},
        {"(CH3COO)2Pb", "r"},
        {"(C2H5O)3Al",  "r"},
        {"(C2H5O)2Mg",  "r"}
};