#ifndef SPHERE_H
#define SPHERE_H
#include "cg.h"
#include "xyz.h"
#include "curve.h"
#include "mat.h"
class Sphere : public Curve {
public:
	Sphere(XYZ _o, real _r, Mat* _m);
	bool hitBy(const Ray& ray, Intersection& hit, real t_max = cg::_MAX_REAL, real t_min = cg::_MIN_REAL) const override;
private:
	Sphere();
	XYZ o;  //ÇòÐÄ
	real r; //°ë¾¶
	Mat* m; //²ÄÖÊ
};
#endif // !SPHERE_H