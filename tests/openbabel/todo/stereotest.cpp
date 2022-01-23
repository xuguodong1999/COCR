#include <boost/test/unit_test.hpp>
#include <openbabel/stereo/stereo.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/graphsym.h>
#include <algorithm>
#include <openbabel/atom.h>
#include <openbabel/obiter.h>

using namespace std;
using namespace OpenBabel;

void test_MakeRefs()
{
  OBStereo::Refs refs = OBStereo::MakeRefs(1, 3, 5);
  BOOST_REQUIRE( refs.size() == 3 );
  BOOST_REQUIRE( refs[0] == 1 );
  BOOST_REQUIRE( refs[1] == 3 );
  BOOST_REQUIRE( refs[2] == 5 );

  refs = OBStereo::MakeRefs(1, 3, 5, 7);
  BOOST_REQUIRE( refs.size() == 4 );
  BOOST_REQUIRE( refs[0] == 1 );
  BOOST_REQUIRE( refs[1] == 3 );
  BOOST_REQUIRE( refs[2] == 5 );
  BOOST_REQUIRE( refs[3] == 7 );
}

void test_ContainsSameRefs()
{
  BOOST_REQUIRE( OBStereo::ContainsSameRefs(OBStereo::MakeRefs(1, 2, 3),
                                        OBStereo::MakeRefs(2, 3, 1)) );

  BOOST_REQUIRE( !OBStereo::ContainsSameRefs(OBStereo::MakeRefs(1, 2, 3),
                                         OBStereo::MakeRefs(2, 7, 1)) );

  // different sizes
  BOOST_REQUIRE( !OBStereo::ContainsSameRefs(OBStereo::MakeRefs(1, 2, 3),
                                        OBStereo::MakeRefs(2, 3, 1, 4)) );
}

void test_NumInversions()
{
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(1, 2, 3)) == 0 );
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(1, 3, 2)) == 1 );
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(2, 1, 3)) == 1 );
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(2, 3, 1)) == 2 );
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(3, 1, 2)) == 2 );
  BOOST_REQUIRE( OBStereo::NumInversions(OBStereo::MakeRefs(3, 2, 1)) == 3 );
}

void test_Permutate()
{
  OBStereo::Refs refs = OBStereo::MakeRefs(1, 2, 3);
  OBStereo::Permutate(refs, 1, 2);

  BOOST_REQUIRE( refs[0] == 1 );
  BOOST_REQUIRE( refs[1] == 3 );
  BOOST_REQUIRE( refs[2] == 2 );

  // test out of range indexes for crashes
  OBStereo::Permutate(refs, -1, 1);
  OBStereo::Permutate(refs, 1, -1);
  OBStereo::Permutate(refs, 9, 1);
  OBStereo::Permutate(refs, 1, 9);
}

void test_Permutated()
{
  OBStereo::Refs refs = OBStereo::MakeRefs(1, 2, 3);
  OBStereo::Refs mutant = OBStereo::Permutated(refs, 1, 2);

  BOOST_REQUIRE( refs[0] == 1 );
  BOOST_REQUIRE( refs[1] == 2 );
  BOOST_REQUIRE( refs[2] == 3 );

  BOOST_REQUIRE( mutant[0] == 1 );
  BOOST_REQUIRE( mutant[1] == 3 );
  BOOST_REQUIRE( mutant[2] == 2 );

}


std::string GetFilename(const std::string &filename)
{
  string path = TESTDATADIR + filename;
  return path;
}

bool doStereoPerception(OBMol &mol, int numTetrahedral, int numCisTrans)
{
  // need to calculate symmetry first
  std::vector<unsigned int> symmetry_classes;
  OpenBabel::OBGraphSym graphsym(&mol);
  graphsym.GetSymmetry(symmetry_classes);

  OBStereoUnitSet units = FindStereogenicUnits(&mol, symmetry_classes);

  int tetrahedralCount = 0;
  int cistransCount = 0;
  for (unsigned int i = 0; i < units.size(); ++i) {
    switch (units.at(i).type) {
      case OBStereo::Tetrahedral:
        tetrahedralCount++;
        break;
      case OBStereo::CisTrans:
        cistransCount++;
        break;
    }
  }

  BOOST_REQUIRE_EQUAL(tetrahedralCount, numTetrahedral);
  BOOST_REQUIRE_EQUAL(cistransCount, numCisTrans);

  return (tetrahedralCount == numTetrahedral) && (cistransCount == numCisTrans);
}

