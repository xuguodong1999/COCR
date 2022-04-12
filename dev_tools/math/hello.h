#include "els_math_export.h"
namespace els::math {
    class ELS_MATH_EXPORT Hello {
        int c;
    public:
        Hello();

        int getC() const;

        int add(const int &a, const int &b) const;
    };
}