#include "generator.hpp"
#include "isomer.hpp"
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/mol.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

class JAtom {
    size_t id;
    unsigned char element_type;// 周期表序号
    std::vector<size_t> bond_ids;
};

class JBond {
    size_t id;
    unsigned char order_stereo;// 键级 /\- = #
    std::vector<size_t> atom_ids;
};

class JMol {
    std::vector<std::shared_ptr<JBond>> bonds;
    std::vector<std::shared_ptr<JAtom>> atoms;
};

OpenBabel::OBMol convertJMolToOBMol(const JMol &jmol) {
    return OpenBabel::OBMol();
}

JMol convertOBMolToJMol(const OpenBabel::OBMol &obmol) {
    return JMol();
}


void SMILESGenerator::traverse(const char *alkane_dir, const size_t &carbonNum,
                               std::function<void(const hash_type &)> func) {
    std::ifstream ifsm(alkane_dir + ("/" + std::to_string(carbonNum)) + ".dat",
                       std::ios::in | std::ios::binary);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open "
                  << alkane_dir + ("/" + std::to_string(carbonNum)) + ".dat"
                  << std::endl;
        exit(-1);
    }
    hash_type full_size;
    ifsm.read(reinterpret_cast<char *>(&full_size), sizeof(hash_type));
    for (hash_type i = 0; i < full_size; i++) {
        hash_type tmp;
        ifsm.read(reinterpret_cast< char *>(&tmp), sizeof(hash_type));
        func(tmp);
    }
    ifsm.close();
}

void SMILESGenerator::HashToSMILES(const char *alkane_dir, const size_t &carbonNum) {
    using namespace OpenBabel;
    OBConversion conv;
    auto format_in = conv.FindFormat("smiles");
    auto format_out = conv.FindFormat("can");
//    for(auto&i: conv.GetSupportedOutputFormat()){
//        std::cout<<i<<std::endl;
//    }
    conv.SetInAndOutFormats(format_in, format_out);
    std::ofstream ofsm(alkane_dir + ("/" + std::to_string(carbonNum)) + ".smi");
    if (!ofsm.is_open()) {
        std::cerr << "fail to open smi for out" << std::endl;
        exit(-1);
    }
    auto test_func = [&](const hash_type &hash_value) -> void {
        AlkaneGraph<unsigned char> alkane;
        auto smiles = alkane.toSMILES(hash_value);
        std::stringstream ism(smiles);
        OBMol mol;
        conv.Read(&mol, &ism);
        std::stringstream osm;
        conv.Write(&mol, &osm);
        auto can = osm.str();
        ofsm << trim(can) << "\n";
    };
    Timer timer;
    timer.start();
    traverse(alkane_dir, carbonNum, test_func);
    ofsm.close();
    timer.stop(true);
}

SMILESGenerator &SMILESGenerator::GetInstance() {
    static SMILESGenerator e;
    return e;
}
#include <openbabel/stereo/tetrahedral.h>
void SMILESGenerator::HashToRichSMILES(const char *alkane_dir, const size_t &carbonNum) {
    using namespace OpenBabel;
    OBConversion conv;
    auto format_in = conv.FindFormat("smiles");
    auto format_out = conv.FindFormat("can");
    conv.SetInAndOutFormats(format_in, format_out);
//    std::ofstream ofsm(alkane_dir + ("D:/r" + std::to_string(carbonNum)) + ".smi");
//    if (!ofsm.is_open()) {
//        std::cerr << "fail to open smi for out" << std::endl;
//        exit(-1);
//    }
    auto test_func = [&](const hash_type &hash_value) -> void {
        AlkaneGraph<unsigned char> alkane;
        auto smiles = alkane.toSMILES(hash_value);
        smiles="CNC(CC(CSC([C@@](CC)(C)CC(C)C#CC(C)C1=CNC=C1)CC)C(C)C)C";
        std::stringstream ism(smiles);
        OBMol mol;
        conv.Read(&mol, &ism);
        OBStereoFacade stereoFacade(&mol);
        auto tStereo=stereoFacade.GetAllTetrahedralStereo();
        for(auto&ts:tStereo){
            std::cout<<ts->GetConfig().center<<std::endl;
        }
        //////////////////////////////
        // 第一类：饱和烷烃碳链
        // 第二类：添加=#的烷烃碳链
        // 第三类：什么都有的，比重：倾向于原子数少的
        // 1、环插入 C-> [c4 c5 c6 C3 C4 C5 C6 C7 C8 萘 蒽 菲]
        // 2、化学键替换 --> [/\=#]
        // 3、杂原子or官能团替换 C-> [C H O N P S F Cl Br I B OH CHO COOH]
        for (auto it = mol.BeginBonds(); it != mol.EndBonds(); it++) {
            //(*it)->SetHash(true);
            //(*it)->SetWedge(false);

        }

        for (auto it = mol.BeginAtoms(); it != mol.EndAtoms(); it++) {
            size_t valence = 0;
            for (auto it2 = (*it)->BeginBonds(); it2 != (*it)->EndBonds(); it2++) {
                valence += (*it2)->GetBondOrder();
            }
            std::cout<<(*it)->IsChiral();
            if (valence == 1) {
//                (*it)->SetAtomicNum(9);
            } else if (valence == 2) {
//                (*it)->CountBondsOfOrder(1);
            } else if (valence == 2) {

            } else if (valence == 4) {

            }
        }

//        mol.AddPolarHydrogens();
//        mol.AddNonPolarHydrogens();
        //////////////////////////////
        std::stringstream osm;
        conv.Write(&mol, &osm);
        auto can = osm.str();
        std::cout << trim(can) << std::endl;
        //ofsm << trim(can) << "\n";
        system("pause");
    };
    Timer timer;
    timer.start();
    traverse(alkane_dir, carbonNum, test_func);
    //ofsm.close();
    timer.stop(true);
}
