#include "math_util.hpp"
#include <cmath>

using namespace xgd;

QPointF MathUtil::getZero() { return QPointF(0, 0); }

QPointF MathUtil::getOneX() { return QPointF(1, 0); }

QPointF MathUtil::getOneY() { return QPointF(0, 1); }

QVector3D MathUtil::getZero3() { return QVector3D(0, 0, 0); }

QVector3D MathUtil::getOneX3() { return QVector3D(1, 0, 0); }

QVector3D MathUtil::getOneY3() { return QVector3D(0, 1, 0); }

QVector3D MathUtil::getOneZ3() { return QVector3D(0, 0, 1); }

qreal MathUtil::getLength(const QPointF &vec) { return sqrt(vec.x() * vec.x() + vec.y() * vec.y()); }

QPointF MathUtil::normalize(const QPointF &vec) {
    qreal len = getLength(vec);
    if (qFuzzyIsNull(len)) {
        return getZero();
    }
    return vec / len;
}

QPointF MathUtil::getVerticalUnitVec(const QPointF &vec) {
    if (vec == getZero()) {
        return getZero();
    } else if (qFuzzyIsNull(vec.y())) {
        return getOneY();
    }
    return normalize(QPointF(1, -vec.x() / vec.y()));
}

qreal MathUtil::getDistance(const QPointF &p1, const QPointF &p2) {
    qreal x = p1.x() - p2.x(), y = p1.y() - p2.y();
    return sqrt(x * x + y * y);
}

qreal MathUtil::getDistance(const QPointF &p1, const QVector3D &p2) {
    qreal x = p1.x() - p2.x(), y = p1.y() - p2.y();
    return sqrt(x * x + y * y);
}

qreal MathUtil::getDistance(const QVector3D &p1, const QPointF &p2) {
    qreal x = p1.x() - p2.x(), y = p1.y() - p2.y();
    return sqrt(x * x + y * y);
}

qreal MathUtil::getDistance(const QVector3D &p1, const QVector3D &p2) {
    return p1.distanceToPoint(p2);
}

qreal MathUtil::getDistance(const QPointF &p, const QPointF &p1, const QPointF &p2) {
    if (p1 == p2) {
        return getDistance(p, (p1 + p2) / 2);
    }
    QPointF line = p1 - p2, hypotenuse = p - p1;
    return QVector3D::crossProduct(
            QVector3D(p.x(), p.y(), 0),
            QVector3D(hypotenuse.x(), hypotenuse.y(), 0)
    ).length() / getLength(line);
}

bool MathUtil::equal(const QVector3D &p, const QVector3D &q) {
    return qFuzzyIsNull(p.x() - q.x()) && qFuzzyIsNull(p.y() - q.y()) && qFuzzyIsNull(p.z() - q.z());
}

bool MathUtil::notEqual(const QVector3D &p, const QVector3D &q) {
    return !equal(p, q);
}

QPointF MathUtil::reflect(const QPointF &p, qreal k, const QPointF &p1) {
    return p1 * (k + 1) - p * k;
}

QVector3D MathUtil::reflect(const QVector3D &p, qreal k, const QVector3D &p1) {
    return p1 * (k + 1) - p * k;
}

QPointF MathUtil::getFootPoint(const QPointF &p, const QPointF &p1, const QPointF &p2) {
    if (p1 == p2) {
        return (p1 + p2) / 2;
    }
    qreal x1 = p1.x(), y1 = p1.y(),
            x = p.x(), y = p.y(),
            dx = p2.x() - x1, dy = p2.y() - y1;
    qreal m = (dx * (x1 - x) + dy * (y1 - y)) / (-dx * dx - dy * dy);
    return QPointF(x1 + m * dx, y1 + m * dy);
}

QVector3D MathUtil::getFootPoint(const QVector3D &p, const QVector3D &p1, const QVector3D &p2) {
    if (equal(p1, p2)) {
        return (p1 + p2) / 2;
    }
    qreal x1 = p1.x(), y1 = p1.y(), z1 = p1.z(),
            x = p.x(), y = p.y(), z = p.y(),
            dx = p2.x() - x1, dy = p2.y() - y1, dz = p2.z() - z1;
    qreal m = (dx * (x1 - x) + dy * (y1 - y) + dz * (z1 - z)) / (-dx * dx - dy * dy - dz * dz);
    return QVector3D(x1 + m * dx, y1 + m * dy, z1 + m * dz);
}

QPointF MathUtil::reflect(const QPointF &p, qreal k, const QPointF &p1, const QPointF &p2) {
    if (p1 == p2) {
        return reflect(p, k, (p1 + p2) / 2);
    }
    return reflect(p, k, getFootPoint(p, p1, p2));
}

QVector3D MathUtil::reflect(const QVector3D &p, qreal k, const QVector3D &p1, const QVector3D &p2) {
    if (equal(p1, p2)) {
        return reflect(p, k, (p1 + p2) / 2);
    }
    return reflect(p, k, getFootPoint(p, p1, p2));
}

QVector3D
MathUtil::reflect(const QVector3D &p, qreal k, const QVector3D &p1, const QVector3D &p2, const QVector3D &p3) {
    QVector3D v1 = (p1 - p2).normalized(), v2 = (p2 - p3).normalized();
    if (equal(v1, v2) || equal(-v1, v2)) {
        return reflect(p, k, (p1 + p3) / 2, p2);
    }
    QVector3D n = QVector3D::crossProduct(v1, v2).normalized();
    qreal offset = QVector3D::dotProduct(n, p1 - p) * (k + 1);
    return n * offset + p;
}