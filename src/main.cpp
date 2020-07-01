#include <opencv2/opencv.hpp>
#include <iostream>


#include <openbabel/atom.h>

#include <string>
#include <openbabel/mol.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

using namespace OpenBabel;
using namespace std;
using namespace cv;

class ForceField {
public:
    ForceField();

    ~ForceField();

    std::string getCoordinate(
            const char *inData,
            const char *inType = "smiles",
            const char *outType = "mol2",
            const char *forcefield = "uff"
    );

    double getEnergy(
            const char *inData,
            const char *inType = "mol2",
            const char *forcefield = "uff"
    );

    double getEnergy();

private:
    double energy;
    static bool pluginLoaded;
};

bool ForceField::pluginLoaded = false;

ForceField::ForceField() {
    if (pluginLoaded) {
        return;
    }
    OBPlugin::LoadAllPlugins();
    pluginLoaded = true;
}

ForceField::~ForceField() {

}

string ForceField::getCoordinate(
        const char *inData,
        const char *inType,
        const char *outType,
        const char *forcefield
) {
    OBConversion conv;
    OBFormat *format_in = conv.FindFormat(inType);
    OBFormat *format_out = conv.FindFormat(outType);
    if (!format_in || !format_out || !conv.SetInAndOutFormats(format_in, format_out)) {
        string tmpInfo = "inType = ";
        tmpInfo += inType;
        tmpInfo += ", outType = ";
        tmpInfo += outType;
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                tmpInfo.c_str()
//        );
        return "";
    }
    stringstream ism(inData);
    if (ism.eof()) {
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                "empty input data!"
//        );
        return "";
    }
    OBMol mol;
    if (!conv.Read(&mol, &ism)) {
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                "fail to perform conv.Read(&mol, &ism)"
//        );
        return "";
    }
    if (mol.Empty()) {
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                "fail to check mol.Empty()"
//        );
        return "";
    }
    OBForceField *pFF = OBForceField::FindForceField(forcefield);
    if (!pFF) {
        string tmpInfo = "could not find forcefield: ";
        tmpInfo += forcefield;
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                tmpInfo.c_str()
//        );
        return "";
    }


    OBBuilder builder;
    //builder.CorrectStereoAtoms(mol);
    //builder.CorrectStereoBonds(mol);
    builder.Build(mol);
    mol.AddHydrogens(false, true);
    //FOR_ATOMS_OF_MOL(atom, mol) {
    //	cout << "id: " << atom->GetId()
    //		<< " x: " << atom->x()<< " y: " << atom->y()<< " z: " << atom->z() << endl;
    //}

    if (!pFF->Setup(mol)) {
//        throwErrorBox(
//                "Error in ForceField::getCoordinate",
//                "could not setup force field"
//        );
        return "";
    }

    //pFF->SteepestDescent(500, 1.0e-4);
    //pFF->WeightedRotorSearch(250, 50);
    //pFF->SteepestDescent(500, 1.0e-6);
    pFF->SteepestDescent(100, 1.0e-4);
    pFF->WeightedRotorSearch(50, 10);
    pFF->SteepestDescent(100, 1.0e-6);
    pFF->UpdateCoordinates(mol);
    energy = pFF->Energy();

    stringstream osm;
    conv.Write(&mol, &osm);
    return osm.str();
}

double ForceField::getEnergy() {
    return energy;
}

double ForceField::getEnergy(
        const char *inData,
        const char *inType,
        const char *forcefield
) {
    OBConversion conv;
    OBFormat *format_in = conv.FindFormat(inType);

    if (!format_in || !conv.SetInFormat(format_in)) {
        string tmpInfo = "inType = ";
        tmpInfo += inType;
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                tmpInfo.c_str()
//        );
        return -1;
    }
    stringstream ism(inData);
    if (ism.eof()) {
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                "empty input data!"
//        );
        return -1;
    }
    OBMol mol;
    if (!conv.Read(&mol, &ism)) {
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                "fail to perform conv.Read(&mol, &ism)"
//        );
        return -1;
    }
    if (mol.Empty()) {
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                "fail to check mol.Empty()"
//        );
        return -1;
    }

    OBForceField *pFF = OBForceField::FindForceField(forcefield);
    if (!pFF) {
        string tmpInfo = "could not find forcefield: ";
        tmpInfo += forcefield;
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                tmpInfo.c_str()
//        );
        return -1;
    }

    OBBuilder builder;
    builder.Build(mol);

    mol.AddHydrogens(false, true);
    if (!pFF->Setup(mol)) {
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                "could not setup force field"
//        );
        return -1;
    }
    energy = pFF->Energy(false);
    if (!isfinite(energy)) {
        string tmpInfo = "Error calculating energy:infinite result for ";
        tmpInfo += mol.GetTitle();
//        throwErrorBox(
//                "Error in ForceField::getEnergy",
//                tmpInfo.c_str()
//        );
        return -1;
    }
    return energy;
}

#include <QApplication>
#include <QWidget>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QWidget w;
    w.show();
    ForceField f;
    auto coor = f.getCoordinate("c1ccccc1", "smiles", "mol2", "mmff94");
    cout << "coor=" << coor << endl;
//    Mat img = imread("/home/xgd/Pictures/backup/4.jpg");
//    Mat img=imread("C:\\Users\\xgd\\Pictures\\2017\\IMG_20170630_125619.jpg");
//    if (img.empty()) {
//        cout << "打开图像失败！" << endl;
//    } else {
//        imshow("image", img);
//        waitKey();
//    }
    return app.exec();
}

