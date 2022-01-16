#include "sphere.h"
Sphere::Sphere() {

}
Sphere::Sphere(XYZ _o, real _r, Mat* _m) : o(_o), r(_r), m(_m) {

}
bool Sphere::hitBy(const Ray& ray, Intersection& hit, real t_max, real t_min) const {
	//  (A+tB-C)^2=R^2
	XYZ&& A = ray.from(), && B = ray.direction();
	real a = dot(B, B);
	real b = 2.0 * dot(B, A - o);
	real c = dot(A - o, A - o) - r * r;
	real delta = b * b - 4 * a * c;
	if (delta < 0) {
		return false;
	}
	//靠得近的解优先
	real ans = (-b - sqrt(delta)) / (2.0 * a);
	if (t_min < ans && ans < t_max) {
		hit.t = ans;
		hit.p = ray.to(ans);
		hit.n = (hit.p - o).norm();
		hit.mat = m;
		return true;
	}
	ans = (-b + sqrt(delta)) / (2.0 * a);
	if (t_min < ans && ans < t_max) {
		hit.t = ans;
		hit.p = ray.to(ans);
		hit.n = (hit.p - o) / r;
		hit.mat = m;
		return true;
	}
	return false;
}
