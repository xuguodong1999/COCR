#pragma once

 #include "ckit/config.h"
#include "base/cocr_types.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>


namespace v1_1 {
    class JMol;

    class JBond;

    class JAtom {
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
}