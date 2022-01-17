/*
 * =====================================================================================
 *
 *       Filename:  kernel_time_parser.hpp
 *    Description:  Parsing file containing average execution time for a kernel
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <sstream>
#include <fstream>

void kernelAverageParser()
{

    std::string prefix = "/home/idaoudi/somp/omps/headers/data/icpp/";
    //std::string prefix = "/Users/daid/phd/omps/headers/data/";
    std::string format = ".dat";
    std::string fileName;
    if (ARCH == "amdpowersave")
    {
        fileName = "amdpowersave-" + std::to_string(mSize) + "-" + std::to_string(bSize) + "-";
        mainKernels = {"dormqr", "dgeqrt", "dtsqrt", "dtsmqr", "dpotrf", "dtrsm", "dsyrk", "dgemm"};
    }
    else if (ARCH == "amdpowersavelu")
    {
        fileName = "amdpowersavelu-" + std::to_string(mSize) + "-" + std::to_string(bSize) + "-";
        mainKernels = {"dgemm", "dswptr", "dlaswp", "dgetrf"};
    }
    else if (ARCH == "intelpowersave")
    {
        fileName = "intelpowersave-" + std::to_string(mSize) + "-" + std::to_string(bSize) + "-";
        mainKernels = {"dormqr", "dgeqrt", "dtsqrt", "dtsmqr", "dpotrf", "dtrsm", "dsyrk", "dgemm"};
    }
    else if (ARCH == "intelpowersavelu")
    {
        fileName = "intelpowersavelu-" + std::to_string(mSize) + "-" + std::to_string(bSize) + "-";
        mainKernels = {"dgemm", "dswptr", "dlaswp", "dgetrf"};
    }

    for (auto kernel : mainKernels)
    {
        std::map<int, long double> nthTimeMap;
        std::string file = prefix + fileName + kernel + format;
        std::ifstream f(file);
        assert(f.is_open());
        std::string line;
        while (std::getline(f, line))
        {
            int nth = 0;
            long double ktime = 0.0;
            std::istringstream ss(line);
            ss >> nth >> ktime;
            nthTimeMap[nth] = ktime;
        }
        kernelNthTimeAverage.insert(std::pair<std::string, std::map<int, long double>>(kernel, nthTimeMap));
    }
}
