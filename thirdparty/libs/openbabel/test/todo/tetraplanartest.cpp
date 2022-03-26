#include <boost/test/unit_test.hpp>
#include <openbabel/stereo/cistrans.h>

using namespace std;
using namespace OpenBabel;

//        2    3
//        -  ///
//        - //
//  0 --- 1
//         \
//          \
//           4


BOOST_AUTO_TEST_CASE(tetraplanartest)
{
  OBCisTransStereo::Config cfg;

  // set clockwise, viewing from 1
  cfg.begin = 0;
  cfg.end = 1;
  cfg.refs = OBStereo::MakeRefs(2, 3, 4, 5);
  BOOST_REQUIRE( cfg.begin == 0 );
  BOOST_REQUIRE( cfg.end == 1 );
  BOOST_REQUIRE( cfg.refs.size() == 4 );
  BOOST_REQUIRE( cfg.refs[0] == 2 );
  BOOST_REQUIRE( cfg.refs[1] == 3 );
  BOOST_REQUIRE( cfg.refs[2] == 4 );
  BOOST_REQUIRE( cfg.refs[3] == 5 );
  BOOST_REQUIRE( cfg.shape == OBStereo::ShapeU );

  // test nothing operation
  OBCisTransStereo::Config cfg2;
  cfg2 = OBTetraPlanarStereo::ToConfig(cfg, 2);
  BOOST_REQUIRE( cfg == cfg2 );
  BOOST_REQUIRE( cfg2.refs[0] == cfg.refs[0] );
  BOOST_REQUIRE( cfg2.refs[1] == cfg.refs[1] );
  BOOST_REQUIRE( cfg2.refs[2] == cfg.refs[2] );
  BOOST_REQUIRE( cfg2.refs[3] == cfg.refs[3] );
  
  // try start = 3
  cfg2 = OBTetraPlanarStereo::ToConfig(cfg, 3);
  BOOST_REQUIRE( cfg2.begin == 0 );
  BOOST_REQUIRE( cfg2.end == 1 );
  BOOST_REQUIRE( cfg2.refs.size() == 4 );
  BOOST_REQUIRE( cfg2.refs[0] == 3 );
  BOOST_REQUIRE( cfg2.refs[1] == 4 );
  BOOST_REQUIRE( cfg2.refs[2] == 5 );
  BOOST_REQUIRE( cfg2.refs[3] == 2 );
  BOOST_REQUIRE( cfg2.shape == OBStereo::ShapeU );

  // try start = 5
  cfg2 = OBTetraPlanarStereo::ToConfig(cfg, 5);
  BOOST_REQUIRE( cfg2.begin == 0 );
  BOOST_REQUIRE( cfg2.end == 1 );
  BOOST_REQUIRE( cfg2.refs.size() == 4 );
  BOOST_REQUIRE( cfg2.refs[0] == 5 );
  BOOST_REQUIRE( cfg2.refs[1] == 2 );
  BOOST_REQUIRE( cfg2.refs[2] == 3 );
  BOOST_REQUIRE( cfg2.refs[3] == 4 );
  BOOST_REQUIRE( cfg2.shape == OBStereo::ShapeU );

  // try U -> Z
  OBCisTransStereo::Config shapeZ = OBTetraPlanarStereo::ToConfig(cfg, 2, OBStereo::ShapeZ);
  BOOST_REQUIRE( shapeZ.begin == 0 );
  BOOST_REQUIRE( shapeZ.end == 1 );
  BOOST_REQUIRE( shapeZ.refs.size() == 4 );
  BOOST_REQUIRE( shapeZ.refs[0] == 2 );
  BOOST_REQUIRE( shapeZ.refs[1] == 3 );
  BOOST_REQUIRE( shapeZ.refs[2] == 5 );
  BOOST_REQUIRE( shapeZ.refs[3] == 4 );
  BOOST_REQUIRE( shapeZ.shape == OBStereo::ShapeZ );

  // try U -> 4
  OBCisTransStereo::Config shape4 = OBTetraPlanarStereo::ToConfig(cfg, 2, OBStereo::Shape4);
  BOOST_REQUIRE( shape4.begin == 0 );
  BOOST_REQUIRE( shape4.end == 1 );
  BOOST_REQUIRE( shape4.refs.size() == 4 );
  BOOST_REQUIRE( shape4.refs[0] == 2 );
  BOOST_REQUIRE( shape4.refs[1] == 4 );
  BOOST_REQUIRE( shape4.refs[2] == 3 );
  BOOST_REQUIRE( shape4.refs[3] == 5 );
  BOOST_REQUIRE( shape4.shape == OBStereo::Shape4 );

  // try Z -> U
  OBCisTransStereo::Config shapeU = OBTetraPlanarStereo::ToConfig(shapeZ, 2, OBStereo::ShapeU);
  BOOST_REQUIRE( shapeU.begin == 0 );
  BOOST_REQUIRE( shapeU.end == 1 );
  BOOST_REQUIRE( shapeU.refs.size() == 4 );
  BOOST_REQUIRE( shapeU.refs[0] == 2 );
  BOOST_REQUIRE( shapeU.refs[1] == 3 );
  BOOST_REQUIRE( shapeU.refs[2] == 4 );
  BOOST_REQUIRE( shapeU.refs[3] == 5 );
  BOOST_REQUIRE( shapeU.shape == OBStereo::ShapeU );

  // try 4 -> U
  shapeU = OBTetraPlanarStereo::ToConfig(shape4, 2, OBStereo::ShapeU);
  BOOST_REQUIRE( shapeU.begin == 0 );
  BOOST_REQUIRE( shapeU.end == 1 );
  BOOST_REQUIRE( shapeU.refs.size() == 4 );
  BOOST_REQUIRE( shapeU.refs[0] == 2 );
  BOOST_REQUIRE( shapeU.refs[1] == 3 );
  BOOST_REQUIRE( shapeU.refs[2] == 4 );
  BOOST_REQUIRE( shapeU.refs[3] == 5 );
  BOOST_REQUIRE( shapeU.shape == OBStereo::ShapeU );

  return 0;
}
