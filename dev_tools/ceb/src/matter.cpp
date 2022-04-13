#include "matter.h"
#include "base/std_util.h"
#include "balance_util.h"
#include <iomanip>

using namespace std;
const int ELECOL = 3;

Matter::Matter() {
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::Matter(string &_name) {
    name = _name;
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::Matter(char *_name) {
    name = _name;
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::~Matter() {
    clear();
}

void Matter::clear() {
    name.clear();
    stateInfo.clear();
    eleList.clear();
    amount = 0;
    state = 'l';
    q = 0;
}

void Matter::inputName() {
    getline(std::cin, name);
}

bool Matter::Exist() {
    string tmp1 = "#CCl4#CH3Cl#C2H5OC2H5#KO2#CuCl2#CHCl3#CH3COONa#CH3COOCH3#CH3COOC2H5#CH3CH2Br#CH2Cl2#C2H5ONa#(C2H5O)2Mg#C2H5ONO2#CH3CH2OSO3H#(C2H5O)3Al#CH3CH2OH#C11H22O11#Ba(NO3)2#Al2S3#NaAlO2#NH3·H2O#AgOH#Ca(ClO)2#Ca(H2PO4)2#CH4#C2H6#C2H4#C2H2#Fe(NO3)2#CaC2#CH3CHO#C2H5OH#Fe(OH)2#Ca(HCO3)2#CH3OH#Ag(NH3)2OH#HCHO#HClO#K2MnO4#NaH#Mg(HCO3)2#Mg3N2#MgSiO3#NO2#NO#N2O4#Na2SO4#Cu(OH)2#H4SiO4#H2SiO3#NaBr#NaHS#NaHSO3#NaHSO4#Cu3P#HPO3#PCl5#PCl3#C6H5SO3H#C6H12#C6H6#C6H5Br#C6H5Cl#CS2#K2S#Ca(HSO3)2#CaSO3#CaSiO3#ZrOCl2#ZrO2#Zr(SO4)2#Zr(NO3)4#Zr#ZnSO4#ZnS#ZnO#ZnCl2#ZnCO3#ZnC2O4#Zn2P2O7#Zn(NO3)2#Zn#WO3#W#VO2#V2O5#V#UO3#UF4#UCl4#U3O8#U#TiOSO4#TiO2#TiCl4#TiCl3#Ti#ThCl4#Th(SO4)2#Th(NO3)4#Th(C2O4)2#Th#SrSO4#SrO#SrCl2#SrCO3#SrC2O4#Sr3(PO4)2#Sr(NO3)2#Sr#SnS2#SnS#SnO2#SnCl2#Sn#SiO2#SiF4#SiCl4#Si#SbCl5#SbCl3#Sb2O5#Sb2O3#Sb#SO3#SO2#S#PbbI2#PbSO4#PbS#PbO2#PbO#PbCrO4#PbCl2#PbCO3#PbC2O4#Pb3O4#Pb3(PO4)2#Pb2O3#Pb(NO3)2#Pb(IO3)2#Pb#P2O5#P#O3#O2#O#NiSO4#NiS#NiO#NiCl2#Ni(NO3)2#Ni#NaOH#NaNO3#NaNO2#NaI#NaHCO3#NaH2PO4#NaH#NaClO#NaCl#NaCO3#NaCN#NaBrO3#NaBiO3#NaAsO4#Na3PO4#Na3AsO4#Na3AsO3#Na2SO4#Na2SiO3#Na2SO3#Na2S2O3#Na2S#Na2O2#Na2O#Na2HPO4#Na2HAsO3#Na2CrO4#Na2CO3#Na2C2O4#Na2B4O7#Na#NH4VO3#NH4NO3#NH4HCO3#NH4H2PO4#NH4Cl#NH3#N2#N#MnSO4#MnS#MnO2#MnO#MnCl2#MnCO3#Mn3O4#Mn2P2O7#Mn2O3#Mn(NO3)2#Mn#MgSO4#MgO#MgNH4PO4#MgNH4AsO4#MgH2#MgCl2#MgCO3#MgC2O4#Mg2P2O7#Mg(OH)2#Mg(NO3)2#Mg#LiOH#LiH#LiCl#Li2O#Li2CO3#Li#KOH#KNaC4O6H4#KNO3#KNO2#KMnO4#KIO3#KI#KHSO4#KHC8O4H8#KHC4O6H4#KHC2O4#KH#KFe(SO4)2#KF#KClO4#KClO3#KCl#KBrO3#KBr#KAl(SO4)2#K4Fe(CN)6#K3PO4#K3Fe(CN)6#K3AsO4#K2SO4#K2S2O7#K2PtCl6#K2O#K2CrO4#K2Cr2O7#K2CO3#K#I2#I#HgSO4#HgS#HgO#HgI2#HgCl2#Hg2SO4#Hg2I2#Hg2Cl2#Hg2Br2#Hg2(NO3)2#Hg(NO3)2#Hg(CN)2#Hg#HNO3#HNO2#HIO3#HI#HF#HClO4#HCl#HCOONa#HCOOH#HCN#HBr#H3PO4#H3PO3#H3BO3#H3AsO4#H3AsO3#H2SO4#H2SO3#H2S#H2O2#H2O#H2CO3#H2C2O4#H2#H#FeSO4#FeS2#FeS#FeO#FeCl3#FeCl2#FeCO3#Fe3O4#Fe2O3#Fe2(SO4)3#Fe(OH)3#Fe(NO3)3#Fe#F2#F#CuSO4#CuSCN#CuS#CuO#CuCl2CuI#CuCl#Cu2S#Cu2O#Cu2(OH)2CO3#Cu(NO3)2#Cu#CsOH#Cs#CrCl3#Cr2O3#Cr2(SO4)3#Cr#CoSO4#CoS#CoCl2#Co3O4#Co2O3#Co(NO3)2#Co#Cl2#Cl#CeO2#Ce(SO4)2#Ce#CdS#CdCl2#CdCO3#Cd#CaSO4#CaO#CaCl2#CaCO3#CaC2O4#Ca3(PO4)2#Ca(OH)2#Ca#CS(NH2)2#CO2#CO(NH2)2#CO#CH3COONH4#CH3COOH#C6H5SO3H#C6H5ONa#C6H5OH#C6H5NO2#C6H5Cl#C6H5COOH#C6H5Br#C6H12#C#Br2#Br#BiONO3#BiOHCO3#BiOCl#BiCl3#Bi2S3#Bi2O3#Bi(NO3)3#Bi#BeO#Be#BaSO4#BaO#BaCrO4#BaCl2#BaCO3#BaBr2#Ba3(AsO4)2#Ba(OH)2#Ba#B2O3#B#As2S3#As2O5#As2O3#As#AlCl3#AlBr3#Al2O3#Al2(SO4)3#Al(OH)3#Al(NO3)3#Al#AgSCN#AgNO3#AgI#AgCl#AgCN#AgBrO3#AgBr#Ag3AsO4#Ag2SO4#Ag2CrO4#Ag#(NH4)3PO4#(NH4)2SO4#(NH4)2S#(NH4)2PtCl6#(NH4)2MoO4#(NH4)2HPO4#(NH4)2Fe(SO4)2#(NH4)2CO3#(NH4)2C2O4#(CH3COO)2UO2#(CH3COO)2Pb#(CH3CO)2O#(CH2COO)2Zn#";
    string tmp2 = "#" + name + "#";
    if (tmp1.find(tmp2) == string::npos)
        return 0;
    else
        return 1;
}

void Matter::getStateInfo() {
    string tmp1 = "#ZnSO4#r#ZnCl2#r#Zn(NO3)2#r#Zn#m#X#x#WO3#s#W#m#V2O5#s#V#m#SiO2#s#SiF4#g#Si#s#SO3#l#SO2#g#S#s#PhSO3H#o#PhONa#r#PhOH#s#PhNO2#o#PhCl#o#PhBr#o#Ph#o#PbS#s#PbAc2#r#PCl5#s#PCl3#s#P2O5#s#P#s#O2#g#NaSO4#r#NaOH#r#NaNO3#r#NaNO2#r#NaHSO4#r#NaHSO3#r#NaHS#r#NaHCO3#r#NaClO#r#NaCl#r#NaBr#r#NaAlO2#r#Na2SiO3#r#Na2SO4#r#Na2SO3#r#Na2S#r#Na2O2#s#Na2O#s#Na2CO3#r#Na#m#NO2#g#NO#g#NH4NO3#r#NH4HCO3#r#NH4Cl#r#NH3·H2O#l#NH3#g#N2O4#g#N2#g#MnSO4#r#MnO2#s#MnO#s#MnCl2#r#Mn#m#MgSiO3#s#MgSO4#r#MgO#s#MgCl2#r#MgCO3#s#Mg3N2#s#Mg(OH)2#s#Mg(NO3)2#r#Mg(HCO3)2#r#Mg#m#MX#r#MOH#r#MH#s#M#m#Li2O#s#Li#m#KOH#r#KO2#s#KNO3#r#KMnO4#r#KI#r#KClO3#r#KCl#r#KBr#r#K2SO4#r#K2S#r#K2MnO4#r#KAl(SO4)2#r#K2Cr2O7#r#K#m#I2#s#HgO#s#Hg(NO3)2#r#Hg#s#HPO3#s#HNO3#r#HI#r#HF#r#HClO#l#HCl#r#HCIO#l#HCHO#o#HBr#r#HAc#l#H4SiO4#s#H3PO4#l#H2SiO3#s#H2SO4#r#H2SO3#l#H2S#g#H2O2#l#H2O#l#H2CO3#l#H2#g#FeSO4#r#FeS2#s#FeS#s#FeO#s#FeCl3#l#FeCl2#r#Fe3O4#s#Fe2O3#s#Fe2(SO4)3#r#FeSO4#r#Fe(OH)3#s#Fe(OH)2#s#Fe(NO3)2#r#Fe#m#F2#g#CuSO4#r#CuS#s#CuO#s#CuCl2#r#Cu3P#s#Cu2S#s#Cu2O#s#Cu(OH)2#s#Cu(NO3)2#r#Cu#s#Cr2O3#s#Cr2(SO4)3#r#Cr#m#Cl2#g#CaSiO3#s#CaSO4#s#CaSO3#s#CaO#s#CaCl2#r#CaCO3#s#CaC2#s#Ca3(PO4)2#s#Ca(OH)2#r#Ca(HSO3)2#r#Ca(HCO3)2#r#Ca(H2PO4)2#r#Ca(ClO)2#r#CS2#l#CO2#g#CO#g#CHCl3#l#CH4#g#CH3OH#l#CH3Cl#g#CH3COONa#r#CH3COONH4#r#CH3COOH#l#CH3COOCH3#l#CH3COOC2H5#l#CH3CHO#l#CH3CH2OSO3H#o#CH3CH2OH#l#CH3CH2Br#o#CH2Cl2#o#CCl4#l#C6H6#l#C2H6#g#C2H5ONa#r#C2H5ONO2#o#C2H5OH#l#C2H5OC2H5#o#C2H4#g#C2H2#g#C11H22O11#s#C#s#Br2#l#BaSO4#s#BaCl2#r#BaCO3#s#Ba(NO3)2#r#AlCl3#r#Al2S3#s#Al2O3#s#Al2(SO4)3#r#Al(OH)3#s#Al(NO3)3#r#Al#m#AgOH#s#AgNO3#r#AgI#s#AgCl#s#AgBr#s#Ag(NH3)2OH#r#Ag#s#(NH4)2SO4#r#(CH3COO)2Pb#r#(C2H5O)3Al#r#(C2H5O)2Mg#r####";
    string tmp2 = "#" + name + "#";
    size_t index = tmp1.find(tmp2);
    if (index == string::npos)
        state = 'g';//找不到，初始化为气体
    else {
        index++;
        while (tmp1[index] != '#')
            index++;
        state = tmp1[index + 1];
    }
}

bool Matter::intoElement() {
    //  格式预处理
    string tar = name;
    if (!preProcess(tar)) {
#ifdef TSETCEI
        cout << "数据预处理失败！处理前为 \""
            << name << "\" ；处理后为 \"" << tar << "\" 。" << endl;
#endif
        return 0;
    }
    //  词法分析
    int i = 0, j = 0, k = 0;
    vector<MatNode> matlist;
    if (!lexicalAnalysis(matlist, tar)) {
#ifdef TESTCEI
        cout << "词法分析失败！处理前为 \"" << name << "\" ，已处理的信息：" << endl;
        cout << "(int)matlist.size=" << (int)matlist.size() << ", matlist=";
        for (i = 0; i < (int)matlist.size(); i++)
            matlist[i].print();
#endif
        return 0;
    }
    //  进一步词法分析，循环处理最小单元
    furtherLexicalAnalysis(matlist);
    //  存入元素列表
    eleList.clear();
    for (i = 0; i < (int) matlist[0].block.size(); i++)
        eleList.push_back(matlist[0].block[i]);
    matlist.clear();
#ifdef TESTCEI
    cout << "物质解析完成，M(" << name << ") = " << getRelativeMolecureMass() << " g/mol" << endl;
#endif
    return 1;
}

void Matter::printElement() {
    size_t i = 0;
    size_t col = 1;
    size_t leneleList = eleList.size();
    for (i = 0; i < leneleList; i++)
        col = (col > eleList[i].name.length() ? col : eleList[i].name.length());
    for (i = 0; i < leneleList; i++)
        cout << name << " 的元素 " << std::setw(col) << eleList[i].name << " 有 " << std::setw(4) << eleList[i].amount
             << "份" << endl;
}

bool Matter::getValence() {
    string tmp1 = "#CCl4#4/1#-1/1#CH3Cl#-2/1#-1/1#1/1#C2H5OC2H5#-2/1#1/1#-2/1#KO2#1/1#-1/2#CuCl2#-1/1#2/1#CHCl3#2/1#-1/1#1/1#CH3Cl#-2/1#-1/1#1/1#CH3COONa#0/1#1/1#1/1#-2/1#CH3COOCH3#-2/3#1/1#-2/1#CH3COOC2H5#-1/1#1/1#-2/1#CH3CH2Br#-1/1#-2/1#1/1#CH2Cl2#0/1#-1/1#1/1#C2H5ONa#-2/1#1/1#1/1#-2/1#(C2H5O)2Mg#-2/1#1/1#2/1#-2/1#C2H5ONO2#-2/1#1/1#5/1#-2/1#CH3CH2OSO3H#-2/1#1/1#-2/1#6/1#(C2H5O)3Al#3/1#-2/1#1/1#-2/1#CH3CH2OH#-2/1#1/1#-2/1#C11H22O11#0/1#1/1#-2/1#Ba(NO3)2#2/1#5/1#-2/1#Al2S3#3/1#-2/1#NaAlO2#3/1#1/1#-2/1#NH3·H2O#1/1#-3/1#-2/1#AgOH#1/1#1/1#-2/1#Ca(ClO)2#2/1#1/1#-2/1#Ca(H2PO4)2#2/1#1/1#-2/1#5/1#CH4#-4/1#1/1#C2H6#-3/1#1/1#C2H4#-2/1#1/1#C2H2#-1/1#1/1#Fe(NO3)2#2/1#5/1#-2/1#CaC2#-1/1#2/1#CH3CHO#-1/1#1/1#-2/1#C2H5OH#-2/1#1/1#-2/1#Fe(OH)2#2/1#1/1#-2/1#Ca(HCO3)2#4/1#2/1#1/1#-2/1#CH3OH#-2/1#1/1#-2/1#Ag(NH3)2OH#1/1#1/1#-3/1#-2/1#HCHO#0/1#1/1#-2/1#HClO#1/1#1/1#-2/1#MgH2#-1/1#2/1#K2MnO4#1/1#6/1#-2/1#KH#-1/1#1/1#LiH#-1/1#1/1#NaH#-1/1#1/1#Cs#0/1#CsOH#1/1#1/1#-2/1#KF#-1/1#1/1#Mg(HCO3)2#4/1#1/1#2/1#-2/1#Mg3N2#2/1#-3/1#MgSiO3#2/1#-2/1#4/1#N2O4#4/1#-2/1#NO#2/1#-2/1#NO2#4/1#-2/1#Na2SO4#1/1#-2/1#6/1#Cu(OH)2#2/1#1/1#-2/1#H4SiO4#1/1#-2/1#4/1#H2SiO3#1/1#-2/1#4/1#NaBr#-1/1#1/1#NaHS#1/1#1/1#-2/1#NaHSO3#1/1#1/1#-2/1#4/1#NaHSO4#1/1#1/1#-2/1#6/1#Cu3P#1/1#-3/1#HPO3#1/1#-2/1#5/1#PCl5#-1/1#5/1#PCl3#-1/1#3/1#C6H5SO3H#-2/3#1/1#-2/1#4/1#C6H12#-2/1#1/1#C6H5NO2#-2/3#1/1#3/1#-2/1#C6H6#-1/1#1/1#C6H5Br#-2/3#-1/1#1/1#C6H5Cl#-2/3#-1/1#1/1#C6H5OH#-2/3#1/1#-2/1#C6H5ONa#-2/3#1/1#1/1#-2/1#CS2#4/1#-2/1#K2S#1/1#-2/1#Ca(HSO3)2#2/1#1/1#-2/1#4/1#CaSO3#2/1#-2/1#4/1#CaSiO3#2/1#-2/1#4/1#Na2SiO3#1/1#-2/1#4/1#Ag#0/1#AgBr#1/1#-1/1#AgBrO3#1/1#5/1#-2/1#AgCl#1/1#-1/1#AgI#1/1#-1/1#AgNO3#1/1#5/1#-2/1#Ag2CrO4#1/1#6/1#-2/1#Ag2SO4#1/1#-2/1#6/1#Ag3AsO4#1/1#5/1#-2/1#Al#0/1#AlBr3#3/1#-1/1#AlCl3#3/1#-1/1#Al(NO3)3#3/1#5/1#-2/1#Al2O3#3/1#-2/1#Al(OH)3#3/1#1/1#-2/1#Al2(SO4)3#3/1#-2/1#6/1#As#0/1#As2O3#3/1#-2/1#As2O5#5/1#-2/1#H3AsO4#5/1#1/1#-2/1#B#0/1#B2O3#3/1#-2/1#Ba#0/1#BaBr2#2/1#-1/1#BaCO3#2/1#4/1#-2/1#BaCl2#2/1#-1/1#BaCrO4#2/1#6/1#-2/1#BaO#2/1#-2/1#Ba(OH)2#2/1#1/1#-2/1#BaSO4#2/1#-2/1#6/1#Ba3(AsO4)2#5/1#2/1#-2/1#Be#0/1#BeO#2/1#-2/1#Bi#0/1#Bi2O3#3/1#-2/1#Br#0/1#NaBrO3#5/1#1/1#-2/1#Br2#0/1#C#0/1#CH3COOH#0/1#1/1#-2/1#(CH3CO)2O#0/1#1/1#-2/1#NaAsO4#7/1#1/1#-2/1#CO#2/1#-2/1#CO2#4/1#-2/1#Na2C2O4#3/1#1/1#-2/1#Ca#0/1#CaCl2#2/1#-1/1#CaCO3#4/1#2/1#-2/1#CaC2O4#3/1#2/1#-2/1#CaO#2/1#-2/1#Ca(OH)2#2/1#1/1#-2/1#CaSO4#2/1#-2/1#6/1#Ca3(PO4)2#2/1#-2/1#5/1#Cd#0/1#Ce#0/1#CeO2#4/1#-2/1#Cl2#0/1#Cl#0/1#Co#0/1#Cr#0/1#Na2CrO4#6/1#1/1#-2/1#Cr2O3#3/1#-2/1#Cu#0/1#CuO#2/1#-2/1#Cu2O#1/1#-2/1#F#0/1#F2#0/1#Fe#0/1#FeO#2/1#-2/1#Fe(OH)3#3/1#1/1#-2/1#Fe2O3#3/1#-2/1#Fe3O4#8/3#-2/1#H#0/1#HBr#-1/1#1/1#HCOOH#2/1#1/1#-2/1#CH3COOH#0/1#1/1#-2/1#C6H5COOH#-2/7#1/1#-2/1#HCl#-1/1#1/1#HClO4#7/1#1/1#-2/1#HF#-1/1#1/1#HI#1/1#-1/1#HIO3#1/1#5/1#-2/1#HNO2#1/1#3/1#-2/1#H2#0/1#H2CO3#4/1#1/1#-2/1#H2C2O4#3/1#1/1#-2/1#H2O#1/1#-2/1#H2S#1/1#-2/1#H2SO3#1/1#-2/1#4/1#H2SO4#1/1#-2/1#6/1#H3AsO3#3/1#1/1#-2/1#H3AsO4#5/1#1/1#-2/1#H3BO3#3/1#1/1#-2/1#H3PO3#1/1#-2/1#3/1#H3PO4#1/1#-2/1#5/1#Hg#0/1#HgO#2/1#-2/1#I#0/1#I2#0/1#K#0/1#KAl(SO4)2#3/1#1/1#-2/1#6/1#KBr#-1/1#1/1#KBrO3#5/1#1/1#-2/1#KCl#-1/1#1/1#KClO3#5/1#1/1#-2/1#KClO4#7/1#1/1#-2/1#KHC2O4#3/1#1/1#1/1#-2/1#KHC4O6H4#3/2#1/1#1/1#-2/1#KHC8O4H8#1/-4#1/1#1/1#-2/1#KHSO4#1/1#1/1#-2/1#6/1#KI#-1/1#1/1#KIO3#5/1#1/1#-2/1#KMnO4#1/1#7/1#-2/1#KNO2#1/1#3/1#-2/1#KNO3#1/1#5/1#-2/1#KNaC4O6H4#3/2#1/1#1/1#1/1#-2/1#KOH#1/1#1/1#-2/1#K2CO3#4/1#1/1#-2/1#K2CrO4#6/1#1/1#-2/1#K2Cr2O7#6/1#1/1#-2/1#K2SO4#1/1#-2/1#6/1#K2S2O7#1/1#-2/1#6/1#K3AsO4#5/1#1/1#-2/1#K3PO4#1/1#-2/1#5/1#Li#0/1#LiCl#-1/1#1/1#LiOH#1/1#1/1#-2/1#Li2CO3#4/1#1/1#-2/1#Li2O#1/1#-2/1#Mg#0/1#MgCO3#4/1#2/1#-2/1#MgC2O4#3/1#2/1#-2/1#MgCl2#-1/1#2/1#Mg(NO3)2#2/1#5/1#-2/1#MgO#2/1#-2/1#Mg(OH)2#1/1#2/1#-2/1#MgSO4#2/1#-2/1#6/1#Mg2P2O7#2/1#-2/1#5/1#Mn#0/1#MnO#2/1#-2/1#MnO2#4/1#-2/1#Mn2O3#3/1#-2/1#Mn3O4#8/3#-2/1#N#0/1#N2#0/1#NH3#1/1#-3/1#HNO3#1/1#5/1#-2/1#NaNO3#5/1#1/1#-2/1#Na#0/1#NaBiO3#5/1#1/1#-2/1#Na#0/1#NaBrO3#5/1#1/1#-2/1#HCOONa#2/1#1/1#1/1#-2/1#NaCl#-1/1#1/1#NaClO#1/1#1/1#-2/1#NaHCO3#4/1#1/1#1/1#-2/1#NaH2PO4#1/1#1/1#-2/1#5/1#NaI#-1/1#1/1#NaNO2#3/1#1/1#-2/1#NaNO3#5/1#1/1#-2/1#NaOH#1/1#1/1#-2/1#Na2B4O7#3/1#1/1#-2/1#Na2CO3#4/1#1/1#-2/1#Na2C2O4#3/1#1/1#-2/1#Na2HAsO3#3/1#1/1#1/1#-2/1#Na2HPO4#1/1#1/1#-2/1#5/1#Na2O#1/1#-2/1#Na2S#1/1#-2/1#Na2SO3#1/1#-2/1#4/1#Na2SO4#1/1#-2/1#6/1#Na2S2O3#1/1#-2/1#2/1#Na3AsO3#3/1#1/1#-2/1#Na3AsO4#5/1#1/1#-2/1#Na3PO4#1/1#-2/1#5/1#Ni#0/1#NiO#2/1#-2/1#O#0/1#H2O#1/1#-2/1#O2#0/1#O3#0/1#P#0/1#H3PO4#1/1#-2/1#5/1#P2O5#-2/1#5/1#Pb#0/1#PbO#-2/1#2/1#PbO2#-2/1#4/1#Pb2O3#-2/1#3/1#Pb3O4#-2/1#8/3#S#0/1#SO2#-2/1#4/1#SO3#-2/1#6/1#Na2SO4#1/1#-2/1#6/1#Sb#0/1#Sb2O3#-2/1#3/1#Sb2O5#-2/1#5/1#Si#0/1#SiF4#-1/1#4/1#SiO2#-2/1#4/1#Sn#0/1#SnO2#-2/1#4/1#Sr#0/1#SrO#-2/1#2/1#Th#0/1#Ti#0/1#TiO2#-2/1#4/1#U#0/1#UF4#-1/1#4/1#UO3#-2/1#6/1#U3O8#-2/1#16/3#V#0/1#VO2#-2/1#4/1#V2O5#-2/1#5/1#W#0/1#WO3#-2/1#6/1#Zn#0/1#ZnCO3#4/1#-2/1#2/1#ZnC2O4#3/1#-2/1#2/1#(CH2COO)2Zn#1/2#1/1#-2/1#2/1#ZnCl2#-1/1#2/1#Zn(NO3)2#5/1#-2/1#2/1#ZnO#-2/1#2/1#ZnS#-2/1#2/1#ZnSO4#-2/1#6/1#2/1#Zn2P2O7#-2/1#5/1#2/1#Zr#0/1#ZrO2#-2/1#4/1#KAl(SO4)2#3/1#1/1#-2/1#6/1#BiCl3#3/1#-1/1#Bi(NO3)3#3/1#5/1#-2/1#BiOCl#3/1#-1/1#-2/1#BiONO3#3/1#5/1#-2/1#NaCO3#4/1#1/1#-2/1#CS(NH2)2#4/1#1/1#-3/1#-2/1#CdS#2/1#-2/1#CoCl2#-1/1#2/1#CdCl2#2/1#-1/1#Co(NO3)2#2/1#5/1#-2/1#CoSO4#2/1#-2/1#6/1#Co2O3#3/1#-2/1#Co3O4#8/3#-2/1#CrCl3#-1/1#3/1#CuCl#-1/1#1/1#CuCl2#-1/1#2/1#CuI#1/1#-1/1#Cu(NO3)2#2/1#5/1#-2/1#CuSO4#2/1#-2/1#6/1#Cu2S#1/1#-2/1#FeCO3#4/1#2/1#-2/1#FeCl2#-1/1#2/1#FeCl3#-1/1#3/1#Fe(NO3)3#3/1#5/1#-2/1#FeSO4#2/1#-2/1#6/1#Fe2(SO4)3#3/1#-2/1#6/1#HgCl2#-1/1#2/1#HgI2#2/1#-1/1#Hg(NO3)2#2/1#5/1#-2/1#HgSO4#2/1#-2/1#6/1#Hg2Br2#-1/1#1/1#Hg2Cl2#-1/1#1/1#Hg2I2#1/1#-1/1#Hg2(NO3)2#1/1#5/1#-2/1#Hg2SO4#1/1#-2/1#6/1#KFe(SO4)2#3/1#1/1#-2/1#6/1#K3Fe(CN)6#2/1#3/1#1/1#-3/1#K4Fe(CN)6#2/1#2/1#1/1#-3/1#MnCO3#4/1#2/1#-2/1#MnCl2#-1/1#2/1#Mn(NO3)2#2/1#5/1#-2/1#MnSO4#2/1#-2/1#6/1#NiCl2#-1/1#2/1#Ni(NO3)2#5/1#2/1#-2/1#NiSO4#2/1#-2/1#6/1#PbCO3#4/1#-2/1#2/1#PbC2O4#3/1#-2/1#2/1#PbCl2#-1/1#2/1#PbI2#-1/1#2/1#Pb(NO3)2#5/1#-2/1#2/1#PbSO4#-2/1#2/1#6/1#Pb3(PO4)2#-2/1#5/1#2/1#SbCl3#-1/1#3/1#SbCl5#-1/1#5/1#SiCl4#-1/1#4/1#SnCl2#-1/1#2/1#SrCO3#4/1#-2/1#2/1#SrCl2#-1/1#2/1#Sr(NO3)2#5/1#-2/1#2/1#SrSO4#-2/1#6/1#2/1#Sr3(PO4)2#-2/1#5/1#2/1#ThCl4#-1/1#4/1#Th(NO3)4#5/1#-2/1#4/1#Th(SO4)2#-2/1#6/1#4/1#TiCl3#-1/1#3/1#TiCl4#-1/1#4/1#TiOSO4#-2/1#6/1#4/1#UCl4#-1/1#4/1#Zr(NO3)4#5/1#-2/1#4/1#ZrOCl2#-1/1#-2/1#4/1#Zr(SO4)2#-2/1#6/1#4/1#BiOHCO3#3/1#4/1#1/1#-2/1#CdCO3#4/1#2/1#-2/1#Ce(SO4)2#4/1#-2/1#6/1#Cr2(SO4)3#3/1#-2/1#6/1#Cu2(OH)2CO3#4/1#2/1#1/1#-2/1#Mn2P2O7#2/1#-2/1#5/1#PbCrO4#6/1#-2/1#2/1#AgCN#1/1#2/1#-3/1#AgSCN#1/1#4/1#-3/1#-2/1#Bi2S3#3/1#-2/1#HCN#2/1#1/1#-3/1#NaCN#2/1#-3/1#1/1#CO(NH2)2#4/1#1/1#-3/1#-2/1#CoS#2/1#-2/1#CuS#2/1#-2/1#(NH4)2Fe(SO4)2#2/1#1/1#-3/1#-2/1#6/1#FeS#2/1#-2/1#FeS2#4/1#-2/1#HCN#2/1#1/1#-3/1#HgS#2/1#-2/1#K2PtCl6#-1/1#1/1#4/1#MgNH4AsO4#5/1#1/1#2/1#-3/1#-2/1#MgNH4PO4#1/1#2/1#-3/1#-2/1#5/1#MnS#2/1#-2/1#CH3COONH4#0/1#1/1#-3/1#-2/1#NH4Cl#-1/1#1/1#-3/1#NH4HCO3#4/1#1/1#-3/1#-2/1#NH4H2PO4#1/1#-3/1#-2/1#5/1#NH4NO3#1/1#1/1#-2/1#NH4VO3#1/1#-3/1#-2/1#5/1#(NH4)2CO3#4/1#1/1#-3/1#-2/1#(NH4)2C2O4#3/1#1/1#-3/1#-2/1#(NH4)2HPO4#1/1#-3/1#-2/1#5/1#(NH4)2MoO4#1/1#6/1#-3/1#-2/1#(NH4)2PtCl6#-1/1#1/1#-3/1#4/1#(NH4)2S#1/1#-3/1#-2/1#(NH4)2SO4#1/1#-3/1#-2/1#-2/1#(NH4)3PO4#1/1#-3/1#-2/1#5/1#NiS#2/1#-2/1#PbS#2/1#-2/1#SnS#-2/1#2/1#SnS2#-2/1#4/1#SrC2O4#3/1#-2/1#2/1#(CH3COO)2Pb#0/1#1/1#-2/1#2/1#As2S3#3/1#-2/1#Pb(IO3)2#5/1#2/1#-2/1#CuSCN#4/1#1/1#-3/1#-2/1#Hg(CN)2#2/1#2/1#-3/1#Hg2(CN)2#2/1#1/1#-3/1#Th(C2O4)2#3/1#-2/1#4/1#(CH3COO)2UO2#0/1#1/1#-2/1#6/1#H2O2#1/1#-1/1#K2O#1/1#-2/1#KO2#1/1#-1/2#BaO2#2/1#-1/1#K2O2#1/1#-1/1#Na2O2#1/1#-1/1#";
    string tmp2 = "#" + name + "#";
    size_t index = tmp1.find(tmp2);
    if (index == string::npos)
        return 0;
    index += tmp2.length();
    int ival = 0, i = 0;
//        char *cval = new char[MAXC];
    for (i = 0; i < (int) eleList.size(); i++) {
        ival = 0;
        std::string cval;
        while (tmp1[index] != '#')
            cval.push_back(tmp1[index++]);
        index++;
        eleList[i].valence = to_frac(cval);
    }
//        delete[]cval;
    return 1;
}

bool Matter::lexicalAnalysis(vector<MatNode> &matlist, string &tar) {
    Element eletmp;
    MatNode tmpnode;
    int tarlen = (int) tar.length();
    int i = 0, iend = 0, j = 0, amount = 0;
    for (i = 0; i < tarlen; i++) {
        tmpnode.clear();
        if (tar[i] >= '0' && tar[i] <= '9') {
            amount = 0;
            j = i;
            while (j < tarlen && tar[j] >= '0' && tar[j] <= '9')
                amount = amount * 10 + tar[j++] - '0';
            i = j - 1;
            tmpnode.num = amount;
            tmpnode.isNum = 1;
            if (i == 0 || (i > 0 && tar[i - 1] == '.'))
                tmpnode.isHead = 1;
            else
                tmpnode.isHead = 0;
        } else if ((tar[i] >= 'a' && tar[i] <= 'z') || (tar[i] >= 'A' && tar[i] <= 'Z')) {
            iend = ((i + ELECOL >= tarlen) ? (tarlen - 1) : (i + ELECOL));
            while (iend >= i) {
                eletmp.clear();
                for (j = i; j <= iend; j++)
                    eletmp.name += tar[j];
                if (eletmp.Exist())
                    break;
                else
                    iend--;
            }
            if (iend < i) {
#ifdef TESTCEI
                cout << "遇到无法解析的字符串！在 \"" << tar << "\" 中。" << endl;
#endif
                return 0;
            }
            eletmp.amount = 1;
            tmpnode.block.push_back(eletmp);
            tmpnode.isNum = 0;
            i = iend;
        } else {//  用num的负数域标记特殊字符
            switch (tar[i]) {
                case '(':
                    tmpnode.num = -7;
                    break;
                case ')':
                    tmpnode.num = -6;
                    break;
                default:
#ifdef TESTCEI
                    cout << "发现不合法字符！是 \"" << tar << "\" 中的第"
                        << i + 1 << "个字符 \"" << tar[i] << "\" 。" << endl;
#endif
                    return 0;
            }
            tmpnode.isNum = 1;
        }
        matlist.push_back(tmpnode);
    }
    return 1;
}

void Matter::furtherLexicalAnalysis(vector<MatNode> &matlist) {
    bool leaveloop = 1;
    int i = 0;
    int j = 0;
    int k = 0;
    while (1) {
        for (i = 0; i < (int) matlist.size(); i++)
            if (matlist[i].isNum == 1 && matlist[i].num < 0) {//  如果还存在括号
                leaveloop = 0;
                break;
            }
        if (leaveloop) break;
        leaveloop = 1;
        for (i = 0; i < (int) matlist.size(); i++) {//  追踪法得到一组可计算括号的位置(j, k)
            if (matlist[i].isNum == 1) {
                if (matlist[i].num == -7)
                    j = i;
                else if (matlist[i].num == -6) {
                    k = i;
                    break;
                }
            }
        }
        CalaBlock(matlist, j + 1, k - 1);
        matlist.erase(matlist.begin() + j + 1, matlist.begin() + k + 1);
    }
    CallastBlock(matlist);
}

bool Matter::CalaBlock(vector<MatNode> &matlist, int i, int j) {
    //  处理一个不含有括号的式子 [i, j]， 数据全部存入matlist[i - 1]
    int k = 0;
    int head = 1;
    int coef = 0;
    int is = i - 1;
    matlist[is].clear();
    if (matlist[i].isNum)                //  注意把前缀数字保留下来最后乘给整个元素整列 否则前缀为1 不影响结果
        head = matlist[i++].num;        //  只处理 matnode[is] 当然后缀只负责更新matnode[k]的数量
    //  开始循环处理 元素阵列的特征   A2 B C3 O ......， 处理完把数据扔到 matlist[i-1] 里面
    //  注意这里的A B C O 本身就是元素阵列 进行循环累加
    while (i <= j) {            //  预设两个matnode为一组 检查第二个是不是数字 否则一个matnode为一组
        coef = 1;
        if (i + 1 <= j && matlist[i + 1].isNum == 1)
            coef = matlist[i + 1].num;
        for (k = 0; k < (int) matlist[i].block.size(); k++) {    //  处理 matlist[i]
            matlist[i].block[k].amount *= (head * coef);        //  修正元素系数
            matlist[is].block.push_back(matlist[i].block[k]);    //  把这个Ele挂到阵列后面
        }
        i++;
        while (i <= j && matlist[i].isNum == 1)i++;                //  循环完成以后 matlist[i]是一个字母
    }
    matlist[is].compress();        //  处理完以后压缩一次元素阵列
    return 1;
}

bool Matter::CallastBlock(vector<MatNode> &matlist) {
    int i = 0;
    int j = (int) matlist.size() - 1;
    int k = 0;
    int head = 1;
    int coef = 0;
    MatNode tmp;
    if (matlist[i].isNum)
        head = matlist[i++].num;
    while (i <= j) {
        coef = 1;
        if (i + 1 <= j && matlist[i + 1].isNum == 1)
            coef = matlist[i + 1].num;
        for (k = 0; k < (int) matlist[i].block.size(); k++) {
            matlist[i].block[k].amount *= (head * coef);
            tmp.block.push_back(matlist[i].block[k]);
        }
        i++;
        while (i <= j && matlist[i].isNum == 1)i++;
    }
    tmp.compress();
    matlist.clear();
    matlist.push_back(tmp);
    return 1;
}

bool Matter::preProcess(string &_tar) {
    //  去空格 统一分隔符样式
    auto tar = StdUtil::replaceSubStr(
            StdUtil::replaceSubStr(
            StdUtil::replaceSubStr(
            StdUtil::replaceSubStr(
            StdUtil::replaceSubStr(
            StdUtil::deleteSubStr(
            _tar, " "),
            "·", "."),
            "{", "("),
            "[", "("),
            "]", ")"),
            "}", ")");
    int j = 0;
    for (j = 0; j < (int) tar.size(); j++) {
        if (!((tar[j] >= '0' && tar[j] <= '9') || (tar[j] >= 'A' && tar[j] <= 'Z') ||
              (tar[j] >= 'a' && tar[j] <= 'z') || (tar[j] == '.') || (tar[j] == '(') || (tar[j] == ')') ||
              (tar[j] == '@') || (tar[j] == '$')))
            return 0;
    }
    //  修饰并列符“.”
    for (int i = 0; i < (int) tar.length() - 1; i++) {
        if (tar[i] == '.') {
            tar.insert(tar.begin() + i + 1, '(');
            for (j = i + 1; j < (int) tar.length(); j++) {
                if (tar[j] == '.') {
                    tar.insert(tar.begin() + j, ')');
                    break;
                }
            }
            if (j == tar.length())tar.push_back(')');
        }
    }
    tar = StdUtil::deleteSubStr(tar, ".");
    //  去电荷标识符，规定，机内正电荷用@，负电荷用$表示，就近结合
    size_t pos = 0;
    string positive = "@";
    string negative = "$";
    q = 0;
    int cq = 0;
    int exp = 1;
    while (1) {
        pos = tar.find(positive);
        if (pos == string::npos)break;
        tar.erase(pos, positive.size());
        pos--;
        while (pos >= 0 && tar[pos] >= '0' && tar[pos] <= '9') {
            cq = cq + exp * (tar[pos] - '0');
            exp *= 10;
            tar.erase(pos--, 1);
        }
        if (cq == 0)cq = 1;
    }
    q += cq;
    cq = 0;
    exp = 1;
    while (1) {
        pos = tar.find(negative);
        if (pos == string::npos)break;
        tar.erase(pos, negative.size());
        pos--;
        while (pos >= 0 && tar[pos] >= '0' && tar[pos] <= '9') {
            cq = cq + exp * (tar[pos] - '0');
            exp *= 10;
            tar.erase(pos--, 1);
        }
        if (cq == 0)cq = 1;
    }
    q -= cq;
    _tar = StdUtil::deleteSubStr(StdUtil::deleteSubStr(tar, "@"), "$");
    return 1;
}

int Matter::getEleAmount(string &_element) {
    for (int i = 0; i < (int) eleList.size(); i++)
        if (_element == eleList[i].name)
            return eleList[i].amount;
    return 0;
}

int Matter::getEleIndex(string &_element) {
    for (int i = 0; i < (int) eleList.size(); i++)
        if (_element == eleList[i].name)
            return i;
    return -1;
}

string Matter::p() {
    string res = name + stateInfo;
    return res;
}

double Matter::getRelativeMolecureMass() {
    int i = 0;
    double res = 0;
    for (i = 0; i < (int) eleList.size(); i++)
        res += eleList[i].amount * Get_relative_atomic_mass(eleList[i].name);
    return res;
}
