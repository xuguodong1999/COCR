#include "xyz.h"
#include <iomanip>
#include <cassert>
#include <cstring>
using namespace std;
using namespace cg;
using namespace xyz;

XYZ::XYZ(real _x, real _y, real _z) {
	ptr[0] = _x;
	ptr[1] = _y;
	ptr[2] = _z;
}

XYZ::XYZ(const XYZ& _xyz) {
	memcpy(ptr, _xyz.ptr, sizeof(ptr));
}

namespace xyz {
	XYZ reflect(const XYZ& _in, const XYZ& _n) {
		XYZ&& n = norm(_n);
		//构造一个两倍于直角边的法线，做减法获得反射方向
		//这里的dot是带符号的，会调整法线方向，无所谓法线向内还是向外
		return (_in - 2 * dot(_in, n) * n).norm();
	}

	bool refract(const XYZ& _in, const XYZ& _n, XYZ& _out, const real _n2, const real _n1) {
		///////|
		///in/ | n
		/////  |
		////___|
		//   h
		XYZ&& n = norm(_n);
		XYZ&& in = norm(_in);		//归一化的入射光斜边
		real n1 = dot(in, n);		//斜边归一化后，法线直角边长度
		real k = _n1 / _n2;			//在斜边归一化时，k表示h1、h2的比值
		real n2 = 1.0 -				//按照n1*sin(a1)=n2*sin(a2)，如果全反射，那么折射光的h大于1
			k * k *					//折射光的h
			(1 - n1 * n1);			//入射光的h
		if (n2 < 0) {
			return false;
		}
		_out = (k * (in - n * n1)	//折射光一侧的h
			- n * sqrt(n2)).norm();	//折射光一侧的n
		return true;
	}

	XYZ randAtNormSphere() {
		//策略：方向随机+长度随机
		real d = cbrt(rand_0_1);
		return d * XYZ(2.0 * rand_0_1 - 1.0, 2.0 * rand_0_1 - 1.0, 2.0 * rand_0_1 - 1.0).norm();
	}

	XYZ randAtNormXY() {
		//策略：方向随机+长度随机
		real d = sqrt(rand_0_1);
		return d * XYZ(2.0 * rand_0_1 - 1.0, 2.0 * rand_0_1 - 1.0, 0).norm();
	}

	std::vector<XYZ> inverse3x3(const std::vector<XYZ> a) {
		assert(false);
		assert(a.size() == 3);
		vector<XYZ> result;
		result.resize(3, XYZ(0, 0, 0));

	}

	std::vector<XYZ> mul3x3(const std::vector<XYZ> a, const std::vector<XYZ> b) {
		assert(a.size() == 3 && b.size() == 3);
		vector<XYZ> result;
		result.resize(3, XYZ(0, 0, 0));
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					result[i][j] = a[i][k] * b[k][i];
				}
			}
		}
		return result;
	}

	XYZ mul3x1(const std::vector<XYZ> a, const XYZ b) {
		assert(a.size() == 3);
		XYZ result(0, 0, 0);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				result[i] += a[i][j] * b[j];
			}
		}
		return result;
	}

	XYZ cast(const XYZ& _from, real _k, const XYZ& _center) {
		return _center * (_k + 1) - _from * _k;
	}

	XYZ cast(const XYZ& _from, real _k, const XYZ& _p1, const XYZ& _p2) {
		if (_p1 == _p2) {
			return cast(_from, _k, (_p1 + _p2) / 2);
		}
		return cast(_from, _k, foot(_from, _p1, _p2));
	}

	XYZ cast(const XYZ& _from, real _k, const XYZ& _p1, const XYZ& _p2, const XYZ& _p3) {
		XYZ v1 = norm(_p1 - _p2), v2 = norm(_p2 - _p3);
		if (abs(v1) == abs(v2)) {
			return cast(_from, _k, (_p1 + _p3) / 2, _p2);
		}
		XYZ& n = cross(v1, v2).norm();
		real offset = dot(n, _p1 - _from) * (_k + 1);
		return n * offset + _from;
	}

	XYZ foot(const XYZ& _from, const XYZ& _p1, const XYZ& _p2) {
		if (_p1 == _p2) {
			return (_p1 + _p2) / 2;
		}
		real x1 = _p1.x(), y1 = _p1.y(), z1 = _p1.z(),
			x = _from.x(), y = _from.y(), z = _from.y(),
			dx = _p2.x() - x1, dy = _p2.y() - y1, dz = _p2.z() - z1;
		real m = (dx * (x1 - x) + dy * (y1 - y) + dz * (z1 - z)) / (-dx * dx - dy * dy - dz * dz);
		return XYZ(x1 + m * dx, y1 + m * dy, z1 + m * dz);
	}
};

const XYZ& XYZ::operator+() const {
	return *this;
}

XYZ XYZ::operator-() const {
	return XYZ(-ptr[0], -ptr[1], -ptr[2]);
}

real XYZ::operator[](int i) const {
	return ptr[i];
}

real& XYZ::operator[](int i) {
	return ptr[i];
}

real XYZ::x() const {
	return ptr[0];
}

real XYZ::y()const {
	return ptr[1];
}

real XYZ::z() const {
	return ptr[2];
}

real& XYZ::r() {
	return ptr[0];
}

real& XYZ::g() {
	return ptr[1];
}

real& XYZ::b() {
	return ptr[2];
}

