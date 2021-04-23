#include "ui/main_tab_widget.h"
#include "application.hpp"
#include "chem/jmol.hpp"
#include "chem/jmol_adapter.hpp"
#include <iostream>

int testMolFormat() {
    std::vector<std::string> formats = GetWritableFormats();
    std::vector<std::string> suffixVec;
    for (auto &format:formats) {
        if (std::string::npos != format.find("Read-only")) { continue; }
        suffixVec.push_back(format.substr(0, format.find(" ")));
        std::cout << "\"" << suffixVec.back() << "\",";
    }
    std::cout << std::endl;
    std::shared_ptr<xgd::JMol> mol = std::make_shared<xgd::JMolAdapter>();
    mol->readAsSMI("c1ccccc1");
    std::cout << "**********************" << std::endl;
    for (auto &format:suffixVec) {
        std::cout << format << std::endl;
        std::string content = mol->writeAs(format);
        if (content.length() > 10000) { content = "error"; }
        std::cout << content << std::endl;
        std::cout << "**********************" << std::endl;
    }
    return 0;
}

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    return testMolFormat();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
