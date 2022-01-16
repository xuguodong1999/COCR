#ifndef MAT_H
#define MAT_H
#include "cg.h"
#include "xyz.h"
#include "ray.h"
#include "curve.h"
class Mat {
public:
	virtual bool reflect(
		const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
	)const = 0;
	virtual XYZ emit()const;//用在Lamp上，返回灯光颜色，一般材料返回0
};

//纯散射材料
class Cloth :public Mat {
public:
	Cloth(const XYZ& _loss);
	bool reflect(
		const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
	)const override;
private:
	XYZ loss;//反映对不同颜色的吸收度
};

//散射+反射材料
class Mirror : public Mat {
public:
	Mirror(const XYZ& _loss, real _rough = 0.0);
	bool reflect(
		const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
	)const override;
private:
	XYZ loss;
	real rough;//散射程度
};

//散射+反射+折射材料
class Glass : public Mat {
public:
	Glass(const XYZ& _loss, real _rough = 0.0, real _n = 1.0);
	bool reflect(
		const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
	)const override;
private:
	XYZ loss;
	real rough;//散射程度
	real n;//折射率
};

//光源材料
class Lamp :public Mat {
public:
	Lamp(const XYZ& _color);
	bool reflect(
		const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
	)const override;
	XYZ emit()const override;
private:
	XYZ color;
};

#endif // !MAT_H