istream& operator>>(istream& _in, XYZ& _t) {
	_in >> _t.ptr[0] >> _t.ptr[1] >> _t.ptr[2];
	return _in;
}

ostream& operator<<(ostream& _out, const XYZ& _t) {
	_out << setprecision(5) << "(" << _t.ptr[0] << "," << _t.ptr[1] << "," << _t.ptr[2] << ")";
	return _out;
}

XYZ operator+(const XYZ& _xyz0, const XYZ& _xyz) {
	return XYZ(_xyz0.ptr[0] + _xyz.ptr[0], _xyz0.ptr[1] + _xyz.ptr[1], _xyz0.ptr[2] + _xyz.ptr[2]);
}

XYZ operator-(const XYZ& _xyz0, const XYZ& _xyz) {
	return XYZ(_xyz0.ptr[0] - _xyz.ptr[0], _xyz0.ptr[1] - _xyz.ptr[1], _xyz0.ptr[2] - _xyz.ptr[2]);
}

XYZ operator*(const XYZ& _xyz0, const XYZ& _xyz) {
	return XYZ(_xyz0.ptr[0] * _xyz.ptr[0], _xyz0.ptr[1] * _xyz.ptr[1], _xyz0.ptr[2] * _xyz.ptr[2]);
}

XYZ operator/(const XYZ& _xyz0, const XYZ& _xyz) {
	return XYZ(_xyz0.ptr[0] / _xyz.ptr[0], _xyz0.ptr[1] / _xyz.ptr[1], _xyz0.ptr[2] / _xyz.ptr[2]);
}

XYZ operator*(real _t, const XYZ& _xyz) {
	return XYZ(_t * _xyz.ptr[0], _t * _xyz.ptr[1], _t * _xyz.ptr[2]);
}

XYZ operator/(XYZ _xyz, real _t) {
	return XYZ(_xyz.ptr[0] / _t, _xyz.ptr[1] / _t, _xyz.ptr[2] / _t);
}

XYZ operator*(const XYZ& _xyz, real _t) {
	return XYZ(_t * _xyz.ptr[0], _t * _xyz.ptr[1], _t * _xyz.ptr[2]);
}

real dot(const XYZ& _xyz0, const XYZ& _xyz) {
	return _xyz0.dot(_xyz);
}

XYZ cross(const XYZ& _xyz0, const XYZ& _xyz) {
	return _xyz0.cross(_xyz);
}

XYZ& XYZ::operator+=(const XYZ& _xyz) {
	ptr[0] += _xyz.ptr[0];
	ptr[1] += _xyz.ptr[1];
	ptr[2] += _xyz.ptr[2];
	return *this;
}

XYZ& XYZ::operator*=(const XYZ& _xyz) {
	ptr[0] *= _xyz.ptr[0];
	ptr[1] *= _xyz.ptr[1];
	ptr[2] *= _xyz.ptr[2];
	return *this;
}

XYZ& XYZ::operator/=(const XYZ& _xyz) {
	ptr[0] /= _xyz.ptr[0];
	ptr[1] /= _xyz.ptr[1];
	ptr[2] /= _xyz.ptr[2];
	return *this;
}

bool operator==(const XYZ& _xyz0, const XYZ& _xyz) {
	return fabs(_xyz0.x() - _xyz.x()) < cg::_EPS
		&& fabs(_xyz0.y() - _xyz.y()) < cg::_EPS
		&& fabs(_xyz0.z() - _xyz.z()) < cg::_EPS;
}

bool operator!=(const XYZ& _xyz0, const XYZ& _xyz) {
	return !(_xyz0 == _xyz);
}


XYZ& XYZ::operator-=(const XYZ& _xyz) {
	ptr[0] -= _xyz.ptr[0];
	ptr[1] -= _xyz.ptr[1];
	ptr[2] -= _xyz.ptr[2];
	return *this;
}

XYZ& XYZ::operator*=(const real _t) {
	ptr[0] *= _t;
	ptr[1] *= _t;
	ptr[2] *= _t;
	return *this;
}

XYZ& XYZ::operator/=(const real _t) {
	real k = 1.0 / _t;
	ptr[0] *= k;
	ptr[1] *= k;
	ptr[2] *= k;
	return *this;
}

XYZ& XYZ::norm() {
	*this /= length();
	return *this;
}

real XYZ::dot(const XYZ& _xyz) const {
	return x() * _xyz.x() + y() * _xyz.y() + z() * _xyz.z();
}

XYZ XYZ::cross(const XYZ& _xyz) const {
	return XYZ(
		ptr[1] * _xyz.ptr[2] - ptr[2] * _xyz.ptr[1],
		ptr[2] * _xyz.ptr[0] - ptr[0] * _xyz.ptr[2],
		ptr[0] * _xyz.ptr[1] - ptr[1] * _xyz.ptr[0]
	);
}

real XYZ::length() const {
	return sqrt(dot(*this));
}

XYZ& XYZ::abs() {
	ptr[0] = fabs(ptr[0]);
	ptr[1] = fabs(ptr[1]);
	ptr[2] = fabs(ptr[2]);
	return *this;
}

const real* XYZ::data() const {
	return ptr;
}

real len(const XYZ& _xyz) {
	return _xyz.length();
}

XYZ abs(const XYZ& _xyz) {
	return XYZ(fabs(_xyz.x()), fabs(_xyz.y()), fabs(_xyz.z()));
}

XYZ norm(const XYZ& _xyz) {
	return XYZ(_xyz).norm();
}
