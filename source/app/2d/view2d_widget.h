#pragma once

#include <QWidget>

namespace Ui {
    class View2DWidget;
}
class GuiMol;

class Mol2DWidget;

class View2DWidget : public QWidget {
Q_OBJECT
public:
    explicit View2DWidget(QWidget *parent = nullptr);

    ~View2DWidget();

    void syncMolToScene(std::shared_ptr<GuiMol> _mol);

    void startWaitHint();

    void onAtomPicked(const size_t &_aid);

    void onBondPicked(const size_t &_bid);

private:
    Ui::View2DWidget *ui;
    Mol2DWidget *mol2DWidget;
    std::string currentFormat;
    int hyBtnClickTimes, expBtnClickTimes;
private slots:

    void showFormatDialog();

    void switchHydrogenState();

    void switchSuperAtomState();

    void reformatInputState();
};