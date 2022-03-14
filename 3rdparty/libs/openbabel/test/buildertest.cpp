#include "obtest.h"
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace OpenBabel;

static unsigned int failed = 0;
static unsigned int testCount = 0;

void clearMolFlags(OBMol &mol)
{
  // Both of these are needed or there's a crash.
  mol.UnsetFlag(OB_SSSR_MOL);
  mol.UnsetFlag(OB_AROMATIC_MOL);

  // Not needed, doesn't cause crash
  //  mol.UnsetFlag(OB_RINGFLAGS_MOL);
  //  mol.UnsetFlag(OB_LSSR_MOL);
  //  mol.UnsetFlag(OB_KEKULE_MOL);
  //  mol.UnsetFlag(OB_CLOSURE_MOL);
  //  mol.UnsetFlag(OB_AROM_CORRECTED_MOL);
  //  mol.UnsetFlag(OB_RINGTYPES_MOL);
}

bool doBuildMoleculeTest(OBMol &mol)
{
  testCount++;

  OBBuilder builder;
  BOOST_REQUIRE(builder.Build(mol, false));
  // Does not need clearMolFlags -- crash still happens if you clear here
  // and not after AddHydrogens()
  BOOST_REQUIRE(mol.AddHydrogens());
  BOOST_REQUIRE(mol.HasAromaticPerceived() == 0);
  BOOST_REQUIRE(mol.HasSSSRPerceived() == 0);
  //  clearMolFlags(mol); // must clear here or you crash
  // Should now be handled by AddHydrogens()

  OBForceField* pff = OBForceField::FindType("mmff94");
  BOOST_REQUIRE(pff != nullptr);
  BOOST_REQUIRE(pff->Setup(mol));
  // Check for explosions -- PR#3016479
  pff->SteepestDescent(100);
  BOOST_REQUIRE(!pff->DetectExplosion()); // no explosions please!

  return true;
}

bool doMultiMoleculeFile(const std::string &filename)
{
  std::string file = OBTestUtil::GetFilename(filename);
  std::ifstream ifs;
  std::string filepath = TEST_SAMPLES_PATH;
  filepath += "files/" + filename;
  ifs.open(filepath);
  BOOST_REQUIRE( ifs );

  OBMol mol;
  OBConversion conv(&ifs, &cerr);
  OBFormat *format = conv.FormatFromExt(file.c_str());
  BOOST_REQUIRE(format);
  BOOST_REQUIRE(conv.SetInFormat(format));

  while (conv.Read(&mol, &ifs)) {
    doBuildMoleculeTest(mol);
  }

  return true;
}

bool doSMILESBuilderTest(string smiles)
{
  testCount++;

  OBMol mol;
  OBConversion conv;
  OBFormat *smilesFormat = conv.FindFormat("smi");
  BOOST_REQUIRE(smilesFormat);
  BOOST_REQUIRE(conv.SetInFormat(smilesFormat));

  BOOST_REQUIRE(conv.ReadString(&mol, smiles));

  OBBuilder builder;
  BOOST_REQUIRE(builder.Build(mol, false)); // some stereo errors are known
  return (mol.Has3D() && mol.HasNonZeroCoords());
}

BOOST_AUTO_TEST_CASE(buildertest)
{
  // fails because of selenium
  //  BOOST_REQUIRE( doMultiMoleculeFile("aromatics.smi") );
  // fails because of stereo crash
  //  BOOST_REQUIRE( doMultiMoleculeFile("nci.smi") );
  // fails because of "organometallic" entries
  //  BOOST_REQUIRE( doMultiMoleculeFile("attype.00.smi") );
    BOOST_REQUIRE( doMultiMoleculeFile("forcefield.sdf") );
    BOOST_REQUIRE( doMultiMoleculeFile("filterset.sdf") );
    BOOST_REQUIRE( doSMILESBuilderTest("OC1=CC3=C([C@@]2([H])CC[C@@]4(C)[C@](CC[C@@H]4O)([H])[C@@]([H])2[C@H](CCCCCCCCCS(CCCC(F)(F)C(F)(F)F)=O)C3)C=C1") );
    BOOST_REQUIRE( doSMILESBuilderTest("O1C[C@H]2O[C@H]2c2ccc(Oc3c(O)ccc(CCC1=O)c3)cc2") );
    BOOST_REQUIRE( doSMILESBuilderTest("N12[C@@H]([C@@H](NC([C@@H](c3ccsc3)C(=O)O)=O)C2=O)SC(C)(C)[C@@-]1C(=O)O") );
    BOOST_REQUIRE( doSMILESBuilderTest("OC1(C2=CN(CC3=CC=CC=C3F)N=N2)CCOC1") );
}
