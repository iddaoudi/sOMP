/*
 * =====================================================================================
 *
 *       Filename:  comm.hpp
 *    Description:  Communications-based model 
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

inline void commModelFunction(Task *msg, int taskNodeLocation, double taskTotalTime, long unsigned int cpuNumber)
{

	std::vector<std::string>::iterator memFinder;
	memFinder = std::find(memAllocNames.begin(), memAllocNames.end(), msg->name);
	std::vector<int> handleVec;
	simgrid::s4u::ExecPtr *senders;

	double start = simgrid::s4u::Engine::get_clock();
	// Not an "empty" task nor a memory allocation task
	if (!msg->name.empty() && memFinder == memAllocNames.end())
	{
		// Map handles and modes of task
		std::map<std::string, std::string> handleModeMap;
		for (long unsigned int i = 0; i < msg->handle.size(); i++)
		{
			handleModeMap.insert(std::pair<std::string, std::string>(msg->handle.at(i), msg->mode.at(i)));
		}
		long unsigned int witness = 0;
		// Store handles locations
		for (std::map<std::string, std::string>::iterator aHandle = handleModeMap.begin(); aHandle != handleModeMap.end(); aHandle++)
		{
			std::map<std::string, int>::iterator handlesLocation = handlesLocationMap.find(aHandle->first);
			if (aHandle->second == "RW")
			{
				handleVec.push_back(handlesLocation->second);
				handleVec.push_back(handlesLocation->second);
				witness += 2;
			}
			else
			{
				handleVec.push_back(handlesLocation->second);
				witness += 1;
			}
		}
		// Keeping it safe
		if (witness != handleVec.size())
		{
			throw std::logic_error("\nHandle vector size is not equal to the task payload (witness).\n");
		}
		// Senders array
		senders = new simgrid::s4u::ExecPtr[witness];
		// Start communications
		int i = 0;
		for (auto itr = handleVec.begin(); itr != handleVec.end(); itr++)
		{
			senders[i] = send_to(simgrid::s4u::Host::by_name(ramReceiverMap.find(*itr)->second), commSize);
			senders[i]->start();
			i++;
		}

		for (long unsigned int i = 0; i < handleVec.size(); i++)
		{
			senders[i]->wait();
		}
	}
	double end = simgrid::s4u::Engine::get_clock();

	long unsigned int coresPerSocket = machines.size() / nCaches;
	// Update clock
	if (!msg->name.empty())
	{
		if (OVERLAP == 1)
			sleeper(msg, cpuNumber, coresPerSocket, (end - start) * 1000);
		else
			sleeper(msg, cpuNumber, coresPerSocket, 0);
	}
}
