#ifndef _MOL3D_BUILDER_HPP_
#define _MOL3D_BUILDER_HPP_

#include "mol3d_entity.hpp"

#include <QObject>
#include <QColor>
#include <QVector3D>

#include <unordered_map>
#include <memory>

namespace Qt3DCore {
    class QEntity;
}


namespace xgd {
    class JMol;
}


class Mol3DBuilder : public QObject {
Q_OBJECT

    Qt3DCore::QEntity *root, *molRoot, *axisRoot;
    std::shared_ptr<xgd::JMol> mol;
    Qt3DCore::QTransform *molRootTrans;
private:
    int sphereRings, sphereSlices, cylinderRings, cylinderSlices;
    float doubleBondSpaceScale, tripleBondSpaceScale;

    std::unordered_map<size_t, std::shared_ptr<SphereWrapper>> atoms;
    std::unordered_map<size_t, std::shared_ptr<BaseWrapper>> bonds;
public:
    Qt3DCore::QTransform *getMolRootTrans() const;

    Qt3DCore::QEntity *getMolRoot() const;

    Qt3DCore::QEntity *getAxisRoot() const;

    Mol3DBuilder(QObject *parent, Qt3DCore::QEntity *_root);

    void prepare(std::shared_ptr<xgd::JMol> _mol, const QVector3D &_viewSize,
                 const QVector3D &_blankArea = {5, 5, 5});

signals:

    void sig_mol_build_done();

    void sig_mol_prepare_done();

public slots:

    void build();

private:
    void buildAxis(const float &_x = 0, const float &_y = 0, const float &_z = 0, const float &_l = 100);
};

#endif//_MOL3D_BUILDER_HPP_
