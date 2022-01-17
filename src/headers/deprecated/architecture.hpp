/*
 * =====================================================================================
 *
 *       Filename:  cache_sz.hpp
 *    Description:  Determines cache size depending on the stored CPU models
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <iostream>

inline int architectureCacheSize(){
    int cSize = realCacheSize/(commSize);
    if (cSize == 0){
        XBT_INFO("Cache size is 0. Tile size won't fit in this machines' L3 cache.");
        std::abort();
    }
    return cSize;
};


