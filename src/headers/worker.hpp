/*
 * =====================================================================================
 *
 *       Filename:  worker.hpp
 *    Description:  sOMP simulator workers
 *         Author:  DAOUDI Idriss, THIBAULT Samuel
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include "models/comm_cache.hpp"
#include "models/comm.hpp"
#include "models/default.hpp"

static void worker()
{

    double taskStartTime = 0;
    double taskEndTime = 0;
    double taskTotalTime = 0;
    simgrid::s4u::Host *host = simgrid::s4u::this_actor::get_host();
    simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(host->get_cname());

    // Retrieve this task CPU number
    long unsigned int cpuNumber = mailboxCpuNumber(mailbox);

    // Which cache is my CPU on
    int localCache = cacheNum(cpuNumber);

    // Map every cache with its communications receiving host --> last host on every node for now
    // FIXME
    for (int i = 0; i < nCaches; i++)
    {
        //cacheReceiverMap.insert(std::pair<int, std::string>(i, machines[(machines.size()*(i+1)/nCaches)-1]));
        cacheReceiverMap.insert(std::pair<int, std::string>(i, cacheList[i]));
        ramReceiverMap.insert(std::pair<int, std::string>(i, ramList[i]));
    }

    /* Worker code */
    do
    {
        // Extract task message and some parameters
        Task *msg = static_cast<Task *>(mailbox->get<void>());
        taskStartTime = msg->startTime;
        taskEndTime = msg->endTime;
        taskTotalTime = taskEndTime - taskStartTime;

        // If it's the ending task, kill this worker
        if (taskStartTime < 0)
        {
            break;
        }

        std::vector<std::string> kernelsWithS = {"dgeqrt", "dormqr", "dtsqrt", "dtsmqr"};
        for (auto &i : kernelsWithS)
        {
            if (msg->name == i)
            {
                msg->handle.push_back("myhandle");
                msg->mode.push_back("S");
            }
        }
        // ******************** If no model wanted ********************
        if (noModel == true)
        {
            std::vector<std::string>::iterator finder;
            finder = std::find(memAllocNames.begin(), memAllocNames.end(), msg->name);
            if (finder != memAllocNames.end())
            {
                sleep_for(0);
            }
            else
            {
                sleep_for(taskTotalTime);
            }
        }

        // ******************** If model is wanted ********************
        else
        {
            // "Static" cache (fake write-back)
            std::vector<std::string>::iterator memFinder;
            memFinder = std::find(memAllocNames.begin(), memAllocNames.end(), msg->name);
            if (memFinder != memAllocNames.end())
            {
                handlesLocationMap.insert(std::pair<std::string, int>(msg->handle.at(0), localCache));
            }

            if (commModelCache == true)
            {
                commModelCacheFunction(msg, localCache, taskTotalTime, cpuNumber);
            }

            if (commModel == true)
            {
                commModelFunction(msg, localCache, taskTotalTime, cpuNumber);
            }

            if (defaultModel == true)
            {
                taskModel(msg, cpuNumber);
                //gemmModelFunction(msg, localCache, taskTotalTime);
            }
        }

        // Successors activation
        typedef std::multimap<int, Task *>::iterator succ_it;
        // Activate the successors of the finished task
        std::pair<succ_it, succ_it> result = successorsMap.equal_range(msg->jobId);
        for (succ_it successor = result.first; successor != result.second; successor++)
        {
            Task *task = successor->second;

            // Notify that the successor dependancy have been satisfied
            mutex->lock();
            task->taskReadySignal--;
            if (task->taskReadySignal == 0)
            {
                // Push successor in the scheduler submission queue
                submissionQueue.push_back(task);
                condVarTaskReady->notify_one();
            }
            // If the successor have yet other dependancies to satisfy, release the mutex
            mutex->unlock();
        }

        // Signal that the worker is ready
        mutex->lock();
        workerState[host->get_cname()] = 1;
        condVarWorkerReady->notify_one();
        mutex->unlock();
    } while (1);
    /*    std::cout << "dgemm tasks: " << gemm << std::endl;
    std::cout << "dpotrf tasks: " << potrf << std::endl;
    std::cout << "dsyrk tasks: " << syrk << std::endl;
    std::cout << "dtrsm tasks: " << trsm << std::endl;
    std::cout << "dplgsy tasks: " << plgsy << std::endl;
    std::cout << "unknown tasks: " << unknown << std::endl;*/
}
