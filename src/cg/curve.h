#ifndef HIT_H
#define HIT_H
#include "cg.h"
#include "xyz.h"
#include "ray.h"
#include <vector>
class Mat;
class Intersection {
public:
	real t;     //参数
	XYZ p;      //交点
	XYZ n;      //朝外的法线
	Mat* mat;   //材质
};

class Curve {
public:
	virtual bool hitBy(const Ray& ray, Intersection& hit, real t_max = cg::_MAX_REAL, real t_min = cg::_MIN_REAL) const = 0;
};

//曲面集合
class CurveGroup : public Curve {
public:
	CurveGroup(const std::vector<const Curve*>& l);
	bool hitBy(
		const Ray& ray, Intersection& hit,
		real t_max = cg::_MAX_REAL, real t_min = cg::_MIN_REAL
	) const override;
	~CurveGroup();
private:
	CurveGroup();
	std::vector<const Curve*>objList;
};

#endif // !HIT_H