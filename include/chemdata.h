#ifndef _CHEMDATA_H_
#define _CHEMDATA_H_

#include <vector>
#include <string>
#include <unordered_map>

namespace cocr {
    using namespace std;

    class ChemistryData {
    public:
        static ChemistryData &GetInstance() {
            static ChemistryData cd;
            return cd;
        }

        const vector<float> &mass;
        const vector<float> &electronegativity;
        const vector<string> &name;
        const vector<string> &formula;
        const vector<string> &valence;
    private:
        ChemistryData();

        ~ChemistryData() {}

        ChemistryData(const ChemistryData &) = delete;

        const ChemistryData &operator=(const ChemistryData &) = delete;
    };
}
#endif //!_CHEMDATA_H_