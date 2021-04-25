#include "ui/main_tab_widget.h"
#include "application.hpp"
#include "chem/jmol.hpp"
#include "chem/jmol_adapter.hpp"
#include <iostream>

int testMolFormat() {
    std::vector<std::string> formats = GetWritableFormats();
    std::vector<std::string> suffixVec;
    for (auto &format:formats) {
        if (std::string::npos != format.find("Read-only")) { continue; }
        suffixVec.push_back(format.substr(0, format.find(" ")));
        std::cout << "\"" << suffixVec.back() << "\",";
    }
    std::cout << std::endl;
    std::shared_ptr<xgd::JMol> mol = std::make_shared<xgd::JMolAdapter>();
    mol->readAsSMI("c1ccccc1");
    std::cout << "**********************" << std::endl;
    for (auto &format:suffixVec) {
        std::cout << format << std::endl;
        std::string content = mol->writeAs(format);
        if (content.length() > 10000) { content = "error"; }
        std::cout << content << std::endl;
        std::cout << "**********************" << std::endl;
    }
    return 0;
}

#include <QDebug>

int testReg() {
    QString fuck = "CCl3";
    QRegExp reg("[CONPS][(Br)(Cl)(F)(I)]");
    qDebug() << fuck;
    qDebug() << fuck.replace(reg, ".");
    return 0;
}

#include "ocr/ocr_runnable.hpp"
#include <QDir>

int loopBenchMark() {
#ifdef Q_OS_LINUX
    QString root("/home/xgd/source/repos/leafxy/testcase/public/");
#elif defined(Q_OS_WIN)
    QString root("C:/Users/xgd/source/repos/leafxy/testcase/public/");
#endif
    auto clef = QDir(root + "CLEF").entryInfoList(QDir::Filter(QDir::Files));
    auto jpo = QDir(root + "JPO").entryInfoList(QDir::Filter(QDir::Files));
    auto uob = QDir(root + "UOB").entryInfoList(QDir::Filter(QDir::Files));
    auto uspto = QDir(root + "USPTO").entryInfoList(QDir::Filter(QDir::Files));
    OCRThread ocrThread;
    std::unordered_set<std::string> superAtomSet;
    auto loop = [&](const QString &_filePath) {
        QImage image;
        if (image.load(_filePath)) {
            image = image.convertToFormat(QImage::Format_Grayscale8);
            ocrThread.bindData(image);
            ocrThread.start();
            ocrThread.wait();
            auto mol = ocrThread.getMol();
            if (mol) {
                mol->loopAtomVec([&](xgd::JAtom &atom) {
                    if (xgd::ElementType::SA == atom.getType()) {
                        superAtomSet.insert(atom.getName());
//                        qDebug() << atom.getQName();
                    }
                });
            }
        }
    };
    for (auto &file:clef) { loop(file.absoluteFilePath()); }
    for (auto &file:jpo) { loop(file.absoluteFilePath()); }
    for (auto &file:uob) { loop(file.absoluteFilePath()); }
    for (auto &file:uspto) { loop(file.absoluteFilePath()); }
    qDebug() << "**************";
    for (auto &sa:superAtomSet) {
        qDebug() << sa.c_str();
    }
    return 0;
}

