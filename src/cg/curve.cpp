#include "curve.h"
using namespace std;

CurveGroup::CurveGroup(const std::vector<const Curve*>& l) :objList(l) {}

bool CurveGroup::hitBy(
	const Ray& ray, Intersection& hit,
	real t_max, real t_min
) const {
	Intersection tmp;
	bool isHitted = false;
	for (auto& i : objList) {
		//拿到最近的hit点，阻断传播
		if (i->hitBy(ray, tmp, t_max, t_min)) {
			isHitted = true;
			t_max = tmp.t;//限制参数上界
			hit = tmp;
		}
	}
	return isHitted;
}

CurveGroup::CurveGroup() {}

CurveGroup::~CurveGroup() {

}
