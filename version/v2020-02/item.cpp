/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "item.h"
#include "geoutil.h"

#include <QPainter>
const char* Item::getClassName() { return "Item"; }

const char* AtomItem::getClassName() { return "Atom"; }

const char* BondItem::getClassName() { return "Bond"; }

const char* SingleBondItem::getClassName() { return "SingleBond"; }

const char* DoubleBondItem::getClassName() { return "DoubleBond"; }

const char* TripleBondItem::getClassName() { return "TripleBond"; }

const char* SolidBondItem::getClassName() { return "SolidBond"; }

const char* DashBondItem::getClassName() { return "DashBond"; }

Item::Item(QGraphicsItem* parent, const char* name)
	: QGraphicsPathItem(parent), isUpdated(false) {
	setObjectName(name);
	setPen(QPen(Qt::black, 5));
}

AtomItem::AtomItem(QGraphicsItem* parent, const char* name)
	: Item(parent, name) {
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

const QFont atomFont = QFont("Arial", StdAtomTextAvgWidth);

void AtomItem::addBond(const BondItem* b) {
	bitems.push_back(b);
}

void AtomItem::updatePath() {
	QPainterPath pth;
#ifdef MAKE_DATASET
	assert(false);
#else
	QPainterPath tmp;
	tmp.addText(0, 0, atomFont, objectName());
	QRectF&& rect = tmp.boundingRect();

	pth.addText(
		-rect.x() - rect.width() / 2,
		-rect.y() - rect.height() / 2,
		atomFont,
		objectName()
	);
#endif // MAKE_DATASET
	setPath(pth);

}

qreal AtomItem::getAverageWidth() const {
	QRectF&& rect = boundingRect();
	return (rect.width() + rect.height()) / 3;
}

const AtomItem* BondItem::from()const {
	assert(aitems.size() == 2);
	return aitems.at(0);
}

const AtomItem* BondItem::to()const {
	assert(aitems.size() == 2);
	return aitems.at(1);
}

void AtomItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "AtomItem" << objectName() << "::paint";

	updatePath();
	painter->drawPath(path());
}

QPainterPath AtomItem::shape() const {
	return QGraphicsPathItem::shape();
}

BondItem::BondItem(QGraphicsItem* parent, const char* name)
	: Item(parent, name) {
}

SingleBondItem::SingleBondItem(QGraphicsItem* parent, const char* name)
	: BondItem(parent, name) {
	aitems.resize(2, nullptr);
}

DoubleBondItem::DoubleBondItem(QGraphicsItem* parent, const char* name)
	: SingleBondItem(parent, name) {
}

TripleBondItem::TripleBondItem(QGraphicsItem* parent, const char* name)
	: SingleBondItem(parent, name) {
}

SolidBondItem::SolidBondItem(QGraphicsItem* parent, const char* name)
	: SingleBondItem(parent, name) {
}

DashBondItem::DashBondItem(QGraphicsItem* parent, const char* name)
	: SingleBondItem(parent, name) {
}

void BondItem::setFrom(const AtomItem* a) {
	aitems[0] = a;
}
void BondItem::setTo(const AtomItem* a) {
	aitems[1] = a;
}
void SingleBondItem::updatePath() {
	const QPointF& a = from()->pos(), & b = to()->pos();
	qreal len = GeoUtil::getDistance(a, b);
	qreal aWidth = from()->getAverageWidth(), bWidth = to()->getAverageWidth();
	QPainterPath pth;
#ifdef MAKE_DATASET
	const QPointF&& d = a - b;
	const QPointF&& v = GeoUtil::getVerticalUnitVec(d);
	double offset = tan(angle2rad(rBondOffsetAngle)) * GeoUtil::getLength(d);
	QPointF start = a + offset * (rand() % 2 - 1) * v,
		end = b + offset * (rand() % 2 - 1) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), a + start));
#else
	pth.moveTo(GeoUtil::reflect(a, aWidth / len - 1, b));
	pth.lineTo(GeoUtil::reflect(b, aWidth / len - 1, a));
#endif // MAKE_DATASET

	setPath(pth);
}

