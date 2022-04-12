#include "element.hpp"

using namespace std;

Element::Element() {
    amount = 0;
    valence = 0;
    Reduced = 0;
    finalValence = 100;
}

Element::Element(string &_name) {
    name = _name;
    amount = 0;
    valence = 0;
    Reduced = 0;
    finalValence = 100;
}

Element::~Element() {
    clear();
}

void Element::clear() {
    name.clear();
    amount = 0;
    valence = 0;
    Reduced = 0;
    finalValence = 100;
}

bool Element::Exist() {
    string tmp1 = "#H#He#Li#Be#B#C#N#O#F#Ne#Na#Mg#Al#Si#P#S#Cl#Ar#K#Ca#Sc#Ti#V#Cr#Mn#Fe#Co#Ni#Cu#Zn#Ga#Ge#As#Se#Br#Kr#Rb#Sr#Y#Zr#Nb#Mo#Tc#Ru#Rh#Pd#Ag#Cd#In#Sn#Sb#Te#I#Xe#Cs#Ba#La#Ce#Pr#Nd#Pm#Sm#Eu#Gd#Tb#Dy#Ho#Er#Tm#Yb#Lu#Hf#Ta#W#Re#Os#Ir#Pt#Au#Hg#Tl#Pb#Bi#Po#At#Rn#Fr#Ra#Ac#Th#Pa#U#Np#Pu#Am#Cm#Bk#Cf#Es#Fm#Md#No#Lr#Rf#Db#Sg#Bh#Hs#Mt#Ds#Rg#Cn#Uut#Fl#Uup#Lv#Uuo#";
    string tmp2 = "#" + name + "#";
    if (tmp1.find(tmp2) == string::npos)
        return 0;
    else
        return 1;
}

bool namelessthan(const Element &a, const Element &b) {
    return a.name < b.name;
}
