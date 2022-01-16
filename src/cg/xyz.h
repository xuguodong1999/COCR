#ifndef XYZ_H
#define XYZ_H
#include "cg.h"
#include <vector>
#include <iostream>

class XYZ;
namespace xyz {
	std::vector<XYZ> mul3x3(const std::vector<XYZ> a, const std::vector<XYZ>b);
	XYZ mul3x1(const std::vector<XYZ> a, const XYZ b);
	std::vector<XYZ> inverse3x3(const std::vector<XYZ> a);
	//将点以点为中心按比例k对映
	XYZ cast(const XYZ& _from, real _k, const XYZ& _center);
	//将点垂直于线按比例k对映
	XYZ cast(const XYZ& _from, real _k, const XYZ& _p1, const XYZ& _p2);
	//将点垂直于面按比例k对映
	XYZ cast(const XYZ& _from, real _k, const XYZ& _p1, const XYZ& _p2, const XYZ& _p3);
	//点在线上的垂足
	XYZ foot(const XYZ& _from, const XYZ& _p1, const XYZ& _p2);
	//反射，返回反射光方向
	XYZ reflect(const XYZ& _in, const XYZ& _n);
	//折射，全反射返回false，n1、n2表示两种介质的折射率，默认n1=1，折射方向存入out
	bool refract(const XYZ& _in, const XYZ& _n, XYZ& _out, const real _n2, const real _n1 = 1.0);
	//返回单位球中心指向内部的任意向量
	XYZ randAtNormSphere();
	//返回XY平面的单位向量
	XYZ randAtNormXY();
};
class XYZ {
public:
	XYZ(real _x = 0, real _y = 0, real _z = 0);
	XYZ(const XYZ& xyz);
	//约定XYZ无副作用
	real x()const;
	real y()const;
	real z()const;
	//约定通过RGB修改值
	real& r();
	real& g();
	real& b();

	real operator[](int i) const;
	real& operator[](int i);
	const XYZ& operator+() const;
	XYZ operator-() const;
	XYZ& operator+=(const XYZ& _xyz);
	XYZ& operator-=(const XYZ& _xyz);
	XYZ& operator*=(const XYZ& _xyz);
	XYZ& operator/=(const XYZ& _xyz);
	XYZ& operator*=(const real _t);
	XYZ& operator/=(const real _t);

	const real* data()const;
	real length()const;
	XYZ& abs();
	XYZ& norm();
	real dot(const XYZ& _xyz)const;
	XYZ cross(const XYZ& _xyz)const;

	friend bool operator==(const XYZ& _xyz0, const XYZ& _xyz);
	friend bool operator!=(const XYZ& _xyz0, const XYZ& _xyz);
	friend std::istream& operator>>(std::istream& _in, XYZ& _t);
	friend std::ostream& operator<<(std::ostream& _out, const XYZ& _t);
	friend XYZ operator+(const XYZ& _xyz0, const XYZ& _xyz);
	friend XYZ operator-(const XYZ& _xyz0, const XYZ& _xyz);
	friend XYZ operator*(const XYZ& _xyz0, const XYZ& _xyz);
	friend XYZ operator/(const XYZ& _xyz0, const XYZ& _xyz);
	friend XYZ operator*(real _t, const XYZ& _xyz);
	friend XYZ operator/(XYZ _xyz, real _t);
	friend XYZ operator*(const XYZ& _xyz, real _t);
private:
	real ptr[3];
};

real len(const XYZ& _xyz);
XYZ abs(const XYZ& _xyz);
XYZ norm(const XYZ& _xyz);
real dot(const XYZ& _xyz0, const XYZ& _xyz);
XYZ cross(const XYZ& _xyz0, const XYZ& _xyz);

#endif // !XYZ_H
