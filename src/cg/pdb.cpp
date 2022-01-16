#include "pdb.h"
#include "sphere.h"
#include "tube.h"
#include <sstream>
#include <cassert>
using namespace std;

static Glass glass(0.0, 0.0, 1.5);
static Cloth whitecloth(XYZ(0.5, 0.5, 0.5));
static Mirror mirror(XYZ(1.0, 1.0, 1.0), 0.0);
static Lamp lamp(XYZ(1, 1, 1));

map<string, unsigned int> SimplePdb::s2i = {
	make_pair("H",1),
	make_pair("C",6),
	make_pair("N",7),
	make_pair("O",8),
	make_pair("F",9),
	make_pair("P",15),
	make_pair("S",16),
	make_pair("Cl",17),
	make_pair("Br",35),
	make_pair("I",53)
};
istream& operator>>(istream& _in, SimplePdb& _data) {
	_data.clear();
	stringstream liness;
	string line, tmps, tmps2;
	int a1, a2;
	real f1, f2, f3;
	while (getline(_in, line)) {
		if (line.find("HETATM") != string::npos || line.find("ATOM") != string::npos) {
			liness.str(line);
			liness >> tmps >> tmps >> tmps;
			liness >> tmps2 >> tmps2;
			liness >> f1 >> f2 >> f3;
			_data.poses.emplace_back(XYZ(f1, f2, f3));
			_data.eles.emplace_back(tmps);
		}
		else if (line.find("CONECT") != string::npos) {
			liness.str(line);
			liness >> tmps >> a1 >> a2;
			do {
				if (a1 > a2) {
					_data.cons.insert(make_pair(a2, a1));
				}
				else {
					_data.cons.insert(make_pair(a1, a2));
				}
				liness >> a2;
			} while (!liness.eof());
			liness.clear();
		}
	}
	return _in;
}

SimplePdb::SimplePdb() {

}
SimplePdb::~SimplePdb() {

}
const vector<Sphere>& SimplePdb::getSpheres() {
	if (s.size() == 0) {
		generate();
	}
	return s;
}
const vector<Tube>& SimplePdb::getTubes() {
	if (c.size() == 0) {
		generate();
	}
	return c;
}

void SimplePdb::generate() {
	s.clear();
	c.clear();
	XYZ center(0, 0, 0);
	for (auto& i : poses)
		center += i;
	center /= poses.size();
	for (auto& i : poses)
		i -= center;
	assert(eles.size() == poses.size());
	for (size_t i = 0; i < eles.size(); i++) {
		if (s2i[eles[i]] == 1) {
			s.emplace_back(Sphere(poses[i], 0.3, &glass));
		}
		else {
			s.emplace_back(Sphere(poses[i], 0.3, &mirror));
		}
	}
	for (auto& i : cons)
		c.emplace_back(Tube(poses[i.first - 1], poses[i.second - 1], 0.07, &lamp));
}

void SimplePdb::clear() {
	s.clear();
	c.clear();
	eles.clear();
	poses.clear();
	cons.clear();
}