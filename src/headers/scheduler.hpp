/*
 * =====================================================================================
 *
 *       Filename:  scheduler.hpp
 *    Description:  sOMP simulator scheduler
 *         Author:  DAOUDI Idriss, GAUTIER Thierry, THIBAULT Samuel
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include "common.hpp"
#include "time.hpp"
#include "cache.hpp"
#include "worker.hpp"
#include "affinity.hpp"

inline static void scheduler()
{

    // Create caches
    int cacheSz = architectureCacheSize();
    cache = new CacheLRU *[nCaches];
    for (int i = 0; i < nCaches; i++)
    {
        cache[i] = new CacheLRU(cacheSz);
    }

    // Create the workers: one for every available machine according to selected affinity
    simgrid::s4u::ActorPtr actorOnWorker[nMachines];
    cpuAffinity(actorOnWorker);

    // Use taskCounter variable to see if all tasks have been dispatched
    int taskCounter = rawListOfTasks.size();
    while (taskCounter != 0)
    {
        while (submissionQueue.size() > 0)
        {

            // Lock everything for synchronization
            mutex->lock();

            // Check if any worker is available: iterate over workerState map
            std::map<std::string, int>::iterator workerStateIterator = workerState.begin();

            while (workerStateIterator != workerState.end())
            {
                if (workerStateIterator->second == 1)
                {
                    simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(workerStateIterator->first);
                    // Signal that the worker is busy now
                    workerState[mailbox->get_cname()] = 0;

                    Task *task;

                    if (SCHEDCACHE)
                    {
                        //
                        // Cache-aware scheduling
                        //
                        long unsigned int cpuNumber = mailboxCpuNumber(mailbox);
                        int localCache = cacheNum(cpuNumber);

                        // Find a task with most available data
                        std::deque<Task *>::iterator best;
                        int bestavail = -1;
                        for (auto aTask = submissionQueue.begin(); aTask != submissionQueue.end(); aTask++)
                        {
                            unsigned avail = 0;

                            for (auto &handle : (*aTask)->handle)
                                if (cache[localCache]->inCache(handle))
                                    // Cache hit
                                    avail++;

                            if (avail == (*aTask)->handle.size())
                            {
                                // It already has all its data, run this!
                                best = aTask;
                                break;
                            }

                            if ((int)avail > bestavail)
                            {
                                bestavail = avail;
                                best = aTask;
                            }
                        }
                        task = *best;
                        submissionQueue.erase(best);
                    }
                    else
                    {
                        // Get the front queue task to send it to the worker
                        task = submissionQueue.front();
                        submissionQueue.pop_front();
                    }

                    // Send the task to the chosen CPU
                    mailbox->put(task, 0);

                    // Update taskCounterer
                    taskCounter--;
                    // Break if there is no task to submit anymore
                    if (submissionQueue.size() == 0)
                    {
                        break;
                    }
                }
                // If the worker is already busy, check for another worker
                else
                {
                    ++workerStateIterator;
                }
            }
            // Wait for the workerState to change before using this worker
            condVarWorkerReady->wait(mutex);
            mutex->unlock();
        }

        // In case there is nothing in the submissionQueue: wait!
        mutex->lock();
        if (taskCounter != 0)
            while (submissionQueue.size() == 0)
                condVarTaskReady->wait(mutex);
        mutex->unlock();
    }

    // Killing the workers
    for (long unsigned int i = 0; i < nMachines; i++)
    {
        simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(usedMachines[i]);

        // Create an end task to unlock the static_casts in the workers
        Task *endTask = new (Task);
        endTask->jobId = -1;
        endTask->startTime = -1;
        endTask->taskReadySignal = 0;

        // Submit the ending task
        mailbox->put(endTask, 0);
        actorOnWorker[i]->join();
    }

    simgrid::s4u::Actor::kill_all();
    delete[] cache;
}
