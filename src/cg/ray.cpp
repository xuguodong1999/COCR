#include "ray.h"
Ray::Ray() {

}
Ray::Ray(const XYZ& _a, const XYZ& _b) :a(_a), b(_b) {

}
XYZ Ray::from() const {
	return a;
}
XYZ Ray::direction() const {
	return b;
}

XYZ Ray::to(const real t) const {
	return a + t * b;
}

