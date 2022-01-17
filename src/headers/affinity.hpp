/*
 * =====================================================================================
 *
 *       Filename:  affinity.hpp
 *    Description:  Affinity policies
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

inline static void cpuAffinity(simgrid::s4u::ActorPtr actorOnWorker[])
{
    if (affinity == "spread")
    {
        long unsigned int counter = 0;
        for (long unsigned int i = 0; i < machines.size() / nCaches; i++)
        {
            for (int j = 0; j < nCaches; j++)
            {
                usedMachines.push_back(machines[i + (machines.size() / nCaches) * j]);
                simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(machines[i + (machines.size() / nCaches) * j]);
                actorOnWorker[counter] = simgrid::s4u::Actor::create("Worker", simgrid::s4u::Host::by_name(mailbox->get_cname()), worker);
                workerState.insert(std::make_pair(machines[i + (machines.size() / nCaches) * j], 1));
                if (counter == nMachines - 1)
                {
                    goto outloop;
                }
                else
                {
                    counter++;
                }
            }
        }
    }
outloop:;
    if (affinity == "close")
    {
        for (long unsigned int i = 0; i < nMachines; i++)
        {
            simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(machines[i]);
            usedMachines.push_back(machines[i]);
            actorOnWorker[i] = simgrid::s4u::Actor::create("Worker", simgrid::s4u::Host::by_name(mailbox->get_cname()), worker);
            workerState.insert(std::make_pair(machines[i], 1));
        }
    }
}
