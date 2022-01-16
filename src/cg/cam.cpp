#include "cam.h"
#include "mat.h"
using namespace std;
Cam::Cam(
	XYZ _eye,
	XYZ _scene,
	XYZ _hair,
	real _viewInDegree,
	real _w,
	real _h
) {
	eye = _eye;
	in_n = norm(_eye - _scene);
	XYZ _up_n = norm(_hair);				//头顶方向
	XYZ _right_n = cross(_up_n, in_n);		//右手方向

	real _h_2 = tan(angle2rad(_viewInDegree) / 2);
	real h = 2 * _h_2;							//成像平面高
	real w = h / _h * _w;						//成像平面宽
	right = _right_n * w;
	up = _up_n * h;
	//leftButtom = - right / 2.0 - up / 2.0;
	leftButtom = -right / 2.0 - up / 2.0 - in_n;
	//look(leftButtom);
}

Ray Cam::seeAt(real i, real j) {
	//return Ray(eye,-in_n + leftButtom + s * right + t * up);
	return Ray(eye, leftButtom + i * right + j * up);
}

XYZ Cam::collect(const Ray& in, CurveGroup& world, int depth) {
	Intersection hit;
	if (world.hitBy(in, hit, cg::_MAX_REAL, 0.000001)) {
		Ray out;
		XYZ loss;
		XYZ emit = hit.mat->emit();
		if (depth < 200 && hit.mat->reflect(in, hit, loss, out)) {
			return emit + loss * collect(out, world, depth + 1);
		}
		else {
			return emit;
		}
	}
	else {
		//return XYZ(0, 0, 0);//无背景光
		real t = 0.5 *(norm(in.direction()).z() + 1.0);
		return 0.9*((1.0 - t) * XYZ(1, 1, 1) + t * XYZ(0.8, 0.9, 1.0));
	}
}