int handleSuperAtom() {
    std::vector<std::string> sa = {"(IIa)", "(III)", "SPh", "OCCL3", "N(", "MeS", "OPh", "O--Ph", "(CH2)16Me", "PhS",
                                   "SMe", "LS4", "9O", "TS", "tranS-", "raC-", "N(H)Me", "Me(H)N", "OHC", "BnO", "n-BU",
                                   "CF2CF3", "COMPOUndA", "-CO-CH-", "-CH-CH2-", "CH2-CH2-CH3", "i-i-iii-=inW=i=iii",
                                   "GH25PdC3EtOH", "tt-eZiti2", "eagentSandCOnditiOnSi", "=ii=iiNgiiiYi=---",
                                   "MP5OnthenPTSOHEtOAC", "InterMediate86", "InterMediate9L", "InterMediate9O",
                                   "-COMPOUndOfFOrMULaI", "InterMediate82", "COMPOUndOfFOMMULaI", "InterMediate8O",
                                   "COMPOUndSS", "NCH3", "COMPOUndS3", "SteP7d", "StePA2", "StePAL", "S-NH2", "PhHN",
                                   "da", "NX", "CL3", "O3", "C2", "C9", "1S52R-aCid", "D-", "LLe", "L-Orn",
                                   "C18H37HNOC-", "MeOH2C", "--CCH2-C", "TO", "H-TIeNMe2HOBtEDC3DMF3", "tF3", "32",
                                   "j--CH2-G", "T--CH2-G", "-CCH2", "O-H2C", "I37C18--O--P", "PH3C-C-", "PH3C-C-Ph",
                                   "Ph--C-C-", "L3S", "I3O", "L29", "L28", "LO8", "LO7", "LO4", "LO2", "67", "66", "6S",
                                   "HHCL", "A8", "A7", "46", "36", "3S", "CH3CONH", "2L", "4-T3", "Z4", "ZS",
                                   "O-CO-NH-", "26", "Fi", "S#O", "Trt", "Nj", "d-Thr", "Ii", "Ld", "eTFA", "O5",
                                   "SO3HHN", "TFA+H2N", "Ph2P", "H7C3", "H23C11", "IZi", "XCF3COOH", "N(H)Et", "H3C-S",
                                   "(CH2)11Me", "-N=N-=N-", "-N=N=N", "N=Nt=N", "OTf", "PMBN", "COMPOUndZ7",
                                   "COMPOUndZ4", "COMPOUndZ2", "COMPOUndL7", "COMPOUndLS", "COMPOUndL3", "COMPOUndL2",
                                   "89", "COMPOUndL4", "COMPOUnd7a", "InterMediate93", "COMPOUnd7", "COMPOUnd6",
                                   "COMPOUndS", "COMPOUnd4a", "COMPOUnd3", "S=O)", "COMPOUnd2", "COMPOUndLa", "OMS",
                                   "Nat", "StrUCtUreI", "OCH2Ph", "FMOCHN", "AA", "SO3", "OSO3", "CH2OSO3", "-CO-NH-N",
                                   "C10H21-O-", "O-C10H21", "GeLZa", "GeLLC", "GeLLa", "EXaMPIe3", "F-YYtiZ=+",
                                   "TriS-HET", "Xi4e-ir", "FW19423", "FW24S32", "N2", "GeL3a", "V--N", "O--N", "V1L",
                                   "jZ", "EtOOC", "NHPh", "7O", "aSO", "H2CO", "-NH-CCH23-OCH3", "EMS", "EM4",
                                   "D-aHLLe", "BQ-3", "COnneCtingUnit2", "COnneCtingUnitL", "T", "Iigand(VI-S)Ph2P",
                                   "CO2C(CH3)3", "SiStUCtUre0ftheLigan", "PPh2", "F5S", "FSS", "Bn2N",
                                   "MetiLPLUViatOLide", "D-PrO", "OBZ", "OBOC", "(CH2)6NHCOCH(CH2", "CheMiCaLFOrMULaI",
                                   "OCH(CH2)3N-t", "3N-t-BOC", "tBU", "-S-N-S-", "F3C-S-", "N--(CH2)2-NH-(", "N-NH2",
                                   "CH3-N", "NH--NH2", "-(CH22-SO3H", "C-NH-CCH2)2-", "I-C=N-(", "=N-(CH2)3-CH3",
                                   "-NH-C=", "-NH-CCH2)2-N", "-NH-CCH2)2-", "-NH-C-", "-CO-CH2-", "-G-fj-", "-tN-S-",
                                   "FOrMULaZA", "FOrMULaLA", ")-C-", "ACO", "OAC", "Lene-naPhthaLeneStrUCtUre",
                                   "tFW2453i", "EYEIt", "PYRIM", "I3SJ", "I33I", "I3HJ", "I3OJ", "I28I", "I26I",
                                   "TPaO7", "I2SI", "I2LJ", "(R)", "L-Thr", "HN-S", "HN-S-", "NH-S", "Nj--S", "N--S",
                                   "N-S-", "K-t-BO", "Et-TFMS5", "THF", "COMPOUndL", "COMPOUndS8", "H3CHN", "HCL",
                                   "EtO2C", "CO2CSPr", "C2H2O4", "-S", "5S", "FOrMULaI", "(2A)", "C-N-", "OCH2CN",
                                   "N--S-", "NHNHCOCF3", "InteMMediate8S", "NHNHCOCH3", "H3CS", "H3CO2S", "(IId)",
                                   "(Ib)", "OCH2CHOHCH2NE", "N(SO2CH3)2", "OCH2CHOHCH2OE", "(XXXI)", "(XVIID)", "(XI)",
                                   "(XD)", "t-", "(VIII)", "(V)", "H2N-CH-C", "C=O", "O(H)", "-C-OH", "(CH2)5", "(XII)",
                                   "256-diiOdO-A-nitrOaniLine", "256-dibrOMO-4-nitrOaniLin", "EN-tteNMe2HUBEEUnU",
                                   "4-BrOMO-Z-nitrOPhenOL", "InteMMediate88", "MeOOC", "2456-TriiOdO-PhenOL", "LabeL3",
                                   ")05-5CH2-", "(Z9)", "-6=5-5CH2-", "(Z7)", "JO4S", "-tCH2-", "-tCH2", "-j6i-tCH2-",
                                   "(Z3)", "FOMMULaI", "C8H13O", "-C-Ph", "-CH-C8H13", "HOOC-C-", "F-CCH2)4-", "R1=",
                                   "LS2", "t-BU", "L49", "Hi", "LSO", "L7", "IIg", "I3CO2OONH2CH2C", "-OCH2(", "O=P",
                                   "C2H5O", "H4NO3S", "-OCH2CH2OCH3", "(H4NO)2-P", "-SO3NH4", "-NH-", "-CH2-CO2-NH-CH",
                                   "-CO2-CH2-CH2", "-NH-CH-CH7-", "-CH-CH2-CH3", "-SO2(CH2)3SO2NHCH2CHCH",
                                   "--SO2CH2CH2CH2SO2D", "CH2CH2CH2SO2NH-CH-C", "-CH-CH2-OE", "Z-Dhb", "f-L", "NO2NC",
                                   "(CH2)3", "SO2NH", "-CH2-N-", "-(CH2)4-TT-N-", "SO2NH-", "NHCOCL", "-tCH25OCH2CF",
                                   "C4H9", "SO2NHCH2CH", "-tH2C-HCj", "-O-(C2H4O)8E", ")-CH2-(CF2)6H", "COOCH2Ph",
                                   "-tH2C-CHj", "COMPOUnd76", "-tH2C-HC-Z", "_04", "-O-CC2H4O)4CH3", "OO", "tEHK(3)",
                                   "CHCLH", "#C", "-IggEt(C)", "COOCH3", "C6H11", "GH", "Ot",
                                   "ReagentSandCOnditiOnSi(e)NMP3", "=L-10-54=-7", "(=)", "tJLti-EB1t#1", "iZj",
                                   "CH2CO2H", "GeLLb", "X4", "333=t(4)", "iiZiif-5j3j7", "(HTM-O)", "9",
                                   "j7==)X)VXKV5NZt)Lt", "BQ-L", "C27H25CLFN5O5", "-O-C-", "(H)", "-O-CH5f", "-NH",
                                   "(-)", "-Oi-tCH2-", "TPD", "(CT-I)", "---(1)", "NMe", "i-CH2CH2CH2C",
                                   "C22H17CLFN5O5", "Ci", "CH=CHCH", "-KiJjtf-XZjI", "-N=Nt=N-", "iC", "O--",
                                   "EtEqBHtEgXK", "MOLWtSS397", "(37)", "911-jX5jL-", "N-O-C-CH3", "H3COCH2", "CH2CH2-",
                                   "ItEgEt(3)", "H3Ca", "333343A+-i7==)LX)Et", "CH=CH-CH=C-", "(CLD-7S)",
                                   "rjJaINiSXXKn1A", "(1)", "(r)", "-CH2-CH=C3", "(I5)", "eCH3", "-N2", "(",
                                   "I-(3q-diChIOPhenD-3-(Z3S68-heXahdre-IEi-(", "I2C=CHCOO-", "IL9I", "N-Ra", "C6",
                                   "(22)", "EPinePhrine", "(I6)", "C=HC-HC=C", "Phen-3", "(6B)", "(=t15)", "(Ibq)",
                                   "(2)", "CFt", "RZ", "HbN", "iggigEt(1O1)", "COOMe", "-CH-CH2", "CH3-C=", "=S=O",
                                   "32T", "(I-S)", "(B-3)", "(HTM-Z)", "(IO)", "(I-Z-q)", "Rd", "NHBOC", "tn3Et(6)",
                                   "NPDA", "ItEitJ", "=F-X=PjFZ)j==JXEP", "-CH2-(CF2)6H", "EX(5-PS)XXrtt)-",
                                   "C=CH-CH=CI", "(i-78)", "C-CH3", "OCH-C6H10", "L4", "(89)", "F3CH2CO",
                                   "EH-CeIqPentaIBIqUneIn-S-DUre", "1L", "3Q-(34-9S)S7==)L)XJI", "L-VaL", "----(Z8)",
                                   "28", ")=S", "USCharin", "K-i7Z=)LX)XrV5rjJJLH", "(CT-2)", "GLUtaMine", "#-#-#-#",
                                   "(iV)", "HS", "I7JLrL--8", "ZX)LtV5rjj0)LfiVEEqfH7KqJ", "R11", "Zr-", "YNN",
                                   "(1-435)", "-(A5)", "TBAF", "N-O-C-", "BU", "(H3C)2HC", "-JZgFiXAS12", "H1SC7",
                                   "COOEt", "CHa", "MWaeO", "(H7C3)2N", "InterMediate8L", "H3", "#", "CH3CCH3",
                                   "CH-CH2", "P_", "-j5jLXJLX4ZXt=tj", "C-O-", "NN", "9i0ai4", "T)", "H2C", "C2HS",
                                   "KtfigEt4C)", "CF2O", "(AZ)", "CH=CH", "64", "H3CQ", "LSL", "SO", "(24)", "d2N",
                                   "(1O)", "IHN", "(CTP-8)", "R18", "--g", "H7N", ")j7E=)L)XZII5Z-di55q-d", "S--N", "E",
                                   "N(CH2CH2CH3)2", "1Xq152-di54-djEXX4j", "C--N", "H2N-C", "CH3-O", "IAL", "P1",
                                   "InteMMediate78", "CL3C", "CL2", "(VI)", "(L", "F-", "NHR7", "i=", "q1t243I", "--",
                                   "-", "23B-(33g-iTSJj75=))X", "t)5", "(A6)", "tEgjEEt(1O)", "(1_)", "--N",
                                   "(CH2)9CH3", "--E-", "(B)", "-O", "COOH", "COMPOUndZO", "Z1", "nHEnr", "ItYII", "_r",
                                   "S=(", "(T)", "-SO2NH-", "R+", "Z7==)L)XZJII3Z-diA55-", "Rq", "(ZS)", "H8N", "RG",
                                   "3=(", "IEgHJNa18", "Ph--N", "(C)", "OCH2OC2H5", "EXaCtNiaSS49SiS", "n", "OHC--N",
                                   "MOLWti48191", "H3CO2C-", "q", "CL-T", "n=8S", "SO2NHCH2CF", "YP", "COMPOUndL9",
                                   "S#U", "Ejtq", "CFL", "H3C-O", "CH2OCH3", "iigiXEg=4=", "BNDit-j5jLXJLr4Zi", "H8C",
                                   "AE", "(B-Z)", "-arL", "CH3-f", "CH8", "XX", "(I)", "(ZO)", "i=C)", "39", "L27",
                                   "N=C", "(IX)", "1)", "-CH2-CH2-", "HCI", "(I-I)", "HI2CHj5-CH3", "CH(CH3)2", "22",
                                   "t2J", "PZ7-Z-(M-9S775=)V)X", "(B-q)", "-XZjIbjZ3E", "SO9", "CHHCH", "(Z)", "OL",
                                   "(P-9S77==JL)XVJXKtfJ-)L", "InterMediate84", "C-O-CH2", "RIO", "(A)",
                                   "iPi1r0r1r4N4hSn21i4hia4hMW155Hid", "B-Pi)-2-(M-PSJ7==)E)V", "--tCH2", "N(C2H5)2",
                                   "RL2", "-C-CH3", "C=C", "C-NH7", "(M-)S77==)L)XVXKt)-)", "Nt", "XS", "O--OC",
                                   "MOLWtiS6800", "H3CH2C", "InterMediate94", "H2C=CH-", "(IZD)", "LO", "5-", "P5T",
                                   "49", "L-R3", "CO2Et", "-CH=N-N", "NBOC", "(9)", "CF6", "CO2H", "R21", "(1-2-1)",
                                   "A--j", "COMPOUnd4", "J=N", "R10", "7", "4-itHtKi7I=nLI-F1", "NHSO3", "-A-R-NH2",
                                   "dEEEtT", "I-", "HO3S", "NHOH", "O-R1a", "InterMediate77", "JL", "Z-", "CH=CH-C-",
                                   "CF3", "h", "BOCHN", "COMPOUndZ3", "C2H5", "-fBEE(3)", "(1-267)", "--C-", "HN",
                                   "CH2-CH2-", "EnXUKEqtqEZKq", "P-CL", "NOMe", "RLL", "RR", "G-(A3A-iPS)Z7Z=)L)V", "2",
                                   "-O2C", "CN", "RO2C", "77==))XI12-diA3f", "XHCL", "L48", "-NHCH2COOH", "H2N-N-",
                                   "-NCCH2CH2CH2CH3", "--CH2-N", "(86)", "MS(MHH)SI7", "(1-21O)", "MeN", "X1", "RLO",
                                   "iggiEEt(2)", "L--(R3)P", "CH2-F", "O2S", "LtFit=tJJI-h", "QP13", "Et(VID", "C-O-CH",
                                   "CF", "CH3CH2", "CH5", "H3C-N", "HgC", "R4", "C(CH3)3", "-P-OH", "CH=CH2", "GH3",
                                   "6", "OSO2CH3", "H3C-N-", "(Iaq)", "O2", "VF", "ChireL", "OQ", "St", "(IS)",
                                   "CH2CO2CH3", "O--S-", "OCH2CF3", "1=S=O", "RII", "RX", "RL", "SO2N", "C8H17",
                                   "EXaCtMaSSi48SOS", "iX", "(CH2)S", "C=C-C=C", "RC", "CC", "H2N", "#N", "C-CL", "NH2",
                                   "EXaMPLeS", "EXaCtMaSS48113", "CateChin", "(1", "OCH3", "-N-CH3", "5ZgiX72",
                                   "H2C-tCH2CH2CH2C", "-N-CH-C-", "JV", "MeO", "I36I", "-O-CHi", "CH=CH-CH=CH", "C3H7",
                                   "VH2", "X3", "NH-NH2", "(TO)", "SO3H", "I=N", "C-CH2j-CH", "JII5Z-di455-dIEXXKtf",
                                   "(g)", "aIKL", "LO6", "GH6", "Et(3)", "(H-7)", "R12", "a3O", "(IIB)", "MOLWtiS2S92",
                                   "EeXUNEqfFE)KHJ", "CH2OMe", ")", "CH3OCH2", "CH2-CH2", "--O-C-", "H3Ce",
                                   "-F5T--OR11", "3", "O=S=O", "O-N", "LXVII", "H3COH2C", "CH3", "LCH2T", "I2TT",
                                   "(10I)", "=Z", "eC2H5", "CCCt", "Ar2", "(S)", "Br6", "CHO", "itIII", "R17", "Oh",
                                   "(7)", "R_", "CaH5", "OEt", "HCLSaLt", "S-R1a", "PCH3", "H5N", "rJAtiaXS12", "NHRS",
                                   "R8b", "S--OH", "nnn(4)", "SH", "SCH2CH2=C", "OMe", "SF2", "n2Na", "HO-H2C-CH2-C",
                                   "R1A", "H-N", "M0LWt-S4196", "--Si", "R5", "C-L-G-L-", "C8", "HOOC", "I)", "OC2H5",
                                   "ZnOXO-VOrUSCharin", "NHAC", "NHCH3", "N=N", "CH=C", "XNH2", "H2=C-tCH2CH2CH2CH5",
                                   "NHR", "CLCH2CH2C", "Z2-EXI4-(34-", "L", "NC", "Pi", "H2C=C", "AF", "M",
                                   "COMPOUndZ6", "MS(MHH)4S8", "8", "6-(434-jPXi7==JL)X-jI3", "OH1S", "n2", "d",
                                   "jH-CH", "OMePraZOLe", "IKEitJN08", "(1-B)", "O2C", "N-", "O-", "Nj--S-", "Z", "CFi",
                                   "Phen-4", "RqR3", "CH3CO", "CONH2", "(IZC)", "t", "+-", "CHiO", "RS", "(qZ)", "D",
                                   "(IV)", "gr", "H4NOOC", "X", "COOC2H5", "MS", "=C-CH3", "S=()", "Si-", "J---CH2-",
                                   "(VII)", "Nh", "I2C-N", "HH", "CDZH", "SO2Me", "(III", "Eatgi-T", "N#", "SO2", "A",
                                   "CH2(", "-CH2-CH2-OH", "4", "H3COOC", "MS497(MtH)", "Rt", "NO2", "23", "t#", "R2",
                                   "R8a", "rJaaiiSXij1d", "AMPA", "(CH3)3C-", "COMPOUnd2L", "-C-NH-N=CH", "Ri", "HO2C",
                                   "-CH=CH-", "HOCH2-CONH-", "1fXLXX(1)", "HO", "R9", "H3C", "N--i", "C-CH2CH3", "Z-Ar",
                                   "N-N", "(55)", "(Z-i)", "IFOrMULaLL", "OC", "RI", "O-CH3", "--O2C", "COCOOCH2CH3",
                                   "Y10", "Nr", "IKtNar", "-C-OR11", "10H", "(IIf)", "O-OH", "(X)", "HC=HC", "R=",
                                   "S-R1A", "C-CH2CL", "L-Phe", "C-N", "XZ", "CH2COOC3H", "5-9S)-2-(P-PS7==L)XJt", "CS",
                                   "C1-C6aIKI-", "C-CH2CH2CH3", "R", "FXK-77a", "FOrMULaII", "O2N", "ZO", "G", "SbF6-",
                                   "NTS", "3333434P-Z7==)LX)LjKi", "NCCH2CH3)2", "t-BUi", "iiit", "CH2C", "(IIU)",
                                   "CH2=CHCO2CH2CH2O-", "f", "H8", "Wnn(2)", "NH-", "-N-S-", "L3", "+", "EE(8)",
                                   "--CH2-", "I3", "XA12", "S6", "Pf", "MeNH", "C1-C6aIKI", "-C=CH2", "rSiiZiX35i7",
                                   "-CH=CH-CH=CH", "-NUXj)L-IO-FXKV", "CHI", "O=S", "R2O", "(33)", "-CH7-CH=CH-CH=",
                                   "C-CH2-N", "I2--=tENE-", "a", "e", "C5H11", "EXaMPIeTO", "", "LX=Kjj_)LtVEqgEK7KAJ",
                                   "EtO", "ETML", "CH2CH=C-", "H2Nt", "RI+", "QgPij", "H3C-Si", "NHNH2", "CONH", "NC-N",
                                   "H2CH5-CH3", "S=O", "(ET-q)", "TfO", "EtHN", "H2C-CH", "LH", "(H", "SIOLa", "(11)",
                                   "EtEBfKEtgC", "N-CH3", "AD", "IRGACURE9O7", "HSN", "S-CH3", "d)n", "MS(MHH)i42O",
                                   "4O8)", "tHN", "CO2Me", "CLO", "OH", "EaMPLe336", "N-RZ", "C24H21CLFN5O5", "R)",
                                   "CKCC", "RZZ", "(5)", "CL-C", "(28)", "CF2O--", "OCF3", "OC2I", "CH", "(a)", "O-CH",
                                   "I2IJ", "EXaCtMaSSS2SI2", "OC2", "2H5", "ii", ")C", "M01Wt49591", "RZO",
                                   "-O-(C3H6O)7CH3", "g", "LI)", "(16)", "H3tCHCH2CH2CH2j5G", "CCL3", "2)", "fKAfg(2)",
                                   "-C-H", "(1I7)", "C-", "2CH2", "HNOC", "-UU-VV-WW-R_", "CH2OCH", "=", "IO",
                                   "-j7==))X)II3Z-diA3g-q", "CO2(", "(I-b)", "CH2CH2-O-C-HC3CH", "(C-4)", "(III-I)",
                                   "PCAPA", "F3C", "(3a)", "HB", "CBP", "N5", "IEgfJ(2)", "n2(37)", "-CH=N-Z",
                                   "-CH2CH2CH2-O-C", "(XXX)", "FOrMULaA", "(3C)", "C26H25CLFN5O5", "HC", "(N)", "UV-3",
                                   "fKetJ1", "CfK8Z", "CK", "R20", "P-H", "(EEtqFEEqHKC)", "(CH2)10", "IK#EEt12i",
                                   "(3I)", "I2CH2-C", "CH3CH", "=CH-CH=C", "MIT", "(Z-A)", "B-rJ-S-P-NJn9E", "-O-CH-C",
                                   "(iij)", "-eBJL-3-rJL-eg-P-", "CHCH2CH2CH2-N", "L8", "ZrKt-)4)", "(CH2CH2CH2C",
                                   "7r=LVEX(5-9S)", "CH3--C", "CHt5CH3", "-tH2C-HC", "(b)", "S-MeHeX", "COO",
                                   "Z)X==nJjUneVEggEZKf", "C=C-", "(d)", "(CH3CH2)2N", "-CH2-OH", "(1O3)", "C6H13",
                                   "4-it)LHRj-j=VjF-)2=", "PPh3", "Ib", "-(CH2)4-", "EXaCtMaSSiSS3IS", "12-NU9tf-XZ-",
                                   "M01Wt-49593", "-CH2-CH2-CH2-C", "iJ", "-tH2C-HC-", "TZ", "CO7H", "3=()",
                                   "-(CH2)2-CCF2)4F", "NO", "NO5", "L9", "O=S-", "i", "ETM2", "BPhen", "H3N", "EtO2CHN",
                                   "-CH2-", "-CH=C-", "NH5", "CLH", "D-VaL", "CHH", "Me", "ZH2-CH2-C", "NH3", "R7",
                                   "CHCH", "StatinLCOrreCtedStrUCtUre", "COOK", "H3CO", "S=C3", "OCH2CH2OH", "IH",
                                   "C25H23CLFN5O3", "BrH", "P-OH", "LHO", "TFA9H2N", "NH2BrH", "iNN", "-O-CH2CH2O--I",
                                   "H9N", "H2-CH=CH", "-CH-tCH2j5-CH3", "I29J", "dP", "-CH-CH2-CH2-C", "H=CH-CH=C-",
                                   "C3", "I34J", "5CH2CH2CH2CH", "Yi", "VH5", "(CH2)q", "H3CH2CH2-G",
                                   "26-DibrOMO-4-nitrOPhenOL", "Nt--O-", "NH2Q", "1F", "S-", "er", "CCH2-CH2-", "(23)",
                                   "C27H26CIN5O5", "4--", "=-CH2-CH2-CH2-C", "5S=O", "i--5a8", "(Y", "-OCH-C6H13",
                                   "NO-", "-C=i-", "I22L", "NO3", "SS", "CLNH", "(CGM", "Ch", "-CO2-CH2-CH",
                                   "CLCH2CH2CH2-", "NH4", "V9", "dr", "1", "FOMMULa62", "L6", "Q", "LS", "OTOS",
                                   "EIK4J", "CH3-CH2-", "O-CH2-CH3", "SandCOnditiOnS(e)NMP39O_C3I9", "3K", "CLL",
                                   "SO2CL", "C22H19CLFN5O3", "EXaMPLeI3", "OCH-C6H13", "R=--C", "FOMMULa6L",
                                   "H2C=CHCH2CH2", "H3C-C", "COMPOUndZS", "H3CH2C-N", "OR1", "-CH-CH-", "EML", "CO2CH3",
                                   "(I2B)", "-N-CH2CH3", "CH-CH3", "COMPOUndL8", "N-CH2CH3", "(CH3CH2)2N-",
                                   "2NHCH2CH2CH2O-CH", "XdiiitXiZ7i0", "CH2CH2", "CH2CH", "R1", "CH-CH2-CH2-C",
                                   "-CH2CH2CH2CH2-", "I3TT", "(GH2)M", "H2C-H2C", "(3)", "CH2CH=C-t", "CH3CH2)2N-",
                                   "(a-g)", "i-tCH2j5CH", "I=C-tCH2CH2CH2CHt5", "C=HC", "CH2CH2CH2CH2", "H3tCH2ji0f",
                                   "CH2CH2CH2CH2-", "-N(CH2CH3)2", "-CH-", "C=CH", "#IEqL25", "H=C",
                                   "SPII53Z-did55-dJEXZ3jf", "CH=N", "OS", "(C2H5)2N-", "CH=CH-", "1(tX)#gn5#JLgd",
                                   "N-BOC", "-CH2-CH2", "NHRI", "(H5C2)2N", "C-CH2CH2CH", "H3C-CH", "C=CH-CHO",
                                   "-CH2CH2CH2CH2CHi", "benZL-S5S-diMethL-OXaZdidin-2-One3", "SCF3", "-CH2CH2CH2CH",
                                   "27", "CH3-C-", "CH3-C", "CHCH2CH2", "C-CH2CH2CL", "ZCH2CH=CHCH3", "I1j",
                                   "=C-CH2CH2CH", "38", "=CH-CH2-CH=", "=CHCH2", "FO2Me", "MeO2C", "-CH2CH2CH2SCH3",
                                   "CHCHCH2CH2=C", "(IIS)", "5JZiiS#XSK_14", "H=C-tCH2CH2CH2CH5", "rZSiiZiXdRiZiK",
                                   "FOrMULaIII", "H2CHT--CH3", "LCH2ji-CH3", "H5C2", "-CH2--CH-", "C-OH", "NMP-An",
                                   "(rNY", "a2", "S4", "FOMMULa6O", "SCH2j-OH", "O4S", "(4)", "M01Wt4S784", "-C=CHCH2-",
                                   "CH2OH", "R6", "H-HCL", "(I4)", "C-CH2C", "CHCH2CH3", ")#S", "CH2CH2C",
                                   "I=CH-CH=CH7", "C--CH2-", "N(CH3)2", "i#(", "-fH-tCH3ji-CH3", "-CONH--C",
                                   "-C-rCH2CH2CH2CHT", "COMPOUndSZ", "NHSO2BU", "InterMediate7L", "N-C2H5", "63",
                                   "CH2-CH=C-", "CCH3", "Ar1", "F-CH2j=-CH3", "V=N", "C-CH2CH2CH2", "iZ_i=ii=Z4",
                                   "NCH2CH2CH2CHt", "iHt5CH3", "ER-8O7S46", "NHC18H37", "N(CH2CH3)2", "CHT5CH3",
                                   "H3C3tHq", "LOO", "EXaCtMaSSi4SSI2", "Et2N", "StePLb", "5)", "EXeMPLifiedCOMPOUnd",
                                   "Ari", "NOO-6-6", "CTMR-E", "nn(16)", "NOO-7-I8", "N-S", "CH3CH2-C", "BOC2N",
                                   "NOO-8-23", "COMPOUnd4b", "nOnOhdraZOne2", "11j", "COCH3", "-CH=N-", "COCH",
                                   "JZqiSXXXS1", "H2C=CH-CH2", "LO3", "H5C2O", "(H3C)2N", "(F-3)", "HO-CH2-C", "HO-H2C",
                                   "ID", "Z=CH-CH2-C", "H3C-C=", "NH3CL", "-C=CHCH2", "CF2CF2H", "H7C=HC-C", "FF",
                                   "-C=CH-CH7", "CH2=CH-CH2", "1=N", "CH2-CH=CH", "-CONH-CH2OH", "-CH3-CH3-CH2",
                                   "SO2NH-tCH2j5", "C26H24CIN5O5", "5CH2-CH2", "S--O", "CF3CF2CF2CF2SO3", "C9H19",
                                   "FOrMULaL", "Et", "S=C)", "(I3)", "CheMiCaLfOrMULa", "-6i-CH2-", "OK", "NHCO", "bQ",
                                   "fbjZ7EXZ3", "(IA)", "EXaCtMaSSA4L1a", "EXaCtMaSSiS6717", "HOH2C", "(ZA)",
                                   "COMPOUndL6", "(3A)", "-ZX)L-IO-5", "(IZ)", "(I7)", "(I8)", "CH7", "(Z8)", "TMS",
                                   "(Z2)", "O-R1A", "S#(", "NHCOOEt", "(2I)", "(2S)", ")=S=O", "O=S=", "OCH--O-", "L2",
                                   "r", "(8)", "HOCC", "2Z", "-N", "C22H18CLN5O5", "=C", "2CF3CO2H", "S#()", "CH2CH=C",
                                   "Ph", "#(", "M01Wt50793", "S=C", "S=C2", "aZ", "C26H23CLFN5O5", "--CH2", "i=(",
                                   "EXaMPLeLI4", "iiiiEZX3-", "SO2NH2", ")#=S", "i=C", "fOrMULaII", "M01WtS3S98", "b",
                                   "iS=O", "ISO7NH-CH-CH7-", "CHia", "igjEEt(A)", "X#(", "LOS", "3=C", "CH2CH2CH2CH",
                                   "IL", "S#C)", "OCF2H", "NHSO2Me", "33", "C24H2iCLFN5O3", "C21H17CLFN5O3", "2CHCH2C",
                                   "M0LWt44184", "M0LWt345587", "CH3O", "eXidiijiXS48ii3", "M01Wt348191",
                                   "CH2CH2C(O)OCH2CH3", "HIN", "i-CH2-CH2-", "iZiWF-5i9j6", "C-CH2CH2C",
                                   "rJZqiFStESZSA", "M01Wf52800", "rZX4iiiSSiWXji27", "tfK9J", "M01Wt5S3598",
                                   "qCH2CO2Et", "iK", "tJZIiSXdXZKn1A", "N=N-", "OW", "4334(28", "(C2H5)2N", "PCH2CO2H",
                                   "C25H22CIN5O5", "Cj", "OCH2CO2H", "FH2COOC3H5", "HLe", "OCH2CO2Et", "eCH2CO2H",
                                   "PCH2CO2Et", "M01WtSS001", "IL8I", "CFZ", "EXaaiNgSSS49i18", "NH6", "C28H28CIN5O5",
                                   "=iZ-J--iii#-=tEi-", "N01Wf52i95", "75=)V)XXjKtrj-JL", "H3C-HC", "Z9",
                                   "HCH2CH2CH2CJ", "3ZiiqSS)i15", "-iiViiiiiKZEaS", "rSaiiXXXdSZi5", "M01WtSS397",
                                   "H11C5", "iiZiif-5j5O3", "rWdiiiStXSi3i7", "IHIN", "M01Wt5S1391", "MOLWt348S85",
                                   "GO2Me", "BOC", "O=Si", "EXaCtMaSSiS4LIS", "MOLWtiS1391", "EXaCtMaSSiSI312",
                                   "N-CH2-", "Kt", "X=C", "ER-8O6286", "(1-269)", "C27H25CLFNSO5", "Z2",
                                   "C25H2iCLFN5O5", "(XIII)", "I2C-CH3", "C28H27CLFN5O5", "MOLWtiS3994",
                                   "EXaCtMaSSiS3914", "NHCbZ", "C29H29CLFN5O5", "M0LWtiS82O2", "EXaCtMaSSiS8118",
                                   "MOLWtiSS397", "EXaCtMaSSSS3IS", "Me2N", "Et(H)N", "MeHN", "AZ=", "NH", "EXaMPIe399",
                                   "HO-C-", "EXaMPIeS93", "NHSO2CH3", "(IC)", "iH2-CH=CH", "C24H21CIFN5O3", "(ID)",
                                   "(II)", "Lit", "(BOe)2N", "Bn", "AH2", "BnN", "-j6=5-tCH2-", "3N", "C7", "LOL",
                                   "Ure0ftheIgand(IV)4Ph2P", "F3CO", "M01Wt46786", "(HTM-e)", "H-", "HO3S-",
                                   "eCH2CO2Et", "++", "1H3N", "HI", "-(CH2)4O-", "AB", "(IIn)", "CH2", "(IIP)",
                                   "MeO2SO", "(IIr)", "OC2HS", "CH2-CH3", "N-OH", "3O", "4O", "A9", "-CO2H", "+TFA",
                                   "(XTV)", "S7", "(I9)", "6O", "SCH2i6OH", "ER-8O6O9S", "2HCL", "ZHCL", "(HTM-)",
                                   "OO3", "a66", "(12)", "NMe2", "SCH3", "AH", "SOCH3", "H2", "ER-8O6287", "N2BF4",
                                   "CO2", "2O", "NW", "(SC)", "ChiraL", "CF2", "H-S-", "q12-di435-dJEXZrKtt-",
                                   "-C-(CF2)8H", "CH3CH2CH2C", "-C-(CF2)6H", "-SO2CH2CH2CH2SO2NHCH2", "-CH2-CCF2)8H",
                                   "InteMMediate83", "O-(C3H6O)13H", "CH2CH3", "O-(C2H4O)2E"};
    std::sort(sa.begin(), sa.end(), [](const std::string &a, const std::string &b) -> bool {
        if (a.length() == b.length()) { return a < b; }
        return a.length() < b.length();
    });
    for (auto &s:sa) {
        std::cout << s << "\",\"";
    }
    return 0;
}

/**
 * FIXME: 结构综合失败的 case 总结：
 * 【不解 bug】超原子一段有键，没有生成这个键
 * 【不解 bug】无意义超原子干扰视线
 * 【必解 bug】超原子多端接入
 * 【必解 bug】芳环中的圈
 * 【必解 bug】上下邻接的单原子图元
 * 【必解 bug】两个键连原子靠得近
 */
int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    return testMolFormat();
//    return testReg();
//    return loopBenchMark();
//    return handleSuperAtom();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
