#include "tube.h"
#include <cassert>
#include <vector>
using namespace std;
Tube::Tube() {

}

Tube::Tube(XYZ _o1, XYZ _o2, real _r, Mat* _m)
	:o1(_o1), o2(_o2), r(_r), m(_m) {
	ss.emplace_back(Sphere(o1, r, m));
	ss.emplace_back(Sphere(o2, r, m));
	real start = 0, d = (o1 - o2).length() / 30, end = (o1 - o2).length();
	for (auto i = start + d; i < end; i += d) {
		ss.emplace_back(Sphere(xyz::cast(o1, i / end - 1, o2), r, m));
	}
}

bool Tube::hitBy(const Ray& ray, Intersection& hit, real t_max, real t_min) const {
	//XYZ magic(-214434.414233, -8797644.04589);
	//real h = (o1 - o2).length();
	//XYZ a(o1), b(o2), c(norm(xyz::foot(magic, o1, o2) - magic) * r + o1),
	//	A(0, 0, 0), B(0, 0, h), C(0, r, 0);
	//auto m1 = xyz::inverse3x3({ a,b,c });
	//auto m2 = { A,B,C };
	//auto t = xyz::mul3x3(m1, m2);
	//auto from = xyz::mul3x1(t, ray.from());
	//auto direction = xyz::mul3x1(t, ray.direction());
	////计算直线和Z=0，Z=h的交点
	//real z0 = from.z();
	//return false;

	Intersection tmp;
	bool isHitted = false;
	for (auto& i : ss) {
		//拿到最近的hit点，阻断传播
		if (i.hitBy(ray, tmp, t_max, t_min)) {
			isHitted = true;
			t_max = tmp.t;//限制参数上界
			hit = tmp;
		}
	}
	return isHitted;
}