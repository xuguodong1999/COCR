#pragma once

#include "element.h"
#include "matnode.h"
#include <string>
#include <vector>

class Matter {
public:
    Matter();

    Matter(std::string &_name);

    Matter(char *_name);

    ~Matter();

    std::string name;
    int amount;
    int q;
    char state;
    std::string stateInfo;

    void clear();

    void inputName();

    bool Exist();

    void getStateInfo();

    std::vector<Element> eleList;

    bool intoElement();

    void printElement();

    bool getValence();

//        void printValence();

    int getEleAmount(std::string &_element);

    int getEleIndex(std::string &_element);

    std::string p();

    double getRelativeMolecureMass();

private:
    bool preProcess(std::string &tar);

    bool CallastBlock(std::vector<MatNode> &matlist);

    bool CalaBlock(std::vector<MatNode> &matlist, int i, int j);

    bool lexicalAnalysis(std::vector<MatNode> &matlist, std::string &tar);

    void furtherLexicalAnalysis(std::vector<MatNode> &matlist);
};