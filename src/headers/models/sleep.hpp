/*
 * =====================================================================================
 *
 *       Filename:  sleep.hpp
 *    Description:  Sleep time of each task
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

void sleeper(Task *msg, long unsigned int cpuNumber, long unsigned int coresPerSocket, double subtract /* ms */)
{
    std::vector<std::string>::iterator finder;
    finder = std::find(memAllocNames.begin(), memAllocNames.end(), msg->name);
    long double sleepTime;
    if (finder == memAllocNames.end())
    {
        if (std::find(mainKernels.begin(), mainKernels.end(), msg->name) != mainKernels.end())
        {
            if (TRACETIME == 1)
            {
                sleepTime = msg->endTime - msg->startTime;
            }
            else
            {
                coresPerSocket = CPS; //FIXME: coresPerSocket variable is overwritten here
                if (cpuNumber / coresPerSocket == nMachines / coresPerSocket)
                {
                    int target = (nMachines - 1) % coresPerSocket + 1;
                    sleepTime = kernelNthTimeAverage.find(msg->name)->second.find(target)->second;
                }
                else
                {
                    sleepTime = kernelNthTimeAverage.find(msg->name)->second.find(coresPerSocket)->second;
                }
            }
        }
        else
        {
            sleepTime = kernelLeastTime[msg->name];
        }
    }
    else
    {
        sleepTime = 0;
    }

    if (subtract > sleepTime * OVERLAPRATIO)
        subtract = sleepTime * OVERLAPRATIO;

    sleepTime -= subtract;
    sleep_for(sleepTime);
}