bool doStereoPerception2(OBMol &mol, int numTetrahedral, int numCisTrans)
{
  // need to calculate symmetry first
  std::vector<unsigned int> symmetry_classes;
  OpenBabel::OBGraphSym graphsym(&mol);
  graphsym.GetSymmetry(symmetry_classes);

  Automorphisms G;
  FindAutomorphisms(&mol, G, symmetry_classes);
  std::vector<OpenBabel::OBStereoUnit> units = FindStereogenicUnits(&mol, symmetry_classes, G);

  int tetrahedralCount = 0;
  int cistransCount = 0;
  for (unsigned int i = 0; i < units.size(); ++i) {
    switch (units.at(i).type) {
      case OBStereo::Tetrahedral:
        tetrahedralCount++;
        break;
      case OBStereo::CisTrans:
        cistransCount++;
        break;
    }
  }

  BOOST_REQUIRE_EQUAL(tetrahedralCount, numTetrahedral);
  BOOST_REQUIRE_EQUAL(cistransCount, numCisTrans);

  return (tetrahedralCount == numTetrahedral) && (cistransCount == numCisTrans);
}

OBStereoUnitSet Units(const OBStereoUnit &u1)
{ OBStereoUnitSet units; units.push_back(u1); return units; }
OBStereoUnitSet Units(const OBStereoUnit &u1, const OBStereoUnit &u2)
{ OBStereoUnitSet units = Units(u1); units.push_back(u2); return units; }
OBStereoUnitSet Units(const OBStereoUnit &u1, const OBStereoUnit &u2,
    const OBStereoUnit &u3)
{ OBStereoUnitSet units = Units(u1, u2); units.push_back(u3); return units; }
OBStereoUnitSet Units(const OBStereoUnit &u1, const OBStereoUnit &u2,
    const OBStereoUnit &u3, const OBStereoUnit &u4)
{ OBStereoUnitSet units = Units(u1, u2, u3); units.push_back(u4); return units; }
OBStereoUnitSet Units(const OBStereoUnit &u1, const OBStereoUnit &u2,
    const OBStereoUnit &u3, const OBStereoUnit &u4, const OBStereoUnit &u5)
{ OBStereoUnitSet units = Units(u1, u2, u3, u4); units.push_back(u5); return units; }

#define TH(id) OBStereoUnit(OBStereo::Tetrahedral, id)

std::vector< std::vector<OpenBabel::OBStereoUnit> > DepUnits(const OBStereoUnitSet &u1)
{ std::vector< std::vector<OpenBabel::OBStereoUnit> > sets; sets.push_back(u1); return sets; }
std::vector< std::vector<OpenBabel::OBStereoUnit> > DepUnits(const OBStereoUnitSet &u1,
    const OBStereoUnitSet &u2)
{ std::vector< std::vector<OpenBabel::OBStereoUnit> > sets = DepUnits(u1); sets.push_back(u2); return sets; }
std::vector< std::vector<OpenBabel::OBStereoUnit> > DepUnits(const OBStereoUnitSet &u1,
    const OBStereoUnitSet &u2, const OBStereoUnitSet &u3)
{ std::vector< std::vector<OpenBabel::OBStereoUnit> > sets = DepUnits(u1, u2); sets.push_back(u3); return sets; }




bool doStereoPerception3(OBMol &mol, const OBStereoUnitSet &refUnits = OBStereoUnitSet(),
    const OBStereoUnitSetOfSets refInterdependent =
    OBStereoUnitSetOfSets())
{
  std::vector<OBAtom*> atoms;
  FOR_ATOMS_OF_MOL(atom, mol)
    atoms.push_back(&*atom);

  for (int i = 0; i < 100; ++i) {
    std::random_shuffle(atoms.begin(), atoms.end());
    mol.RenumberAtoms(atoms);

    // need to calculate symmetry first
    std::vector<unsigned int> symmetry_classes;
    OpenBabel::OBGraphSym graphsym(&mol);
    int nclasses = graphsym.GetSymmetry(symmetry_classes);
    cout << "nclasses = " << nclasses << endl;
    BOOST_REQUIRE( nclasses == 7 );

    Automorphisms G;
    FindAutomorphisms(&mol, G, symmetry_classes);
    cout << "G.size " << G.size() << endl;
    std::vector<OpenBabel::OBStereoUnit> units = FindStereogenicUnits(&mol, symmetry_classes, G);

    BOOST_REQUIRE_EQUAL(units.size(), refUnits.size());

    for (unsigned int i = 0; i < units.size(); ++i) {
      bool foundUnit = false;
      for (unsigned int j = 0; j < refUnits.size(); ++j) {
        if (units[i].type == refUnits[j].type)
          if (units[i].id == refUnits[j].id)
            foundUnit = true;
      }
      BOOST_REQUIRE( foundUnit );
    }

  }
  return true;
}

/**
 * Test detection of stereocenters
 *
 *
 */
