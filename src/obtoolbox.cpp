#include "obtoolbox.h"
#include "ioutil.h"
#include <openbabel/mol.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

using namespace OpenBabel;
using namespace std;
namespace cocr {


    ErrorCode OBToolBox::smiles2coor3d(const string &smiles, std::string &result,
                                       const OBToolBox::OBMolFormat &outputFormat,
                                       const OBToolBox::OBForceField &forceFieldType) {
        OBConversion conv;
        OBFormat *formatIn = conv.FindFormat("smiles");
        if (!formatIn) { return ErrorCode::OBFormatMissing; }
        OBFormat *formatOut = nullptr;
        switch (outputFormat) {
            case OBMolFormat::MOL2:
                formatOut = conv.FindFormat("mol2");
                break;
            case OBMolFormat::PDB:
                formatOut = conv.FindFormat("pdb");
                break;
            default:
                formatOut = conv.FindFormat("pdb");
        }
        if (!formatOut) { return ErrorCode::OBFormatMissing; }
        conv.SetInAndOutFormats(formatIn, formatOut);
        stringstream reader(smiles);
        OBMol mol;
        conv.Read(&mol, &reader);
        if (mol.Empty()) { return ErrorCode::OBReadError; }
        OpenBabel::OBForceField *pFF = nullptr;
        switch (forceFieldType) {
            case OBForceField::UFF:
                pFF = OpenBabel::OBForceField::FindForceField("uff");
                break;
            case OBForceField::MMFF94:
                pFF = OpenBabel::OBForceField::FindForceField("mmff94");
                break;
            case OBForceField::GAFF:
                pFF = OpenBabel::OBForceField::FindForceField("gaff");
                break;
            case OBForceField::GHEMICAL:
                pFF = OpenBabel::OBForceField::FindForceField("ghemical");
                break;
            default:
                pFF = OpenBabel::OBForceField::FindForceField("uff");
        }
        if (!pFF) { return ErrorCode::OBForceFieldMissing; }
        OBBuilder builder;
        builder.Build(mol);
        mol.AddHydrogens(false, true);
        pFF->Setup(mol);
        pFF->SteepestDescent(100, 1.0e-4);
        pFF->WeightedRotorSearch(50, 10);
        pFF->SteepestDescent(100, 1.0e-6);
        pFF->UpdateCoordinates(mol);
        stringstream writter(result);
        conv.Write(&mol, &writter);
        return ErrorCode::NormalExec;
    }

    OBToolBox::OBToolBox() {
        const auto obDataPath = qApp->applicationDirPath().toStdString() + "/obabel";
        const auto envEntry = "BABEL_DATADIR=" + obDataPath;
        putenv(const_cast<char *>(envEntry.c_str()));
        releaseToDir(":/obabel", obDataPath.c_str());
    }
}