/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "obutil.h"
#include "ioutil.h"
#include <cstdlib>
#include <QByteArray>

#include <openbabel/mol.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

using namespace std;
using namespace OpenBabel;

OBUtil::OBUtil()
{
	OBPlugin::LoadAllPlugins();
}
std::string OBUtil::getpdb(const char* smiles) {
	OBConversion conv;
	OBFormat* format_in = conv.FindFormat("smiles");
	OBFormat* format_out = conv.FindFormat("pdb");
	conv.SetInAndOutFormats(format_in, format_out);
	stringstream ism(smiles);
	OBMol mol;
	conv.Read(&mol, &ism);
	if (mol.Empty()) {
		exit(-3);
	}
	FOR_ATOMS_OF_MOL(atom, mol) {
		cout << "id: " << atom->GetId()
			<< " x: " << atom->x()<< " y: " << atom->y()<< " z: " << atom->z() << endl;
	}
	cout << "3" << endl;
	OBForceField* pFF = OBForceField::FindForceField("MMFF94");
	if (!pFF) {
		exit(-4);
	}
	cout << "4" << endl;
	OBBuilder builder;
	builder.Build(mol);
	mol.AddHydrogens(false, true);
	pFF->Setup(mol);
	pFF->SteepestDescent(100, 1.0e-4);
	pFF->WeightedRotorSearch(50, 10);
	pFF->SteepestDescent(100, 1.0e-6);
	pFF->UpdateCoordinates(mol);
	stringstream osm;
	conv.Write(&mol, &osm);
	return osm.str();
}
bool OBUtil::releasePlugin() {
	// NDK need char* instead of const char*
	putenv(const_cast<char*>(("BABEL_DATADIR=" + obPluginFilePath).constData()));
	//cout << const_cast<char*>(("BABEL_DATADIR=" + obPluginFilePath).constData()) << endl;
	return IOUtil::release(obPluginPath, obPluginFilePath);
}
