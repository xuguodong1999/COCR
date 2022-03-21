#pragma once

#include "3d/wrapper_single.hpp"

namespace Qt3DExtras {
    class QCylinderMesh;
}

class CylinderWrapper : public SingleWrapper {
Q_OBJECT

    friend class MultiCylinderWrapper;

    Qt3DExtras::QCylinderMesh *cylinder;
public:
    CylinderWrapper(Qt3DCore::QEntity *_root);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setRadius(const float &_radius);

private:
    void setLength(const float &_length);
};