void DoubleBondItem::updatePath() {
	const QPointF& a = from()->pos(), & b = to()->pos();
	qreal len = GeoUtil::getDistance(a, b);
	qreal aWidth = from()->getAverageWidth(), bWidth = to()->getAverageWidth();

	const QPointF&& d = a - b;
	const QPointF&& v = GeoUtil::getVerticalUnitVec(d);
	double offset = tan(angle2rad(StdDoubleBondAngle)) * GeoUtil::getLength(d);
	QPainterPath pth;
#ifdef MAKE_DATASET
	double offset1 = tan(angle2rad(rBondAngle1)) * GeoUtil::getLength(d);
	double offset2 = tan(angle2rad(rBondAngle2)) * GeoUtil::getLength(d);
	QPointF start = a + (offset + offset1 * (rand() % 2 - 1)) * v,
		end = b + (offset + offset2 * (rand() % 2 - 1)) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), start));
	start = a - (offset + offset1 * (rand() % 2 - 1)) * v,
		end = b - (offset + offset2 * (rand() % 2 - 1)) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), start));
#else
	QPointF start = a + offset * v, end = b + offset * v;
	pth.moveTo(GeoUtil::reflect(start, aWidth / len - 1, end));
	pth.lineTo(GeoUtil::reflect(end, aWidth / len - 1, start));
	start = a - offset * v, end = b - offset * v;
	pth.moveTo(GeoUtil::reflect(start, aWidth / len - 1, end));
	pth.lineTo(GeoUtil::reflect(end, aWidth / len - 1, start));
#endif // MAKE_DATASET
	setPath(pth);
}

void TripleBondItem::updatePath() {
	const QPointF& a = from()->pos(), & b = to()->pos();
	qreal len = GeoUtil::getDistance(a, b);
	qreal aWidth = from()->getAverageWidth(), bWidth = to()->getAverageWidth();

	const QPointF&& d = a - b;
	const QPointF&& v = GeoUtil::getVerticalUnitVec(d);
	double offset = tan(angle2rad(StdTripleBondAngle)) * GeoUtil::getLength(d);
	QPainterPath pth;
#ifdef MAKE_DATASET
	double offset1 = tan(angle2rad(rBondAngle1)) * GeoUtil::getLength(d);
	double offset2 = tan(angle2rad(rBondAngle2)) * GeoUtil::getLength(d);
	QPointF start = a + (offset + offset1 * (rand() % 2 - 1)) * v,
		end = b + (offset + offset2 * (rand() % 2 - 1)) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), start));
	start = a - (offset + offset1 * (rand() % 2 - 1)) * v,
		end = b - (offset + offset2 * (rand() % 2 - 1)) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), start));
	start = a - (offset1 * (rand() % 2 - 1)) * v,
		end = b - (offset2 * (rand() % 2 - 1)) * v;
	pth.moveTo(GeoUtil::reflect(start, rAtomSide1Keep * (aWidth / len - 1), end));
	pth.lineTo(GeoUtil::reflect(end, rAtomSide2Keep * (aWidth / len - 1), start));
#else
	QPointF start = a + offset * v, end = b + offset * v;
	pth.moveTo(GeoUtil::reflect(start, aWidth / len - 1, end));
	pth.lineTo(GeoUtil::reflect(end, aWidth / len - 1, start));
	start = a - offset * v, end = b - offset * v;
	pth.moveTo(GeoUtil::reflect(start, aWidth / len - 1, end));
	pth.lineTo(GeoUtil::reflect(end, aWidth / len - 1, start));
	start = a, end = b;
	pth.moveTo(GeoUtil::reflect(start, aWidth / len - 1, end));
	pth.lineTo(GeoUtil::reflect(end, aWidth / len - 1, start));
#endif // MAKE_DATASET
	setPath(pth);
}

