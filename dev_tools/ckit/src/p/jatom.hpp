#pragma once

#include <cocr_chem_export.h>
#include "base/cocr_types.hpp"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>


namespace cocr {
    class JMol;

    COCR_CHEM_EXPORT extern std::vector<std::string> ELEMENT_NAME_LIST;
    COCR_CHEM_EXPORT extern std::vector<float> ELEMENT_ELECTRON_NEG_LIST;
    COCR_CHEM_EXPORT extern std::vector<float> ELEMENT_MASS_LIST;
    COCR_CHEM_EXPORT extern std::vector<float> ELEMENT_RADIUS_LIST;
    COCR_CHEM_EXPORT extern std::unordered_map<std::string, ElementType> STR_ELEMENT_MAP;
    COCR_CHEM_EXPORT extern std::unordered_map<ElementType, int> ELEMENT_COMMON_NEB_NUM_MAP;

    using id_type = size_t;

    class JBond;

    class COCR_CHEM_EXPORT JAtom {
        ElementType type;
        int charge;
        inline static const float sDefaultRadius = 100;
        id_type id;
        bool mIsImplicit;
        std::string name;
        bool mIsLeftToRight;
        std::vector<std::pair<float, std::shared_ptr<JBond>>> saBonds;
    public:
        void clearSABonds();

        std::vector<std::pair<float, std::shared_ptr<JBond>>> &
        getSaBonds();

        void insertSuperAtomBonds(std::shared_ptr<JBond> _bond, const float &_offset);

        void setIsLeftToRight(bool isLeftToRight);

        bool isLeftToRight() const;

        void setCharge(int charge);

        void setType(const ElementType &_type);

        bool isSuperAtom() const;

        float x, y, xx, yy, zz;
        float x0, y0, x1, y1;

        static float getDefaultRadius();

        JAtom(const size_t &_id, const ElementType &_element, const float &_x = 0, const float &_y = 0);

        JAtom(const size_t &_id, const std::string &_name, const float &_x0 = 0, const float &_y0 = 0,
              const float &_x1 = 0, const float &_y1 = 0);

        bool isImplicit() const;

        void setImplicit(bool _isImplicit = true);

        void setId(const id_type &_id);

        id_type getId();

        const std::string &getName() const;

        const ElementType &getType() const;

        int getAtomicNumber() const;

        void set2D(const float &_x, const float &_y);

        void set3D(const float &_x, const float &_y, const float &_z);

        int getCharge() const;

        float getMass() const;

        float getElectronNeg() const;

        float getRadius() const;

        int getCommonNebNum() const;
    };

    COCR_CHEM_EXPORT const std::string &convertElementTypeToName(const ElementType &_type);

    COCR_CHEM_EXPORT ElementType convertNameToElementType(const std::string &_name);

    COCR_CHEM_EXPORT ColorName getAtomColor(const ElementType &_element);

    COCR_CHEM_EXPORT int getCommonNebNum(const ElementType &_type);
}