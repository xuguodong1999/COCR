#include "axx_ndk.h"


#include <openbabel/mol.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

#include <jni.h>
#include <string>
#include <exception>

//using namespace cv;
using namespace std;
using namespace OpenBabel;

std::string convertSmiToPdb(const char *smi) {
    OBConversion conv;
    try {
        auto format_in = conv.FindFormat("smiles");
        auto format_out = conv.FindFormat("pdb");
        stringstream ism(smi);
        OBMol mol;
        RET_EMPTY_STRING_IF_NOT(conv.SetInFormat(format_in));
        RET_EMPTY_STRING_IF_NOT(conv.SetOutFormat(format_out));
        RET_EMPTY_STRING_IF_NOT(conv.Read(&mol, &ism));
        auto pFF = OBForceField::FindForceField("UFF");
        RET_EMPTY_STRING_IF_NOT(pFF);
        OBBuilder builder;
        RET_EMPTY_STRING_IF_NOT(builder.Build(mol));
        RET_EMPTY_STRING_IF_NOT(mol.AddHydrogens());
        // FIXME: ret value in OBForceField::Setup is false?
        pFF->Setup(mol);
        RET_EMPTY_STRING_IF_NOT(pFF->UpdateCoordinates(mol));
        stringstream osm;
        RET_EMPTY_STRING_IF_NOT(conv.Write(&mol, &osm));
        return std::move(osm.str());
    } catch (exception e) {
        return "";
    }
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_xgd_axxlab_DetectActivity_stringFromJNI(
        JNIEnv *env, jobject) {
    std::string hello = "Hello from C++\n";
    hello += convertSmiToPdb("c1ccccc1");
//    Mat mat(1024, 1024, CV_8UC3);
//    ostringstream osm;
//    osm << endl << mat.size() << endl;
//    hello += osm.str();
    return env->NewStringUTF(hello.c_str());
}