void SolidBondItem::updatePath() {
	const QPointF& aa = from()->pos(), & bb = to()->pos();
	qreal len = GeoUtil::getDistance(aa, bb);
	qreal aWidth = from()->getAverageWidth(), bWidth = to()->getAverageWidth();
	QPointF a = GeoUtil::reflect(aa, aWidth / len - 1, bb), b = GeoUtil::reflect(bb, aWidth / len - 1, aa);

	const QPointF&& d = a - b;
	const QPointF&& v = GeoUtil::getVerticalUnitVec(d);
	double offset = tan(angle2rad(StdWedgeBondAngle)) * GeoUtil::getLength(d);

#ifdef MAKE_DATASET
	double offset1 = tan(angle2rad(rBondAngle1)) * GeoUtil::getLength(d);
	double offset2 = tan(angle2rad(rBondAngle2)) * GeoUtil::getLength(d);
	QPointF b1 = b + (offset + offset1 * (rand() % 2 - 1)) * v,
		b2 = b - (offset + offset2 * (rand() % 2 - 1)) * v;
	QPainterPath pth;
	pth.moveTo(a);
	pth.lineTo(b1);
	pth.lineTo(b2);
	double step = 1 - rSolidBondDense;
	QPointF stepx = (a - b1) * step, stepy = (b2 - b1) * step;
	QPointF x, y; x = y = b1;
	for (double i = 0; i < 0.999; i += step) {
		x += stepx; y += stepy;
		pth.moveTo(x);
		pth.lineTo(y);
	}
#else
	QPointF b1 = b + v * offset, b2 = b - v * offset;
	QPainterPath pth;
	pth.moveTo(a);
	pth.lineTo(b1);
	pth.lineTo(b2);
	double step = 0.01;
	QPointF stepx = (a - b1) * step, stepy = (b2 - b1) * step;
	QPointF x, y; x = y = b1;
	for (double i = 0; i < 0.999; i += step) {
		x += stepx; y += stepy;
		pth.moveTo(x);
		pth.lineTo(y);
	}
#endif // MAKE_DATASET
	setPath(pth);
}

void DashBondItem::updatePath() {
	const QPointF& aa = from()->pos(), & bb = to()->pos();
	qreal len = GeoUtil::getDistance(aa, bb);
	qreal aWidth = from()->getAverageWidth(), bWidth = to()->getAverageWidth();
	QPointF a = GeoUtil::reflect(aa, aWidth / len - 1, bb), b = GeoUtil::reflect(bb, aWidth / len - 1, aa);

	const QPointF&& d = a - b;
	const QPointF&& v = GeoUtil::getVerticalUnitVec(d);
	double offset = tan(angle2rad(StdWedgeBondAngle)) * GeoUtil::getLength(d);

#ifdef MAKE_DATASET
	double offset1 = tan(angle2rad(rBondAngle1)) * GeoUtil::getLength(d);
	double offset2 = tan(angle2rad(rBondAngle2)) * GeoUtil::getLength(d);
	QPointF b1 = b + (offset + offset1 * (rand() % 2 - 1)) * v,
		b2 = b - (offset + offset2 * (rand() % 2 - 1)) * v;
	QPainterPath pth;
	pth.moveTo(b1);
	pth.lineTo(b2);
	double step = 1 - rDashBondDense;
	QPointF stepx = (a - b1) * step, stepy = (a - b2) * step;
	QPointF x(b1), y(b2);
	for (double i = 0; i < 0.999; i += step) {
		x += stepx; y += stepy;
		pth.moveTo(x);
		pth.lineTo(y);
	}
#else
	QPointF b1 = b + v * offset, b2 = b - v * offset;
	QPainterPath pth;
	pth.moveTo(b1);
	pth.lineTo(b2);
	double step = std::min(0.2, StdDashBondDense / len);
	QPointF stepx = (a - b1) * step, stepy = (a - b2) * step;
	QPointF x(b1), y(b2);
	for (double i = 0; i < 0.999; i += step) {
		x += stepx; y += stepy;
		pth.moveTo(x);
		pth.lineTo(y);
	}
#endif // MAKE_DATASET
	setPath(pth);
}

void SingleBondItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "SingleBondItem::paint";

	updatePath();
	painter->drawPath(path());
}

void DoubleBondItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "DoubleBondItem::paint";
	updatePath();
	painter->drawPath(path());
}

void TripleBondItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "TripleBondItem::paint";

	updatePath();
	painter->drawPath(path());
}

void SolidBondItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "SolidBondItem::paint";

	updatePath();
	painter->drawPath(path());
}

void DashBondItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//qDebug() << "DashBondItem::paint";

	updatePath();
	painter->drawPath(path());
}
