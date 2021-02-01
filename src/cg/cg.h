#ifndef CG_H
#define CG_H
#include <cmath>
#include <limits>
typedef double real;
namespace cg {
	const real _PI_1 = acos(-1);
	const real _PI_2 = acos(-1) / 2.0;
	const real _PI_4 = acos(-1) / 4.0;
	const real _2_PI = acos(-1) * 2.0;
	const real _4_PI = acos(-1) * 4.0;
	const real _EPS = 1e-6;
	const real _MAX_REAL = std::numeric_limits<real>::max();
	const real _MIN_REAL = std::numeric_limits<real>::min();
};
#define rand_0_1		(rand() / (RAND_MAX + cg::_EPS))
#define look(a)			std::cout<<"["<<#a<<"]= "<<(a)<<std::endl
#define angle2rad(a)	a * cg::_PI_1 / 180.0
#define rad2angle(r)	r * 180.0 / cg::_PI_1
#define safedelete(ptr)		if(ptr!=nullptr){delete ptr;ptr=nullptr;}
#endif // !CG_H
