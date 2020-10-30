#include "config.hpp"

const std::string &getCouchFilePath() {
    static const std::string data_path = getDataDir() + "/couch/couch.dat";
    return data_path;
}

void Timer::start() {
    start_stamp = last_stamp = std::chrono::system_clock::now();
}

void Timer::display_duration() {
    end_stamp = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_stamp - last_stamp);
    std::cout << "Timer mid: [takes "
              << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
              << " ms]" << std::endl;
    last_stamp = end_stamp;
}

void Timer::stop(bool _display_duration) {
    end_stamp = last_stamp = std::chrono::system_clock::now();
    if (_display_duration) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                end_stamp - start_stamp);
        std::cout << "Timer end: [takes "
                  << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
                  << " ms]" << std::endl;
    }
}

std::string trim(const string &str) {
    static std::string blanks("\f\v\r\t\n ");
    std::string ret = str;
    ret.erase(0, str.find_first_not_of(blanks));
    ret.erase(str.find_last_not_of(blanks) + 1);
    return std::move(ret);
}

const std::string &getDrugBankFilePath() {
    static const std::string data_path = getDataDir() + "/drugbank/full database.xml";
    return data_path;
}

const std::string &getAlkaneCacheDir() {
    static const std::string data_path = getDataDir() + "/alkane";
    return data_path;
}

const std::vector<string> ElementsData = {
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

size_t IdInterface::Get_new_id() {
    if (idPool.empty()) {
        return currentMaxId++;
    } else {
        size_t ret = idPool.back();
        idPool.pop_back();
        return ret;
    }
}

void IdInterface::Give_back_id(const size_t &_freedId) {
    idPool.push_back(_freedId);
}

const size_t IdInterface::getId() { return id; }

IdInterface::IdInterface() : id(Get_new_id()) {

}

IdInterface::~IdInterface() {
    Give_back_id(id);
}

std::string deleteSubStr(const string &_target, const string &_subStr) {
    size_t nSize = _subStr.size();
    size_t pos = 0;
    std::string ret = _target;
    while (true) {
        pos = _target.find(_subStr);
        if (pos == string::npos)
            return std::move(ret);
        ret.erase(pos, nSize);
    }
}

std::string replaceSubStr(const string &_target,
                          const string &_subStr, const string &_newSubStr) {
    std::string ret = _target;
    for (string::size_type pos = 0; pos != string::npos; pos += _newSubStr.length()) {
        if ((pos = _target.find(_subStr, pos)) != string::npos)
            ret.replace(pos, _subStr.length(), _newSubStr);
        else
            break;
    }
    return std::move(ret);
}
