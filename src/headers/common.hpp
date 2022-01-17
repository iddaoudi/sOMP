/*
 * =====================================================================================
 *
 *       Filename:  common.hpp
 *    Description:  Common definitions
 *         Author:  DAOUDI Idriss, THIBAULT Samuel
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

std::deque<Task *> submissionQueue;
std::list<Task *> rawListOfTasks;
std::multimap<int, Task *> successorsMap;
std::map<int, Task *> tasksMap;
std::map<std::string, int> workerState;
std::map<int, std::string> cacheReceiverMap;
std::map<int, std::string> ramReceiverMap;
std::map<std::string, int> handlesLocationMap;
std::vector<std::string> gemmNames{"dgemm"};
std::vector<std::string> memAllocNames{"dplgsy", "dplrnt"};
std::map<int, std::string> scratchCpuHandle;
std::map<std::string, long double> kernelLeastTime;

simgrid::s4u::MutexPtr mutex = simgrid::s4u::Mutex::create();
simgrid::s4u::ConditionVariablePtr condVarTaskReady = simgrid::s4u::ConditionVariable::create();
simgrid::s4u::ConditionVariablePtr condVarWorkerReady = simgrid::s4u::ConditionVariable::create();

inline void taskPopulating(Task *aTask)
{
    // Populating Task object
    for (int i = 0; i < nTasks; i++)
    {
        int parsedPayload = 0;
        int taskReadySignal = 0;
        aTask[i].jobId = parsedJobId[i];
        tasksMap.insert(std::pair<int, Task *>(aTask[i].jobId, &aTask[i]));
        aTask[i].workerId = parsedWorkerId[i];
        aTask[i].name = parsedName[i];
        aTask[i].submitOrder = parsedSubmitOrder[i];
        aTask[i].submitTime = parsedSubmitTime[i];
        aTask[i].startTime = parsedStartTime[i];
        aTask[i].endTime = parsedEndTime[i];
        aTask[i].memoryTaskNode = parsedMemoryNode[i];
        for (auto it = parsedDependanceVector.at(i).begin(); it != parsedDependanceVector.at(i).end(); it++)
        {
            if (i == 0)
            {
                aTask[i].dependance.push_back(0);
            }
            else
            {
                aTask[i].dependance.push_back(*it);
            }
        }
        for (auto it = parsedSizeVector.at(i).begin(); it != parsedSizeVector.at(i).end(); it++)
        {
            aTask[i].size.push_back(*it);
        }
        for (auto it = parsedModeVector.at(i).begin(); it != parsedModeVector.at(i).end(); it++)
        {
            aTask[i].mode.push_back(*it);
        }
        for (auto it = parsedHandleVector.at(i).begin(); it != parsedHandleVector.at(i).end(); it++)
        {
            aTask[i].handle.push_back(*it);
        }
        for (auto it = parsedModeVector.at(i).begin(); it != parsedModeVector.at(i).end(); it++)
        {
            if (*it == "RW")
            {
                parsedPayload += 2;
            }
            else if (!(*it).empty())
            {
                parsedPayload += 1;
            }
        }
        aTask[i].payload = parsedPayload;
        taskReadySignal = parsedDependanceVector.at(i).size();
        aTask[i].taskReadySignal = taskReadySignal;
        rawListOfTasks.push_back(&aTask[i]);
    }
}

// Compute cache size depending on architecture
inline int architectureCacheSize()
{
    int cSize = realCacheSize / (commSize);
    if (cSize == 0)
    {
        throw std::logic_error("\nTile size won't fit in this machines' L3 cache.");
    }
    return cSize;
};

// Own version of the S4U send_to() function
auto send_to = [](auto destination, double byte_amount) {
    simgrid::s4u::Host *source = simgrid::s4u::Host::current();
    std::vector<simgrid::s4u::Host *> m_host_list = {source, destination};
    std::vector<double> flops_amount = {0, 0};
    std::vector<double> bytes_amount = {0, byte_amount, 0, 0};
    return simgrid::s4u::this_actor::exec_init(m_host_list, flops_amount, bytes_amount);
};

auto send_from_to = [](simgrid::s4u::Host *source, simgrid::s4u::Host *destination, double byte_amount) {
    std::vector<simgrid::s4u::Host *> m_host_list = {source, destination};
    std::vector<double> flops_amount = {0, 0};
    std::vector<double> bytes_amount = {0, byte_amount, 0, 0};
    return simgrid::s4u::this_actor::exec_init(m_host_list, flops_amount, bytes_amount);
};

// Own version of the S4U sleep_for() function
auto sleep_for = [](double time) {
    return simgrid::s4u::this_actor::sleep_for(time * 0.001); // Conversion from (ms) to (s)
};
