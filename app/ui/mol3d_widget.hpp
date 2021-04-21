#ifndef _XGD_MOL3D_WIDGET_HPP_
#define _XGD_MOL3D_WIDGET_HPP_

#include "gesture_widget.hpp"
#include <QWidget>

class Mol3DWindow;

class Mol3DBuilder;
namespace Qt3DCore {
    class QEntity;
}
class WaitHintWidget;
namespace xgd {
    class JMol;
}
// Qt3DWindow 无法生成 QGestureEvent
class Mol3DWidget : public GestureWidget {
Q_OBJECT
    std::shared_ptr<xgd::JMol> mol, newMol;
    Mol3DWindow *window;
    QWidget *mol3DWindowContainer;
    WaitHintWidget *hintWidget;
    Qt3DCore::QEntity *root;
    Mol3DBuilder *builder;
public:
    Mol3DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

    void startWaitHint();

protected:
    void resizeEvent(QResizeEvent *e) override;

public slots:

    void endWaitHint();
//    bool eventFilter(QObject *watched, QEvent *e);
};

#endif//_XGD_MOL3D_WIDGET_HPP_
