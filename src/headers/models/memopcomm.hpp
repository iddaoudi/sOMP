/*
 * =====================================================================================
 *
 *       Filename:  memopcomm.hpp
 *    Description:  Communications for memory operations
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

std::string L3Host(int localNode, int cpuNumber)
{
    return cacheReceiverMap.find(localNode)->second;
}

inline void memOpCommR(std::string cacheStatusValue, std::vector<simgrid::s4u::ExecPtr> &senderVector, std::string handle, std::string location, int localNode, int cpuNumber)
{

    // In cache
    // Comms from L3 to CPU
    simgrid::s4u::ExecPtr sender;
    sender = send_from_to(simgrid::s4u::Host::by_name(L3Host(localNode, cpuNumber)), simgrid::s4u::Host::current(), commSize);
    sender->start();
    senderVector.push_back(sender);
    // Not in cache
    if (cacheStatusValue != "NULL")
    {
        // Comms to local L3
        simgrid::s4u::ExecPtr sender;
        sender = send_from_to(simgrid::s4u::Host::by_name(cacheStatusValue), simgrid::s4u::Host::by_name(L3Host(localNode, cpuNumber)), commSize);
        sender->start();
        senderVector.push_back(sender);
    }
}

inline void memOpCommW(std::vector<simgrid::s4u::ExecPtr> &senderVector, std::string handle, std::string location, int localNode, int cpuNumber)
{

    // In cache or not, same thing
    simgrid::s4u::ExecPtr sender;

    // Comms from CPU to L3 (for W)
    sender = send_from_to(simgrid::s4u::Host::current(), simgrid::s4u::Host::by_name(L3Host(localNode, cpuNumber)), commSize);
    sender->start();
    senderVector.push_back(sender);

    // Comms from L3 to RAM (for W)
    sender = send_from_to(simgrid::s4u::Host::by_name(L3Host(localNode, cpuNumber)), simgrid::s4u::Host::by_name(location), commSize);
    sender->start();
    senderVector.push_back(sender);
}
