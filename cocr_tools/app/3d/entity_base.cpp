#include "3d/entity_base.hpp"
#include "3d/view3d_widget.h"

#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

BaseEntity::BaseEntity(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent) {
    mPicker = new Qt3DRender::QObjectPicker(this);
    mPicker->setHoverEnabled(false);
    mPicker->setDragEnabled(false);
    mPicker->setEnabled(true);
    addComponent(mPicker);
    connect(mPicker, &Qt3DRender::QObjectPicker::clicked, this, &BaseEntity::onPicked);
}

void BaseEntity::onPicked(Qt3DRender::QPickEvent *event) {
    if (mId == sAxisId) {
        qDebug() << "axis picked";
        return;
    }
    switch (type) {
        case EntityType::AtomSphere: {
            qDebug() << "atom " << mId << " picked";
            if (view3DWidget) {
                view3DWidget->onAtomPicked(mId);
            }
            break;
        }
        case EntityType::BondCylinder: {
            qDebug() << "bond " << mId << " picked";
            if (view3DWidget) {
                view3DWidget->onBondPicked(mId);
            }
            break;
        }
        default:
            break;
    }
}

void BaseEntity::setType(const EntityType &_type) {
    type = _type;
}

void BaseEntity::setId(const size_t &_id) {
    mId = _id;
}

BaseEntity::~BaseEntity() {
//    qDebug() << "~BaseEntity()";
}

void BaseEntity::SetView3DWidget(View3DWidget *_view3DWidget) {
    view3DWidget = _view3DWidget;
}

