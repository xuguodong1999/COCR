#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include "config.hpp"
#include "3rdutil.hpp"
class SMILESGenerator {
public:
    static SMILESGenerator &GetInstance();

    /**
     * 拿到烷烃的smiles串
     * @param alkane_dir
     * @param carbonNum
     */
    static void HashToSMILES(const char *alkane_dir, const size_t &carbonNum);

    /**
     * 拿到添加了杂原子、基团的smiles串
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