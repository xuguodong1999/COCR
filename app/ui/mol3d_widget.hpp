#ifndef _XGD_MOL3D_WIDGET_HPP_
#define _XGD_MOL3D_WIDGET_HPP_

#include "jmol.hpp"
#include "gesture_widget.hpp"
#include <QWidget>

class Mol3DWindow;

class Mol3DBuilder;
namespace Qt3DCore {
    class QEntity;
}
class WaitHintWidget;

// Qt3DWindow 无法生成 QGestureEvent
class Mol3DWidget : public GestureWidget {
Q_OBJECT
    std::shared_ptr<xgd::JMol> mol;
    Mol3DWindow *window;
    WaitHintWidget *hintWidget;
    Qt3DCore::QEntity *root;
    Mol3DBuilder *builder;
public:
    Mol3DWidget(QWidget *parent = nullptr, std::shared_ptr<xgd::JMol> _mol = nullptr);

    void syncMolToScene();

//    bool eventFilter(QObject *watched, QEvent *e);
};

#endif//_XGD_MOL3D_WIDGET_HPP_
