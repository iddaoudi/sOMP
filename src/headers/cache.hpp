/*
 * =====================================================================================
 *
 *       Filename:  cache.hpp
 *    Description:  Cache class definition 
 *         Author:  DAOUDI Idriss 
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <unordered_map>

int mailboxCpuNumber(simgrid::s4u::Mailbox *mailbox)
{
    // Retrieve this CPU number
    const char *cpuNumberChar = mailbox->get_cname();
    return atoi(cpuNumberChar + 4);
}

int cacheNum(int cpuNumber)
{
    // Which cache is the CPU on
    int tmp = 0;
    for (int i = 0; i < nCaches; i++)
    {
        if (cpuNumber >= i * corePerCache && cpuNumber < (i + 1) * corePerCache)
        {
            tmp = i;
            break;
        }
    }
    return tmp;
}

class CacheLRU
{
    std::list<std::string> handlesList;
    std::list<std::string> lockedList;
    std::list<std::string> unlockedList;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheHashMap;
    std::unordered_map<std::string, int> dataLockFlag;
    long unsigned int cacheSize;

public:
    CacheLRU(int);
    bool appendToCache(std::string);
    void unlockData(std::string);
    void displayCache();
    void displayCacheSize();
    bool eraseFromCache(std::string);
    bool inCache(std::string);
};

CacheLRU::CacheLRU(int x)
{
    cacheSize = x;
}

void CacheLRU::displayCacheSize()
{
    std::cout << "Cache size is : " << cacheSize << std::endl;
}

bool CacheLRU::appendToCache(std::string x)
{
    // If space available for locked data
    if (lockedList.size() < cacheSize)
    {
        // If data not in cache
        if (cacheHashMap.find(x) == cacheHashMap.end())
        {
            // If cache full -> evict LRU element from unlocked data list
            if (handlesList.size() == cacheSize)
            {
                std::string last = unlockedList.back();
                unlockedList.pop_back();
                std::list<std::string>::iterator it = std::find(handlesList.begin(), handlesList.end(), last);
                handlesList.erase(it);
                cacheHashMap.erase(last);
            }
        }
        // If data is in cache
        else
        {
            std::list<std::string>::iterator iter;
            iter = std::find(lockedList.begin(), lockedList.end(), x);
            // If data is locked
            if (iter != lockedList.end())
            {
                if (CACHELOCK == 1)
                {
                    // Update flag
                    dataLockFlag[x] += 1;
                    // Make it first in cache
                    handlesList.erase(cacheHashMap[x]);
                    handlesList.push_front(x);
                    cacheHashMap[x] = handlesList.begin();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                std::list<std::string>::iterator it = std::find(unlockedList.begin(), unlockedList.end(), x);
                unlockedList.erase(it);
                handlesList.erase(cacheHashMap[x]);
            }
        }
        // Push data in locked list by default
        lockedList.push_front(x);
        handlesList.push_front(x);
        cacheHashMap[x] = handlesList.begin();
        if (CACHELOCK == 1)
        {
            dataLockFlag[x] = 1;
        }
        return true;
    }
    else
    {
        // No space: all data in cache is locked!
        return false;
    }
}

void CacheLRU::unlockData(std::string x)
{
    std::list<std::string>::iterator it = std::find(lockedList.begin(), lockedList.end(), x);
    // If data is locked
    if (it != lockedList.end())
    {
        if (CACHELOCK == 1)
        {
            // Update flag
            dataLockFlag[x] -= 1;
            // Only release if all flags down
            if (dataLockFlag[x] == 0)
            {
                lockedList.erase(it);
                unlockedList.push_front(x);
            }
        }
        else
        {
            lockedList.erase(it);
            unlockedList.push_front(x);
        }
    }
}

bool CacheLRU::inCache(std::string x)
{
    if (cacheHashMap.find(x) != cacheHashMap.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CacheLRU::eraseFromCache(std::string x)
{
    if (cacheHashMap.find(x) != cacheHashMap.end())
    {
        std::list<std::string>::iterator it = std::find(lockedList.begin(), lockedList.end(), x);
        std::list<std::string>::iterator it2 = std::find(unlockedList.begin(), unlockedList.end(), x);
        if (it != lockedList.end())
        {
            return false;
        }
        else if (it2 != unlockedList.end())
        {
            unlockedList.erase(it2);
        }
        handlesList.erase(cacheHashMap[x]);
        cacheHashMap.erase(x);
        return true;
    }
    else
    {
        return false;
    }
}

void CacheLRU::displayCache()
{
    std::cout << "Cache content: " << std::endl;
    for (auto iter = handlesList.begin(); iter != handlesList.end(); iter++)
    {
        std::cout << (*iter) << std::endl;
    }
}

CacheLRU **cache;
