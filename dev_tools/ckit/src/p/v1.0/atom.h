#pragma once

#include "ckit/config.h"
#include "base/cocr_types.h"
namespace v1_0 {
    class JAtom {
        ElementType elementType;
        size_t id;
        AtomStereo atomStereo;
    public:
        float x, y, z;

        const AtomStereo &getAtomStereo() const;

        void setAtomStereo(const AtomStereo &_atomStereo);

        const ElementType &getElementType() const;

        const std::string &getElementName() const;

        size_t getAtomicNumber() const;

        void setElementType(const ElementType &_elementType);

        void setElementType(const std::string &_elementName);

        void setElementType(const size_t &_atomicNumber);

        const size_t &getId() const;

        void setId(const size_t &_id);

        JAtom();

        JAtom(const size_t &_id, const size_t &_atomicNumber);

        bool isCoord2dEmbedded() const;

        void setCoord2d(const float &_x, const float &_y);
    };
}