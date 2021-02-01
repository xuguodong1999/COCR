#ifndef CYLINDER_H
#define CYLINDER_H
#include "cg.h"
#include "xyz.h"
#include "curve.h"
#include "mat.h"
#include "sphere.h"
#include <vector>
class Tube : public Curve {
public:
	Tube(XYZ _o1, XYZ _o2, real _r, Mat* _m);
	//原定解法：
	//以圆柱底心为原点、高为Z轴建系XYZ'
	//从XYZ、XYZ'各取3个点，算变换矩阵/四元数，把坐标映射到新的坐标系
	//把直线投影到平面Z=0和Z=h，计算直线和圆截面的交点，检验结果在不在定义域内

	//实际解法：
	//用一串小球模拟圆柱,在线段o1o2上间隔d创建一串小球，算直线和小球串的交点
	bool hitBy(const Ray& ray, Intersection& hit, real t_max = cg::_MAX_REAL, real t_min = cg::_MIN_REAL) const override;
private:
	Tube();
	XYZ o1, o2;		//两个底面中心
	real r;			//半径
	Mat* m;			//材质
	std::vector<Sphere> ss;
};

#endif // !CYLINDER_H
