#include "els_base_export.h"
namespace els::base {
    class ELS_BASE_EXPORT Hello {
        int c;
    public:
        Hello();

        int getC() const;

        int add(const int &a, const int &b) const;
    };
}