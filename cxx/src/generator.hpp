#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include "cocr_types.hpp"

#include <functional>

class SMILESGenerator {
public:
    static SMILESGenerator &GetInstance();

    /**
     * 保存烷烃的smiles串到文件
     * @param alkane_dir
     * @param carbonNum
     */
    static void HashToSMILES(const char *alkane_dir, const size_t &carbonNum);

    /**
     * 保存添加了杂原子、基团的smiles串到文件
     * @param alkane_dir
     * @param carbonNum
     */
    static void HashToRichSMILES(const char *alkane_dir, const size_t &carbonNum);

private:
    SMILESGenerator() {}

    ~SMILESGenerator() {}

    SMILESGenerator(const SMILESGenerator &) = delete;

    const SMILESGenerator &operator=(const SMILESGenerator &) = delete;

    static void traverse(
            const char *alkane_dir, const size_t &carbonNum,
            std::function<void(const hash_type &)> func);
};


#endif //_GENERATOR_HPP_