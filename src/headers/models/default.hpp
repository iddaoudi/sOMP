/*
 * =====================================================================================
 *
 *       Filename:  default.hpp
 *    Description:  Default task model 
 *         Author:  DAOUDI Idriss, THIBAULT Samuel
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

inline void taskModel(Task *msg, long unsigned int cpuNumber)
{
	long unsigned int coresPerSocket = machines.size() / nCaches;
	if (!msg->name.empty())
	{
		sleeper(msg, cpuNumber, coresPerSocket, 0);
	}
}
