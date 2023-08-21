#include "XGD/Chem/EquationBalanceSolver.h"
#include "Equation/Equation.h"
#include "Equation/BalanceUtil.h"

std::optional<std::vector<int>> CebApi::balance(const std::string &equation) {
    Equation e;
    e.clear();
    bool result = e.exec(equation);
    if (!result) {
        return std::nullopt;
    }
    return e.getRatioList();
}

std::optional<std::string> CebApi::balanceAndRenderToHtml(const std::string &equation) {
    Equation e;
    e.clear();
    bool result = e.exec(equation);
    if (!result) {
        return std::nullopt;
    }
    return add_html(e.p());
}
