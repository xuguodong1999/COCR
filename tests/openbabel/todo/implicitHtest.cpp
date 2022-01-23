#include <boost/test/unit_test.hpp>
#include <openbabel/obconversion.h>
#include <openbabel/math/vector3.h>
#include "obtest.h"
using namespace std;
using namespace OpenBabel;

typedef vector<vector3> vv3;

void testLossOfHydrogen(string filename)
{
  string testfile = OBTestUtil::GetFilename(filename);
  ifstream ifs(testfile.c_str());
  OBConversion conv(&ifs);
  BOOST_REQUIRE(conv.SetInFormat("sdf"));
  OBMol mol;
  BOOST_REQUIRE(conv.Read(&mol));
  bool success = true;
  int i = 0;
  while (success) {
    unsigned int Natoms = mol.NumAtoms();
    mol.DeleteHydrogens();
    mol.AddHydrogens();
    unsigned int newNatoms = mol.NumAtoms();
    cout << "Mol#" << i << ", Title " << mol.GetTitle() << ", Original atoms vs New atoms: ";
    cout << Natoms << " vs " << newNatoms << "\n";
    BOOST_REQUIRE( Natoms == newNatoms);
    cout << "\n";
    success = conv.Read(&mol);
    i += 1;
  }
}

BOOST_AUTO_TEST_CASE(implicitHtest)
{
  testLossOfHydrogen("implicitH.sdf");
}
