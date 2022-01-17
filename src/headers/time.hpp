/*
 * =====================================================================================
 *
 *       Filename:  time.hpp
 *    Description:  Getting smallest time of every kernel
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

inline void leastTime(Task *aTask)
{
    std::vector<std::string> kernels;
    for (int i = 0; i < nTasks; i++)
    {
        std::vector<std::string>::iterator kernel = std::find(kernels.begin(), kernels.end(), aTask[i].name);
        if (kernel == kernels.end())
        {
            kernels.push_back(aTask[i].name);
        }
    }

    std::vector<std::vector<long double>> times;

    for (const auto &kernel : kernels)
    {
        std::vector<long double> kernelTimes;
        for (int i = 0; i < nTasks; i++)
        {
            if (aTask[i].name == kernel)
            {
                long double tmp = aTask[i].endTime - aTask[i].startTime;
                kernelTimes.push_back(tmp);
            }
        }
        long double tmp = *std::min_element(kernelTimes.begin(), kernelTimes.end());
        kernelLeastTime[kernel] = tmp;
        times.push_back(kernelTimes);
    }

    // Safety
    assert(times.size() == kernels.size());
}
