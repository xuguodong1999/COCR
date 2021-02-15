#include "hw_mol.hpp"
#include "mol_op.hpp"
#include "isomer.hpp"
#include "colors.hpp"
#include "polya.hpp"
#include "std_util.hpp"
#include "crnn_data.hpp"
#include "hw_data.hpp"
#include "soso_darknet.hpp"
#include "linetextdata.hpp"
#include <random>
#include <memory>
#include <QCoreApplication>
#include <opencv2/highgui.hpp>

#include <opencv2/imgproc.hpp>

extern std::string WORKSPACE;

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    try {
        srand(0633);
        CRNNDataGenerator generator;
        generator.init(WORKSPACE + "soso-text");
        generator.dump(1000000, 10000);
//        generator.display();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
