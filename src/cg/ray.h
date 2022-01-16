#ifndef RAY_H
#define RAY_H
#include "cg.h"
#include "xyz.h"
class Ray {
public:
	Ray();
	Ray(const XYZ& _a, const XYZ& _b);
	XYZ from() const;//起点
	XYZ direction() const;//方向
	XYZ to(const real t) const;//终点
private:
	//r=a+tb
	XYZ a;
	XYZ b;
};

#endif // !RAY_H