void test_StereoPerception()
{
  OBMol mol;
  OBConversion conv;
  BOOST_REQUIRE( conv.SetInFormat("mol") );

  /*
  cout << "structure break1" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/break1.mol")) );
    // need to calculate symmetry first
    std::vector<unsigned int> symmetry_classes;
    OpenBabel::OBGraphSym graphsym(&mol);
    cout << "nclasses = " << graphsym.GetSymmetry(symmetry_classes, false) << endl;

    Automorphisms G = FindAutomorphisms(&mol, symmetry_classes);
    cout << "G.size " << G.size() << endl;

  BOOST_REQUIRE( doStereoPerception3(mol, Units(TH(0), TH(4), TH(3), TH(9), TH(15)),
        DepUnits(Units(TH(0), TH(4)), Units(TH(3), TH(9)), Units(TH(15)))) );
  */

  cout << "structure perception1" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception1.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception2" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception2.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception3" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception3.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "structure perception4" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception4.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception5" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception5.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception6" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception6.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 2) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 2) );

  cout << "structure perception7" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception7.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception8" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception8.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception9" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception9.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception10" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception10.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 1, 1) );
  BOOST_REQUIRE( doStereoPerception2(mol, 1, 1) );

  cout << "structure perception11" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception11.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 3, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 3, 0) );

  cout << "structure perception12" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception12.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception13" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception13.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception14" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception14.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception15" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception15.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "structure perception16" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception16.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 1, 2) );
  BOOST_REQUIRE( doStereoPerception2(mol, 1, 2) );

  cout << "structure perception17" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception17.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "structure perception18" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception18.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 3, 1) );
  BOOST_REQUIRE( doStereoPerception2(mol, 3, 1) );

  cout << "structure perception19" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception19.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "structure perception20" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/perception20.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 4, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 4, 0) );

  /*
   * J. Chem. Inf. Comput. Sci., Vol. 33, No. 6, 1993
   *
   * Figure 1. Examples of compounds containing parastereocenters
   */
  cout << "Razinger paper, fig. 1: structure a" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_a.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 2) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 2) );

  cout << "Razinger paper, fig. 1: structure b" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_b.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 2) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 2) );

  cout << "Razinger paper, fig. 1: structure c" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_c.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "Razinger paper, fig. 1: structure d" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_d.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 3, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 3, 0) );

  cout << "Razinger paper, fig. 1: structure e" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_e.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 6, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 6, 0) );

  cout << "Razinger paper, fig. 1: structure f" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_f.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 1) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 1) );

  cout << "Razinger paper, fig. 1: structure g" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_g.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 0, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 0, 0) );

  cout << "Razinger paper, fig. 1: structure h" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_h.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "Razinger paper, fig. 1: structure i" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_i.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 3, 0) ); // rule 2a
  BOOST_REQUIRE( doStereoPerception2(mol, 3, 0) );

  cout << "Razinger paper, fig. 1: structure j" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_j.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 7, 0) ); // rule 2b
  BOOST_REQUIRE( doStereoPerception2(mol, 7, 0) );

  cout << "Razinger paper, fig. 1: structure k" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_k.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 3, 0) ); // counter example to j
  BOOST_REQUIRE( doStereoPerception2(mol, 3, 0) );

  cout << "Razinger paper, fig. 1: structure l" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_l.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 13, 0) ); // rule 2b
  BOOST_REQUIRE( doStereoPerception2(mol, 13, 0) );

  cout << "Razinger paper, fig. 1: structure m" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_m.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 12, 0) ); // counter example to l
  BOOST_REQUIRE( doStereoPerception2(mol, 12, 0) );

  cout << "Razinger paper, fig. 1: structure n" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_n.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 5, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 5, 0) );

  cout << "Razinger paper, fig. 1: structure o" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_o.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 2, 0) );
  BOOST_REQUIRE( doStereoPerception2(mol, 2, 0) );

  cout << "Razinger paper, fig. 1: structure p" << endl;
  BOOST_REQUIRE( conv.ReadFile(&mol, GetFilename("stereo/razinger_fig1_p.mol")) );
  BOOST_REQUIRE( doStereoPerception(mol, 5, 2) );
  BOOST_REQUIRE( doStereoPerception2(mol, 5, 2) );



//  conv.ReadString(&mol, "CC1CCC(C)CC1");
}




BOOST_AUTO_TEST_CASE(stereotest)
{
  int defaultchoice = 1;
  
  int choice = defaultchoice;

  if (argc > 1) {
    if(sscanf(argv[1], "%d", &choice) != 1) {
      printf("Couldn't parse that input as a number\n");
      return -1;
    }
  }
  // Define location of file formats for testing
  #ifdef FORMATDIR
    char env[BUFF_SIZE];
    snprintf(env, BUFF_SIZE, "BABEL_LIBDIR=%s", FORMATDIR);
    putenv(env);
  #endif

  switch(choice) {
  case 1:
    test_MakeRefs();
    break;
  case 2:
    test_ContainsSameRefs();
    break;
  case 3:
    test_NumInversions();
    break;
  case 4:
    test_Permutate();
    break;
  case 5:
    test_Permutated();
    break;
  case 6:
    test_StereoPerception();
    break;
  default:
    cout << "Test number " << choice << " does not exist!\n";
    return -1;
  }

  return 0;
}
