#include "mat.h"
using namespace std;

Cloth::Cloth(const XYZ& _loss) : loss(_loss) {

}

bool Cloth::reflect(
	const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
)const {
	//向任意反方向散射
	out = Ray(
		hit.p,
		xyz::randAtNormSphere() + hit.n//约束到半球
	);
	_loss = loss;
	return true;
}

Mirror::Mirror(const XYZ& _loss, real _rough) : loss(_loss), rough(_rough) {
}

bool Mirror::reflect(
	const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
)const {
	XYZ reflected = xyz::reflect(in.direction(), hit.n);
	out = Ray(
		hit.p,
		reflected								//精准反射，模长1
		+ rough * xyz::randAtNormSphere()		//随机散射，模长rough*[0,1)
	);
	_loss = loss;
	return (dot(out.direction(), hit.n) > 0);
}

Glass::Glass(const XYZ& _loss, real _rough, real _n) : loss(_loss), rough(_rough), n(_n) {

}

bool Glass::reflect(
	const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out
)const {
	XYZ _n;
	real _ni;
	_loss = XYZ(1.0, 1.0, 1.0);
	if (dot(in.direction(), hit.n) > 0) {//出射
		_n = -hit.n;
		_ni = 1.0 / n;
	}
	else {//入射
		_n = hit.n;
		_ni = n;
	}
	//折射的时候没考虑反射
	XYZ refracted;
	if (xyz::refract(in.direction(), _n, refracted, _ni)) {
		out = Ray(hit.p, refracted);
		return true;
	}
	//全反射
	XYZ reflected = xyz::reflect(in.direction(), hit.n);
	out = Ray(
		hit.p,
		reflected + rough * xyz::randAtNormSphere()
	);
	_loss = loss;
	return (dot(out.direction(), hit.n) > 0);
}

Lamp::Lamp(const XYZ& _color) :color(_color) {
}

bool Lamp::reflect(const Ray& in, const Intersection& hit, XYZ& _loss, Ray& out) const {
	return false;
}

XYZ Lamp::emit() const {
	return color;
}

XYZ Mat::emit() const {
	return XYZ(0, 0, 0);
}
