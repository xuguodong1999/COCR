#pragma once

#include "XGD/Base/Fraction.h"
#include <string>

//为字符串tar按照化学方程式书写规则添加HTML标签
std::string add_html(const std::string &_tar);

//返回元素ele的质子数
int Get_proton_num(std::string &ele);

//返回元素ele的相对原子质量
double Get_relative_atomic_mass(std::string &ele);

//返回第pn号元素的相对原子质量
double Get_relative_atomic_mass(int proton_num);

//返回第pn号元素的电负性
double Get_electronegativity(int proton_num);

frac to_frac(const std::string &_src);
