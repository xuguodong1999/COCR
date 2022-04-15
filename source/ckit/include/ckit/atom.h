#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include "base/element_type.h"
#include "base/atom_stereo.h"

#include <vector>
#include <memory>

class ELS_CKIT_EXPORT Atom {
    inline static const float sDefaultRadius = 100;
    id_type id;
    ElementType elementType;
    AtomStereo atomStereo;
    int charge;
    bool mIsImplicit;
    std::string name;
    bool mIsLeftToRight;
    std::vector<std::pair<float, std::shared_ptr<Bond>>> saBonds;
public:
    static float getDefaultRadius();

    Atom();
    Atom(const id_type &_id, const size_t &_atomicNumber);
    Atom(const id_type &_id, const ElementType &_element, const float &_x = 0, const float &_y = 0);
    Atom(const id_type &_id, const std::string &_name, const float &_x0 = 0, const float &_y0 = 0,
         const float &_x1 = 0, const float &_y1 = 0);
    //
    float x, y;
    float xx, yy, zz;
    float x0, y0, x1, y1;
    //
    const AtomStereo &getAtomStereo() const;
    void setAtomStereo(const AtomStereo &_atomStereo);
    //
    void setType(const ElementType &_type);
    const ElementType &getType() const;
    //
    const ElementType &getElementType() const;
    void setElementType(const ElementType &_elementType);
    //
    void setElementType(const std::string &_elementName);
    const std::string &getElementName() const;
    const std::string &getName() const;
    void setElementType(const size_t &_atomicNumber);
    size_t getAtomicNumber() const;
    //
    void setId(const id_type &_id);
    id_type getId();
    //
    void setIsLeftToRight(bool isLeftToRight);
    bool isLeftToRight() const;
    //
    bool isCoord2dEmbedded() const;
    void setCoord2d(const float &_x, const float &_y);
    void set2D(const float &_x, const float &_y);
    //
    void set3D(const float &_x, const float &_y, const float &_z);
    //
    void setCharge(int charge);
    int getCharge() const;
    //
    bool isImplicit() const;
    void setImplicit(bool _isImplicit = true);
    //
    float getMass() const;
    float getElectronNeg() const;
    float getRadius() const;
    int getCommonNebNum() const;
    //
    void clearSABonds();
    std::vector<std::pair<float, std::shared_ptr<Bond>>> & getSaBonds();
    void insertSuperAtomBonds(std::shared_ptr<Bond> _bond, const float &_offset);
    //
    bool isSuperAtom() const;
};
