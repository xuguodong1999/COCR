#ifndef _CHEMDATA_H_
#define _CHEMDATA_H_

#include "fraction.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace cocr {
    class ChemistryData {
    public:
        enum class State {
            unknown = 0,
            gass = 1,
            liquid = 2,
            solid = 3,
            plasma = 4,
            metal = 100,
            salt = 101,
            insoluble = 200,
            soluble = 201,
            organic = 300
        };

        static ChemistryData &GetInstance() {
            static ChemistryData cd;
            return cd;
        }

        const std::vector<float> &mass;

        const std::vector<float> &getMass() const;

        const std::vector<float> &getElectronegativity() const;

        const std::vector<std::string> &getName() const;

        const std::vector<std::string> &getFormula() const;


        const std::vector<float> &electronegativity;
        const std::vector<std::string> &name;
        const std::vector<std::string> &formula;
        const std::unordered_map<std::string, std::vector<std::string>> &valence;
        std::unordered_map<std::string, State> state;

        const std::unordered_map<std::string, std::vector<std::string>> &getValence() const;

        const std::unordered_map<std::string, State> &getState() const;

        int getProtonNum(const std::string &elementName) {
            for (auto i = 0; i < name.size(); i++) {
                if (name[i] == elementName)return i;
            }
            return 0;
        }

        float getAtomMass(const std::string &elementName) {
            int protonName = getProtonNum(elementName);
            if (0 <= protonName && protonName < mass.size())return mass[protonName];
            return 0;
        }

        bool isElementNameRecorded(const std::string &elementName) {
            for (auto &i:name) {
                if (i == elementName)return true;
            }
            return false;
        }

        bool isMatterNameRecorded(const std::string &matterName) {
            for (auto &i:formula) {
                if (i == matterName)return true;
            }
            return false;
        }

        State getMatterState(const std::string &matterName) {
            auto entry = state.find(matterName);
            if (entry == state.end()) {
                return State::unknown;
            }
            return entry->second;
        }

        std::vector<cocr::frac> getMatterValence(const std::string &matterName) {
            std::vector<cocr::frac> result;
            auto entry = valence.find(matterName);
            if (entry == valence.end()) {
                return result;
            }
            auto &strValList = entry->second;
            frac tmp;
            for (auto &f:strValList) {
                tmp.fromString(f);
                result.push_back(tmp);
            }
            return result;// 数据量不大
        }

    private:
        ChemistryData();

        ~ChemistryData() {}

        ChemistryData(const ChemistryData &) = delete;

        const ChemistryData &operator=(const ChemistryData &) = delete;
    };
}
#endif //!_CHEMDATA_H_