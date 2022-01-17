/*
 * =====================================================================================
 *
 *       Filename:  comm_cache.hpp
 *    Description:  Communications-based model with L3 cache implementation
 *         Author:  DAOUDI Idriss, GAUTIER Thierry
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include "sleep.hpp"
#include "memopcomm.hpp"
#include <sstream>

inline void commModelCacheFunction(Task *msg, int localNode, double taskTotalTime, long unsigned int cpuNumber)
{


    // Memory allocation: storing every handle in cache
    std::vector<std::string>::iterator finder;
    finder = std::find(memAllocNames.begin(), memAllocNames.end(), msg->name);
    if (finder != memAllocNames.end())
    {
        cache[0]->appendToCache(msg->handle.at(0));
        //assert(cacheSt != false);
        cache[localNode]->unlockData(msg->handle.at(0));
        // Memory allocation communication
        simgrid::s4u::ExecPtr sender;
        sender = send_to(simgrid::s4u::Host::by_name(cacheReceiverMap.find(localNode)->second), commSize);
        sender->start();
        sender->wait();
        // Only one copy
        /* for (int i = 0; i < nCaches; i++){
            if (i != localNode){
                if (cache[i]->inCache(msg->handle.at(0)) == true){
                    bool tmpStatus = cache[i]->eraseFromCache(msg->handle.at(0));
                    if (tmpStatus == false){
                        throw std::runtime_error("Can't erase allocation handle from cache!");
                    }
                }
            }
        }*/
    }

    // Send and start() communications for memory operations
    std::vector<simgrid::s4u::ExecPtr> senderVector;
    std::vector<simgrid::s4u::ExecPtr> WsenderVector;

    double start = simgrid::s4u::Engine::get_clock();
    std::vector<std::string> cacheStatus;
    // All other tasks
    if (!msg->name.empty() && finder == memAllocNames.end())
    {
        std::vector<int> handleInCache;
        std::map<std::string, std::string> handleModeMap;

        // Creating a map {handle: mode}
        bool scratchIsPresent = false;
        for (long unsigned int i = 0; i < msg->handle.size(); i++)
        {
            handleModeMap.insert(std::pair<std::string, std::string>(msg->handle.at(i), msg->mode.at(i)));
            if (msg->mode.at(i) == "S")
            {
                scratchIsPresent = true;
            }
        }

        // Unique scratch for every core
        // Note: it's necessary to split every case in different for loops since erase() make the iterator have an undefined behavior
        // Case where the scratch has already been modified
        if (scratchIsPresent == true)
        {
            for (auto &aHandle : handleModeMap)
            {
                if (scratchCpuHandle[cpuNumber].size() != 0 && aHandle.second == "S")
                {
                    handleModeMap.erase(aHandle.first);
                    handleModeMap.insert(std::make_pair(scratchCpuHandle[cpuNumber], "S"));
                    break;
                }
            }
            // Case where the scratch is first encountred
            for (auto &aHandle : handleModeMap)
            {
                std::ostringstream identifier;
                identifier << cpuNumber;
                std::string handleNewName = aHandle.first + "-" + identifier.str();
                if (scratchCpuHandle[cpuNumber].size() == 0 && aHandle.second == "S")
                {
                    scratchCpuHandle[cpuNumber] = handleNewName;
                    handleModeMap.erase(aHandle.first);
                    handleModeMap.insert(std::make_pair(handleNewName, "S"));
                    handlesLocationMap.insert(std::make_pair(handleNewName, localNode));
                    break;
                }
            }
        }

        // Cache management
        for (auto &aHandle : handleModeMap)
        {
            // Flag for handle presence in cache
            int handleFlag = 0;
            // Search for the handle in local cache first
            if (cache[localNode]->inCache(aHandle.first) == true)
            {
                localCacheHit += 1;
                handleFlag += 1;
                bool tmpStatus = cache[localNode]->appendToCache(aHandle.first);
                if (tmpStatus == true)
                {
                    cacheStatus.push_back("NULL");
                }
                else
                {
                    cacheStatus.push_back(ramReceiverMap.find(handlesLocationMap.find(aHandle.first)->second)->second);
                }
            }
            else
            {
                // Fetch for the handle in all other caches
                for (int i = 0; i < nCaches; i++)
                {
                    // If handle is found in distant cache
                    if (cache[i]->inCache(aHandle.first) == true && i != localNode)
                    {
                        distantCacheHit += 1;
                        handleFlag += 1;
                        // If handle is RW or W, remove it from distant cache
                        if (aHandle.second == "RW" || aHandle.second == "W")
                        {
                            bool eraseStatus = cache[i]->eraseFromCache(aHandle.first);
                            cache[localNode]->appendToCache(aHandle.first);
                            // If handle can be removed, ie not locked
                            if (eraseStatus == true)
                            {
                                // Update cache according to availability
                                cacheStatus.push_back(cacheReceiverMap.find(i)->second);
                            }
                            else
                            {
                                // Handle is locked, ie go fetch from RAM
                                cacheStatus.push_back(ramReceiverMap.find(handlesLocationMap.find(aHandle.first)->second)->second);
                            }
                        }
                        // For R type operations, just try copy in local cache
                        else
                        {
                            cache[localNode]->appendToCache(aHandle.first);
                            cacheStatus.push_back(cacheReceiverMap.find(i)->second);
                        }
                        break;
                    }
                }
                // If a handle is nowhere
                if (handleFlag == 0)
                {
                    cacheMiss += 1;
                    cache[localNode]->appendToCache(aHandle.first);
                    cacheStatus.push_back(ramReceiverMap.find(handlesLocationMap.find(aHandle.first)->second)->second);
                }
            }
            // If handle found, remove handle from other caches (if the handle is in other caches!)
            if (aHandle.second == "RW" || aHandle.second == "W")
            {
                for (int i = 0; i < nCaches; i++)
                {
                    if (i != localNode)
                    {
                        cache[i]->eraseFromCache(aHandle.first);
                    }
                }
            }
        }

        for (auto &handle : handleModeMap)
        {
            if (handle.second == "RW" || handle.second == "W")
            { // || handle.second == "S"){ // Then W
                std::string location = ramReceiverMap.find(handlesLocationMap.find(handle.first)->second)->second;
                memOpCommW(WsenderVector, handle.second, location, localNode, cpuNumber);
            }
            if (handle.second == "S")
            {
                if (SCRATCHW == 1)
                {
                    std::string location = ramReceiverMap.find(handlesLocationMap.find(handle.first)->second)->second;
                    memOpCommW(WsenderVector, handle.second, location, localNode, cpuNumber);
                }
            }
        }

        // Blocking wait() for W, before submitting R
        if (SERIALRW == 1)
        {
            for (long unsigned int i = 0; i < WsenderVector.size(); i++)
            {
                WsenderVector[i]->wait();
            }
        }

        int cacheStatusCounter = 0;
        for (auto &handle : handleModeMap)
        {
            if (handle.second == "R" || handle.second == "RW")
            { // First R
                std::string location = ramReceiverMap.find(handlesLocationMap.find(handle.first)->second)->second;
                memOpCommR(cacheStatus[cacheStatusCounter], senderVector, handle.second, location, localNode, cpuNumber);
            }
            cacheStatusCounter++;
        }

        // Blocking wait() for W
        if (SERIALRW == 0)
        {
            for (long unsigned int i = 0; i < WsenderVector.size(); i++)
            {
                WsenderVector[i]->wait();
            }
        }

        if (OVERLAPR == 1)
        {
            // Overlapped Blocking wait() for R
            for (long unsigned int i = 0; i < senderVector.size(); i++)
            {
                senderVector[i]->wait();
            }
        }
    }
    
    double end = simgrid::s4u::Engine::get_clock();

    // non-overlapped Blocking wait() for R
    if (OVERLAPR == 0)
    {
        for (long unsigned int i = 0; i < senderVector.size(); i++)
        {
            senderVector[i]->wait();
        }
    }

    long unsigned int coresPerSocket = machines.size() / nCaches;
    // Update Simgrids' internal clock
    if (!msg->name.empty())
    {
        if (OVERLAP == 1)
        {
            sleeper(msg, cpuNumber, coresPerSocket, (end - start) * 1000);
        }
        else
        {
            sleeper(msg, cpuNumber, coresPerSocket, 0);
        }
    }

    // Unlock data in cache
    for (long unsigned int i = 0; i < msg->handle.size(); i++)
    {
        cache[localNode]->unlockData(msg->handle.at(i));
    }
}
