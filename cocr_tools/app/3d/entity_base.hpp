#pragma once

#include <Qt3DCore/QEntity>

#include <limits>

namespace Qt3DRender {
    class QObjectPicker;

    class QPickEvent;
}

class View3DWidget;

enum class EntityType {
    AtomSphere, BondCylinder, ConeMesh
};

class BaseEntity : public Qt3DCore::QEntity {
Q_OBJECT

    Qt3DRender::QObjectPicker *mPicker;
    size_t mId;
    EntityType type;
    inline static View3DWidget *view3DWidget = nullptr;
public:
    static void SetView3DWidget(View3DWidget *_view3DWidget);

    inline static const size_t sAxisId = std::numeric_limits<size_t>::max();

    // 必须指定父节点，不手动管理 BaseEntity类的生命周期
    explicit BaseEntity(QNode *parent);

    ~BaseEntity();

    void setType(const EntityType &_type);

    void setId(const size_t &_id);

public slots:

    void onPicked(Qt3DRender::QPickEvent *event);
};