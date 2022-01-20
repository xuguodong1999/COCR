#ifndef _3D_CONE_HPP_
#define _3D_CONE_HPP_

#include "wrapper_single.hpp"

#include <QVector3D>

namespace Qt3DExtras {
    class QConeMesh;
}

class ConeWrapper : public SingleWrapper {
Q_OBJECT
    Qt3DExtras::QConeMesh *cone;
public:
    ConeWrapper(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

private:
    void setHeight(const float &_height);
};

#endif//_3D_CONE_HPP_
