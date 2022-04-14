#include "base/material_type.h"
#include "d/d_material_state_info.h"
#include "d/d_material_name.h"
#include "d/d_material_valence.h"
#include "base/std_util.h"

bool MaterialUtil::Exist(const std::string &name) {
    auto it = KNOWN_MATTER_SET.find(name);
    return it != KNOWN_MATTER_SET.end();
}

std::unordered_map<std::string, std::vector<frac>> KNOWN_VAL_FRAC_MAP;


static std::optional<frac> to_frac(const std::string &_src) {
    auto vec = StdUtil::split(_src, "/");
    if (vec.size() != 2) {
        return std::nullopt;
    }
    return frac{std::stoi(vec[0].begin()), std::stoi(vec[1].begin())};
}
static const std::vector<frac>EMPTY_VALENCE;
static const std::string EMPTY_STATE_INFO;
const std::vector<frac> &MaterialUtil::QueryValence(const std::string &name) {
    auto it0 = KNOWN_VAL_FRAC_MAP.find(name);
    if (it0 != KNOWN_VAL_FRAC_MAP.end()) {
        return it0->second;
    }
    auto it = KNOWN_VAL_MAP.find(name);
    if (it == KNOWN_VAL_MAP.end()) {
        return EMPTY_VALENCE;
    }
    const auto &stringData = it->second;
    std::vector<frac> data(stringData.size());
    for (size_t i = 0; i < stringData.size(); i++) {
        auto num = to_frac(stringData[i]);
        if (!num) {
            return EMPTY_VALENCE;
        }
        data[i] = num.value();
    }
    auto[it2, success] = KNOWN_VAL_FRAC_MAP.insert({name, std::move(data)});
    if (!success) {
        return EMPTY_VALENCE;
    }
    return it2->second;
}

const std::string &MaterialUtil::QueryStateInfo(const std::string &name) {
    auto it = KNOWN_STATE_INFO.find(name);
    if (it == KNOWN_STATE_INFO.end()) {
        return EMPTY_STATE_INFO;
    }
    return it->second;
}
