#ifndef _XGD_TEXT_CORRECTION_HPP_
#define _XGD_TEXT_CORRECTION_HPP_
#include <string>
#include <vector>
namespace xgd{
    class TextCorrector{
    public:
        std::vector<std::string>correct(const std::string&_text);
    };
}
#endif//_XGD_TEXT_CORRECTION_HPP_
