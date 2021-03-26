#ifndef _XGD_MOL3D_WIDGET_HPP_
#define _XGD_MOL3D_WIDGET_HPP_

#include "jmol.hpp"
#include <QWidget>

class Mol3DWindow;

class Mol3DBuilder;
namespace Qt3DCore {
    class QEntity;
}
class WaitHintWidget;

class QGestureEvent;

// TODO: 封装一套拦截 QGestureEvent 的 QWidget 子类，暴露出关键事件的回调，2D、3D主窗体都有手势需求
// TODO: 3DWindow 职责缩减，交互接口向外暴露，自己的事件分发仅仅处理3D相关的组件选中事
class Mol3DWidget : public QWidget {
Q_OBJECT
    std::shared_ptr<xgd::JMol> mol;
    Mol3DWindow *window;
    WaitHintWidget *hintWidget;
    Qt3DCore::QEntity *root;
    Mol3DBuilder *builder;
public:
    Mol3DWidget(QWidget *parent = nullptr, std::shared_ptr<xgd::JMol> _mol = nullptr);

    void syncMolToScene();
};

#endif//_XGD_MOL3D_WIDGET_HPP_
