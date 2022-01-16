#ifndef BIMAP_H
#define BIMAP_H

#include <unordered_map>
#include <map>
#include <unordered_set>

/** 代码出处： https://github.com/Mazyod/Bimap
*/
template<typename KeyType, typename ValueType>
class BiMap {
public:
    inline BiMap() {}

    inline ~BiMap() {
        kMap.clear();
        vMap.clear();
    }

    using Keys = std::unordered_set<KeyType>;
    using KeyMap = std::unordered_map<KeyType, ValueType>;
    using ValueMap = std::map<ValueType, Keys>;
    using ValueMapEntry = std::pair<ValueType, Keys>;

    inline void set(const KeyType &key, const ValueType &value) {
        kMap.insert(std::make_pair(key, value));
        vMap[value].insert(key);
    }

    inline Keys &getKeys(const ValueType &value) {
        return vMap.at(value);
    }

    inline ValueType &getValue(const KeyType &key) {
        return kMap.at(key);
    }

    inline bool removeKey(const KeyType &key) {
        auto has = hasKey(key);
        if (has) {
            auto &value = getValue(key);
            auto &keys = getKeys(value);
            keys.erase(key);
            if (keys.empty()) {
                vMap.erase(value);
            }
            kMap.erase(key);
        }
        return has;
    }

    inline bool removeValue(const ValueType &value) {
        auto has = hasValue(value);
        if (has) {
            auto &keys = getKeys(value);
            for (auto item : keys) {
                kMap.erase(item);
            }
            vMap.erase(value);
        }
        return has;
    }

    inline bool hasKey(const KeyType &key) const {
        return kMap.find(key) != kMap.end();
    }

    inline bool hasValue(const ValueType &value) const {
        return vMap.find(value) != vMap.end();
    }

    inline unsigned long size() const {
        return kMap.size();
    }

    inline const ValueMap &valueMap() const {
        return vMap;
    };

    inline const KeyMap &keyMap() const {
        return kMap;
    };
    KeyMap kMap;
    ValueMap vMap;
};

#endif // BIMAP_H
