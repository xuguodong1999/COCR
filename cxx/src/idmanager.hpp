#ifndef _IDMANAGER_HPP_
#define _IDMANAGER_HPP_

#include <vector>

class IdInterface {
    // c++17
    inline static std::vector<size_t> idPool;
    inline static size_t currentMaxId = 0;
protected:
    int id;

    IdInterface();

    ~IdInterface();

public:
    static size_t Get_new_id();

    static void Give_back_id(const size_t &_freedId);

    const size_t getId();
};

#endif//_IDMANAGER_HPP_
