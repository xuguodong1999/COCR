#include "interface.hpp"

float R::sNormSizeK = 1.0;
float R::sSubSizeK = 1.0;
float R::sNormOffsetKx = 0.2;
float R::sNormOffsetKy = 0.2;
float R::sSubOffsetKx = 0.1;
float R::sSubOffsetKy = 0.1;
float R::sAngleK = 0.0;
std::set<std::string> R::bsSet, R::aeSet, R::btSet;
std::set<int> R::acSet;
std::vector<std::string> R::sElementData = {
        "None", "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
        "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
        "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
        "Ga", "Ge", "As", "Se", "Br", "Kr",
        "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd",
        "In", "Sn", "Sb", "Te", "I", "Xe",
        "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy",
        "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt",
        "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
        "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No",
        "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn"
};

const char *get_couch_data_path() {
#ifdef WIN32
    return "C:\\Users\\xgd\\source\\COCR\\data\\couch.dat";
#elif defined(unix)
    return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/couch.dat";
#endif
}

const char *get_drugbank_json_path() {
#ifdef WIN32
    return "C:\\Users\\xgd\\source\\COCR\\data\\drugbank.json";
#elif defined(unix)
    return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/drugbank.json";
#endif
}

const char *get_drugbank_h_json_path() {
#ifdef WIN32
    return "C:\\Users\\xgd\\source\\COCR\\data\\drugbank_h.json";
#elif defined(unix)
    return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/drugbank_h.json";
#endif
}

const char *get_chonps_json_path() {
#ifdef WIN32
    return "C:\\Users\\xgd\\source\\COCR\\data\\fake_chonps12345.json";
#elif defined(unix)
    return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/fake_chonps12345.json";
#endif
}
