#pragma once
#include "wrapper_cylinder.hpp"
#include <QVector3D>
#include <QColor>
#include <QString>

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>


class MultiCylinderWrapper : public BaseWrapper {
Q_OBJECT
    std::vector<std::shared_ptr<CylinderWrapper>> cylinders;
    std::vector<Qt3DExtras::QPhongMaterial *> materials;
    static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy;
public:
    MultiCylinderWrapper(Qt3DCore::QEntity *_root, int _num = 1);

    void setColor(const QColor &_color) override;

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setDistance(const float &_distance);

    void setDirection(const QVector3D &_from, const QVector3D &_to,
                      const std::optional<QVector3D> &_norm = std::nullopt);

    void setRadius(const float &_radius);

    void setObjectName(const QString &_name) override;

    void setId(const size_t &_id) override;

private:
    void setTranslation(const QVector3D &_trans);

    void loopMultiWrappers(std::function<void(CylinderWrapper &)> _func);
};