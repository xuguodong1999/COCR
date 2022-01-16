#include "CImg.h"
#include "curve.h"
#include "sphere.h"
#include "tube.h"
#include "xyz.h"
#include "cg.h"
#include "cam.h"
#include "pdb.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using namespace cimg_library;

int main() {
	SimplePdb sp;
	cout << "input pdb path(../C6H12.pdb, e.g.):";
	string path="C:/Users/xgd/source/repos/rendering-hw2-171860633/C5H12.pdb";
	cin >> path;
	ifstream ifsm(path);
	if (!ifsm.is_open()) {
		cout << path << " not found!" << endl;
		exit(-1);
	}
	ifsm >> sp;

	vector<const Curve*> world;
	for (auto& i : sp.getSpheres())
		world.push_back(&i);
	for (auto& i : sp.getTubes())
		world.push_back(&i);

	//const int w = 1000, h = 800, times = 10;
	const int w = 1920/2, h = 1080/2, times = 50;
	Cam cam(XYZ(10, 0, 5), XYZ(0, 0, 0), XYZ(0, 0, 1), 45, w, h);
	CurveGroup cg(world);

	CImg<UINT8> img(w, h, 1, 3);
	XYZ tmp[times];
	cimg_forXY(img, i, j) {
		XYZ col(0, 0, 0);
		#pragma omp parallel for
		for (int s = 0; s < times; s++) {
			Ray r = cam.seeAt((i + rand_0_1) / w, (j + rand_0_1) / h);
			tmp[s] = cam.collect(r, cg, 0);
		}
		for (int s = 0; s < times; s++)
			col += tmp[s];
		col /= real(times);
		img(i, h - j - 1, 0) = col.r() * 255;
		img(i, h - j - 1, 1) = col.g() * 255;
		img(i, h - j - 1, 2) = col.b() * 255;
	}
	img.display("image");
	std::cout << "done." << std::endl;
	return 0;
}
