#include "statistic.hpp"

float RC::sNormSizeK = 1.0;
float RC::sSubSizeK = 1.0;
float RC::sNormOffsetKx = 0.2;
float RC::sNormOffsetKy = 0.2;
float RC::sSubOffsetKx = 0.1;
float RC::sSubOffsetKy = 0.1;
float RC::sAngleK = 0.0;
std::set<string> RC::bsSet, RC::aeSet, RC::btSet;
std::set<int> RC::acSet;
RC::ShapeAttr RC::shapeAttr;