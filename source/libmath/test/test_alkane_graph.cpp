#include <catch2/catch.hpp>
#include "math/alkane_graph.h"

TEST_CASE("alkane_graph", "") {
    AlkaneGraph<unsigned, unsigned> alkane1, alkane2;
    const std::string smi1{"CC(C)C"}, smi2{"C(C)CC"};
    alkane1.fromSMILES(smi1);
    alkane2.fromSMILES(smi2);
    REQUIRE(alkane1.hash() == alkane2.hash());
}