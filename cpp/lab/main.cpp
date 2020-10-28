//#include <vtkSphere.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
//#include <torch/torch.h>
//#include <QDebug>
//#include <Eigen/Eigen>
//#include <fdeep/fdeep.hpp>
//#include <nlohmann/json.hpp>
//#include <rapidjson/document.h>
//#include <taskflow/taskflow.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <coordgen/sketcherMinimizer.h>
#include <iostream>

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/MolOps.h>

#ifdef _DEBUG
RDLogger rdErrorLog;
#endif

void hello_sssr() {
    std::shared_ptr<RDKit::ROMol> mol(RDKit::SmilesToMol("c1ccc(C)cc1"));
    if (!mol->getRingInfo()->isInitialized()) {
        RDKit::MolOps::findSSSR(*mol);
    }
    for (unsigned int i = 0; i < mol->getNumAtoms(); ++i) {
        const RDKit::Atom *atom = mol->getAtomWithIdx(i);
        std::cout << mol->getRingInfo()->numAtomRings(atom->getIdx()) << " ";
    }
    std::cout << std::endl;

    for (unsigned int i = 0; i < mol->getNumBonds(); ++i) {
        const RDKit::Bond *bond = mol->getBondWithIdx(i);
        std::cout << mol->getRingInfo()->numBondRings(bond->getIdx()) << " ";
    }
    std::cout << std::endl;

    const RDKit::Bond *bond = mol->getBondWithIdx(1);
    if (mol->getRingInfo()->numBondRings(bond->getIdx())) {
        std::cout << "Bond " << bond->getIdx() << " is in a ring" << std::endl;;
    }
    RDKit::VECT_INT_VECT rings;
    RDKit::MolOps::symmetrizeSSSR(*mol, rings);
    std::cout << "Number of symmetric SSSR rings : " << rings.size() << std::endl;
    for (auto ring: rings) {
        for (auto ringat: ring) {
            std::cout << ringat << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Number of SSSR rings : " << RDKit::MolOps::findSSSR(*mol) << std::endl;
}

int main() {
    hello_sssr();
//    torch::Tensor x = torch::tensor(0.5);
//    std::cout << torch::sigmoid(x) << std::endl;
    RDKit::RWMol mol;
    sketcherMinimizer minimizer;
    /* create a molecule */
    auto *min_mol = new sketcherMinimizerMolecule();
    /* add an atom and set its parameters */
    const size_t num = 5;
    sketcherMinimizerAtom *a[num];
    sketcherMinimizerBond *b[num];
    for (int i = 0; i < num - 1; i++) {
        a[i] = min_mol->addNewAtom();
        a[i]->setAtomicNumber(1);
    }
    a[num - 1] = min_mol->addNewAtom();
    a[num - 1]->setAtomicNumber(6);
    for (int i = 0; i < num - 1; i++) {
        b[i] = min_mol->addNewBond(a[i % num], a[num - 1]);
        b[i]->setBondOrder(1);
    }
    /* load minimizer */
    minimizer.initialize(min_mol);
    /* generate coordinates */
    minimizer.runGenerateCoordinates();
    /* print coordinates */
    std::cout << "scatter([";
    for (int i = 0; i < num; i++) {
        std::cout << a[i]->getCoordinates().x() << ",";
    }
    std::cout << "],[";
    for (int i = 0; i < num; i++) {
        std::cout << a[i]->getCoordinates().y() << ",";
    }
    std::cout << "],'K*');\naxis([-5,5,-5,5]);\ngrid on;" << std::endl;
    exit(0);
}