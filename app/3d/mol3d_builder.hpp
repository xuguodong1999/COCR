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
    Qt3DCore::QEntity *root;
    std::shared_ptr<xgd::JMol> mol;
    int sphereRings, sphereSlices, cylinderRings, cylinderSlices;
    float doubleBondSpaceScale, tripleBondSpaceScale;

    std::unordered_map<size_t, std::shared_ptr<SphereEntity>> atoms;
    std::unordered_map<size_t, std::shared_ptr<BaseEntity>> bonds;
public:
    Mol3DBuilder(QObject *parent, Qt3DCore::QEntity *_root) : QObject(parent), root(_root) {}

    void prepare(std::shared_ptr<xgd::JMol> _mol, const QVector3D &_viewSize,
                 const QVector3D &_blankArea = {5, 5, 5});

signals:

    void on_mol_build_done();

    void on_mol_prepare_done();

public slots:

    void build();
};

#endif//_MOL3D_BUILDER_HPP_
