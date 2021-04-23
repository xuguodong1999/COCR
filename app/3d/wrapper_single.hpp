#ifndef _3D_SINGLE_WRAPPER_HPP_
#define _3D_SINGLE_WRAPPER_HPP_

#include "wrapper_base.hpp"

namespace Qt3DExtras {
    class QPhongMaterial;
}

class SingleWrapper : public BaseWrapper {
Q_OBJECT
protected:
    Qt3DExtras::QPhongMaterial *material;
public:
    SingleWrapper(Qt3DCore::QEntity *_root);

    void setColor(const QColor &_color) override;
};

#endif//_3D_SINGLE_WRAPPER_HPP_
