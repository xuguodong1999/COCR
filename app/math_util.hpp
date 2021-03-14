#ifndef _XGD_MATH_UTIL_HPP_
#define _XGD_MATH_UTIL_HPP_

#include <QVector3D>
#include <QPointF>

namespace xgd {
    class MathUtil {
        static QPointF getZero();

        static QPointF getOneX();

        static QPointF getOneY();

        static QVector3D getZero3();

        static QVector3D getOneX3();

        static QVector3D getOneY3();

        static QVector3D getOneZ3();

        static qreal getLength(const QPointF &vec);

        static QPointF normalize(const QPointF &vec);

        static QPointF getVerticalUnitVec(const QPointF &vec);

        static qreal getDistance(const QPointF &p1, const QPointF &p2);

        static qreal getDistance(const QPointF &p1, const QVector3D &p2);

        static qreal getDistance(const QVector3D &p1, const QPointF &p2);

        static qreal getDistance(const QVector3D &p1, const QVector3D &p2);

        static qreal getDistance(const QPointF &p, const QPointF &p1, const QPointF &p2);

        static bool equal(const QVector3D &p, const QVector3D &q);

        static bool notEqual(const QVector3D &p, const QVector3D &q);

        static QPointF reflect(const QPointF &p, qreal k, const QPointF &p1);

        static QVector3D reflect(const QVector3D &p, qreal k, const QVector3D &p1);

        static QPointF getFootPoint(const QPointF &p, const QPointF &p1, const QPointF &p2);

        static QVector3D getFootPoint(const QVector3D &p, const QVector3D &p1, const QVector3D &p2);

        static QPointF reflect(const QPointF &p, qreal k, const QPointF &p1, const QPointF &p2);

        static QVector3D reflect(const QVector3D &p, qreal k, const QVector3D &p1, const QVector3D &p2);

        static QVector3D
        reflect(const QVector3D &p, qreal k, const QVector3D &p1, const QVector3D &p2, const QVector3D &p3);

    private:
        MathUtil() = default;
    };
}
#endif//_XGD_MATH_UTIL_HPP_
