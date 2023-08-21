#pragma once

#include <QObject>
#include <QColor>
#include <QtGui/QVector3D>

#include <unordered_map>
#include <memory>

namespace Qt3DCore {
    class QTransform;

    class QEntity;
}

class GuiMol;


class SphereWrapper;

class BaseWrapper;

class Mol3DBuilder : public QObject {
Q_OBJECT

    Qt3DCore::QEntity *root, *molRoot, *axisRoot;
    std::shared_ptr<GuiMol> mol;
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

    void prepare(std::shared_ptr<GuiMol> _mol, const QVector3D &_viewSize,
                 const QVector3D &_blankArea = {5, 5, 5});

    void resetMolRoot();

Q_SIGNALS:

    void sig_mol_build_done();

    void sig_mol_prepare_done();

public Q_SLOTS:

    void build();

private:
    void buildAxis(const float &_x = 0, const float &_y = 0, const float &_z = 0, const float &_l = 100);
};