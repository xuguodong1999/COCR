#include "generator.hpp"
#include "isomer.hpp"
#include "timer.hpp"
#include "rdkit_util.hpp"
#include "std_util.hpp"

using namespace std;

void SMILESGenerator::traverse(const char *alkane_dir, const size_t &carbonNum,
                               function<void(const hash_type &)> func) {
    ifstream ifsm(alkane_dir + ("/" + to_string(carbonNum)) + ".dat",
                  ios::in | ios::binary);
    if (!ifsm.is_open()) {
        cerr << "fail to open "
             << alkane_dir + ("/" + to_string(carbonNum)) + ".dat"
             << endl;
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
    ofstream ofsm(alkane_dir + ("/" + to_string(carbonNum)) + ".smi");
    if (!ofsm.is_open()) {
        cerr << "fail to open smi for out" << endl;
        exit(-1);
    }
    auto test_func = [&](const hash_type &hash_value) -> void {
        AlkaneGraph<unsigned char> alkane;
        auto smiles = alkane.toSMILES(hash_value);
        auto can = getStandardSMILES(smiles);
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

void SMILESGenerator::HashToRichSMILES(const char *alkane_dir, const size_t &carbonNum) {
//    ofstream ofsm(alkane_dir + ("D:/r" + to_string(carbonNum)) + ".smi");
//    if (!ofsm.is_open()) {
//        cerr << "fail to open smi for out" << endl;
//        exit(-1);
//    }
    auto test_func = [&](const hash_type &hash_value) -> void {
        AlkaneGraph<unsigned char> alkane;
        auto smiles = alkane.toSMILES(hash_value);
        //smiles = "CNC(CC(CSC([C@@](CC)(C)CC(C)C#CC(C)C1=CNC=C1)CC)C(C)C)C";
        //stringstream ism(smiles);
        // TODO:
        //////////////////////////////
        // 第一类：饱和烷烃碳链
        // 第二类：添加=#的烷烃碳链
        // 第三类：什么都有的，比重：倾向于原子数少的
        // 1、环插入 C-> [c4 c5 c6 C3 C4 C5 C6 C7 C8 萘 蒽 菲]
        // 2、化学键替换 --> [/\=#]
        // 3、杂原子or官能团替换 C-> [C H O N P S F Cl Br I B OH CHO COOH]


        //////////////////////////////
        auto can = getStandardSMILES(smiles);
        cout << "in :" << smiles << endl;
        cout << "out:" << trim(can) << endl;
        //ofsm << trim(can) << "\n";
        system("pause");
    };
    Timer timer;
    timer.start();
    traverse(alkane_dir, carbonNum, test_func);
    //ofsm.close();
    timer.stop(true);
}
