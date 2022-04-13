#include "els_ckit_export.h"
#include "base/hello.h"

namespace els::ckit {
    class ELS_CKIT_EXPORT Hello {
        els::base::Hello hello;
    public:
        Hello();

        int getC() const;

        int add(const int &a, const int &b) const;
    };
}