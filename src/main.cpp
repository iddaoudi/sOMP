/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *    Description:  sOMP simulator main file
 *         Author:  DAOUDI Idriss, THIBAULT Samuel 
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <simgrid/s4u.hpp>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <iostream>
#include <map>
#include <mutex>
#include <cmath>
#include <iterator>
#include <getopt.h>
#include <stdexcept>
#include <chrono>
#include "headers/help.hpp"

#define commSize (bSize*bSize*8)

XBT_LOG_NEW_DEFAULT_CATEGORY(main, "Specific messages for sOMP simulator");

// Global variables
int realCacheSize;
long parsedTasks;
int nCaches;
double alpha;
double beta;
std::vector<std::string> hostList;
std::vector<std::string> cacheList;
std::vector<std::string> ramList;
std::multimap<std::string, std::map<int, long double>> kernelNthTimeAverage;

#include "headers/xml_parser.hpp"
#include "headers/rec_parser.hpp"

bool noModel;
bool commModel;
bool commModelCache;
bool defaultModel;

std::string affinity;

std::map<std::string, int> handlesMap;
std::vector<std::string> machines;
std::vector<std::string> usedMachines;
int mSize;
int bSize;
int corePerCache;
long nTasks;
long unsigned int nMachines;
int cacheMiss;
int localCacheHit;
int distantCacheHit;
int OVERLAP;
int SERIALRW;
int OVERLAPR;
double OVERLAPRATIO;
int SCHEDCACHE;
int CPS;
int CACHELOCK;
int TRACETIME;
int SCRATCHW;
std::string ARCH;

std::vector<std::string> mainKernels;

#include "headers/kernel_time_parser.hpp"
#include "headers/scheduler.hpp"

static void master(){

    if (TRACETIME == 0){
        kernelAverageParser();
    }
    Task* aTask = new Task[nTasks];

    // Populating Task object
    taskPopulating(aTask);
    // Obtaining quickest task time by kernel
    leastTime(aTask);
    
    // Populating successors map
    for (int i = 0; i < nTasks; i++) {
        for (auto it = aTask[i].dependance.begin(); it != aTask[i].dependance.end(); it++){
            successorsMap.insert(std::pair<int, Task *>((*it), &aTask[i]));
        }   
    }

    // Push the first tasks in the submission queue
    for (int i = 0; i < nTasks; i++) {
        if (aTask[i].taskReadySignal == 0){
            submissionQueue.push_back(&aTask[i]);
        }
    }
    
    // Start the scheduler
    scheduler();
    delete [] aTask;
}

int main (int argc, char* argv[]){

    OVERLAP = std::atoi(std::getenv("SOMP_OVERLAP"));
    OVERLAPR = std::atoi(std::getenv("SOMP_OVERLAPR"));
    OVERLAPRATIO = std::atof(std::getenv("SOMP_OVERLAPRATIO"));
    SERIALRW = std::atoi(std::getenv("SOMP_SERIALRW"));
    SCHEDCACHE = std::atoi(std::getenv("SOMP_SCHEDCACHE"));
    CACHELOCK = std::atoi(std::getenv("SOMP_CACHELOCK"));
    TRACETIME = std::atoi(std::getenv("SOMP_TRACETIME"));
    SCRATCHW = std::atoi(std::getenv("SOMP_SCRATCHW"));
    if (TRACETIME == 0){
        ARCH = std::getenv("SOMP_ARCH");
        CPS = std::atoi(std::getenv("SOMP_CPS"));
    } else {
        ARCH = "NULL";
        CPS = 0;
    }
        
    // Start S4U engine
    simgrid::s4u::Engine e(&argc, argv);

    // Load platform file from S4U
    e.load_platform(argv[1]);

    // Parse input files
    recFileParser(argc, argv);
    xmlParser(argc, argv);
    machines  = hostList;

    // Useful global variables
    nTasks    = parsedTasks;
    corePerCache = machines.size()/nCaches;

    // Get command line arguments
    std::string modelOption;
    int arguments;
    const option longOptions[] = {
        {"matrix", required_argument, nullptr, 'm'},
        {"bloc", required_argument, nullptr, 'b'},
        {"threads", required_argument, nullptr, 't'},
        {"options", required_argument, nullptr, 'o'},
        {"affinity", required_argument, nullptr, 'a'},
        {nullptr, no_argument, nullptr, 0}
    };
    while ((arguments = getopt_long(argc, argv, "m:b:t:o:a:h:", longOptions, nullptr)) != -1){
        if (optind > 3){
            switch(arguments){
                case 'm':
                    if (optarg) mSize = atoi(optarg);
                    break;
                case 'b':
                    if (optarg) bSize = atoi(optarg);
                    break;
                case 't':
                    if (optarg) nMachines = atoi(optarg);
                    break;
                case 'o':
                    if (optarg) modelOption = optarg;
                    break;
                case 'a':
                    if (optarg) affinity = optarg;
                    break;
                case 'h':
                    Help();
                    throw std::invalid_argument("\nTerminating...");
                case '?':
                    Help();
                    throw std::invalid_argument("\nInvalid argument. Terminating...");
            }
        }
    }
    if (!mSize) {
        XBT_INFO("Missing matrix size!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }
    if (!bSize) {
        XBT_INFO("Missing block size!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }
    if (!nMachines) {
        XBT_INFO("Missing number of threads/cores!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }
    if (modelOption.empty()) {
        XBT_INFO("Missing model options!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }
    if (affinity.empty()) {
        XBT_INFO("Missing affinity option!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }

    // Defining model from user input and checking for correctness
    if (modelOption == "nomodel"){
        noModel = true;
    } else if (modelOption == "comm"){
        commModel = true;
    } else if (modelOption == "commcache"){
        commModelCache = true;
    } else if (modelOption == "task"){
        defaultModel = true;
    } else {
        XBT_INFO("Invalid model argument.");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }

    // Checking for correct affinity option
    if (affinity != "spread" && affinity != "close"){
        XBT_INFO("Invalid affinity option!");
        Help();
        throw std::invalid_argument("\nTerminating...");
    }

    // Print error in case the number of cores submitted by user exceeds the number of cores available
    if (nMachines > machines.size()){
        XBT_INFO("Number of cores submitted exceeds the number of cores available on the platform (number of cores available is %lu).", machines.size());
        throw std::invalid_argument("\nTerminating...");
    }

    // Create the master/scheduler
    simgrid::s4u::Actor::create("sOMP-Scheduler", simgrid::s4u::Host::by_name(machines[0]), master);
    
    // Time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Start running the simulator
    e.run();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() * 1e-6; /* us to s */
    
    // Some useful prints
    std::cout << mSize << ' ' << bSize << ' ' << nMachines << ' ' << e.get_clock() << ' ' << localCacheHit << ' ' << distantCacheHit << ' ' << cacheMiss << ' ' << elapsedTime << std::endl;

}
