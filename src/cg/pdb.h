#ifndef PDB_H
#define PDB_H
#include "cg.h"
#include "xyz.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <set>
class Sphere;
class Tube;
class Pdb {
public:

protected:
	std::vector<std::string> eles;
	std::vector<XYZ> poses;
	std::set<std::pair<int, int>> cons;
};
class SimplePdb :public Pdb {
public:
	SimplePdb();
	~SimplePdb();
	const std::vector<Sphere>& getSpheres();
	const std::vector<Tube>& getTubes();
	friend std::istream& operator>>(std::istream& _in, SimplePdb& _data);
private:
	void generate();
	void clear();
	std::vector<Sphere> s;
	std::vector<Tube> c;
	static std::map<std::string, unsigned int> s2i;
};
#endif // !PDB_H