#ifndef _ATOM_HPP_
#define _ATOM_HPP_

#include "cocr_types.hpp"
#include "fraction.hpp"
#include <string>
#include <vector>
#include <unordered_map>

extern std::vector<std::string> ElementsData;
extern std::unordered_map<ElementType, frac> ElementValenceData;

class JAtom {
    ElementType elementType;
    size_t id;
    AtomStereo atomStereo;
public:
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


};

// 返回建议比例，以碳为基准
float atomRadius(const ElementType &_element);
#endif//_ATOM_HPP_