#ifndef CAM_H
#define CAM_H
#include "cg.h"
#include "Ray.h"
#include "xyz.h"
#include "curve.h"
class Cam {
public:
	Cam(
		XYZ _eye,			//光线汇聚点
		XYZ _scene,			//聚焦于
		XYZ _hair,			//头顶方位
		real _viewInDegree, //视角大小，控制成像平面和光线汇聚点的距离
		real _w,			//场景宽
		real _h				//场景高
	);
	Ray seeAt(real i, real j);//发出观察线
	XYZ collect(const Ray& in, CurveGroup& world, int depth);//收集光线积累
private:
	XYZ eye;
	XYZ leftButtom;
	XYZ right, up, in_n;
};



#endif // !CAM_H
