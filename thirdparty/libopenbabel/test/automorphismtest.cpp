#include "obtest.h"
#include <openbabel/isomorphism.h>
#include <openbabel/query.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

using namespace std;
using namespace OpenBabel;

bool doAutomorphismTest(OBMol &mol, int numAutomorphisms)
{
  OBIsomorphismMapper::Mappings G;
  FindAutomorphisms(&mol, G);

  return (G.size() == numAutomorphisms);
}

void testAutomorphisms()
{
  OBMol mol;
  OBConversion conv;
  conv.SetInFormat("smi");
  conv.ReadString(&mol, "C1C(CC2CC2)C1");

  Automorphisms aut;
  FindAutomorphisms((OBMol*)&mol, aut);
  BOOST_REQUIRE_EQUAL( aut.size(), 8 );
}

/**
 * Test detection of stereoisomers
 */

BOOST_AUTO_TEST_CASE(automorphismtest)
{
  OBMol mol;
  OBConversion conv;
  BOOST_REQUIRE( conv.SetInFormat("mol") );
  testAutomorphisms();
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_1.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 8) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_2.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 2) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_3.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 48) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_4.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 2) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_5.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 2) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_6.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 6) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_7.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 1) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_8.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 1) );
  BOOST_REQUIRE( conv.ReadFile(&mol, OBTestUtil::GetFilename("hao_xu_9.mol")) );
  BOOST_REQUIRE( doAutomorphismTest(mol, 20) );
}



