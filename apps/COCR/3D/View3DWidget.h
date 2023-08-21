#pragma once

#include <QWidget>

namespace Ui {
    class View3DWidget;
}
class GuiMol;

class Mol3DWidget;

class View3DWidget : public QWidget {
Q_OBJECT
public:
    explicit View3DWidget(QWidget *parent = nullptr);

    ~View3DWidget();

    void syncMolToScene(std::shared_ptr<GuiMol> _mol);

    void startWaitHint();

    void onAtomPicked(const size_t &_aid);

    void onBondPicked(const size_t &_bid);

private:
    Ui::View3DWidget *ui;
    Mol3DWidget *mol3DWidget;
    std::string currentFormat;
    int hyBtnClickTimes, expBtnClickTimes;
private Q_SLOTS:

    void showFormatDialog();

    void switchSuperAtomState();

    void reformatInputState();
};