#pragma once

#include "WrapperSingle.h"

namespace Qt3DExtras {
    class QSphereMesh;
}

class SphereWrapper : public SingleWrapper {
Q_OBJECT

    Qt3DExtras::QSphereMesh *sphere;
public:
    SphereWrapper(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;
};