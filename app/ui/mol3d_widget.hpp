#ifndef _XGD_MOL3D_WIDGET_HPP_
#define _XGD_MOL3D_WIDGET_HPP_

#include "jmol.hpp"
#include <QWidget>

class Mol3DWindow;

class Mol3DBuilder;
namespace Qt3DCore {
    class QEntity;
}
class Mol3DWidget : public QWidget {
Q_OBJECT
    std::shared_ptr<xgd::JMol> mol;
    Mol3DWindow *window;
    Qt3DCore::QEntity *root;
    Mol3DBuilder *builder;
public:
    Mol3DWidget(QWidget *parent = nullptr, std::shared_ptr<xgd::JMol> _mol = nullptr);

    void syncMolToScene();
};

#endif//_XGD_MOL3D_WIDGET_HPP_
