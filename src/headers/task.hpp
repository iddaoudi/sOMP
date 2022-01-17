/*
 * =====================================================================================
 *
 *       Filename:  task.hpp
 *    Description:  Task class definition
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

class Task
{
public:
	std::string name;
	int jobId;
	int workerId;
	int submitOrder;
	std::vector<int> dependance;
	std::vector<int> size;
	std::vector<std::string> mode;
	std::vector<std::string> handle;
	long double submitTime;
	long double startTime;
	long double endTime;
	int taskReadySignal;
	double payload;
	int memoryTaskNode;
	inline void PrintArgs()
	{
		XBT_INFO("Task name               : %s", name.c_str());
		XBT_INFO("Task ID		          : %d", jobId);
		XBT_INFO("Task submit time        : %Lf", submitTime);
		XBT_INFO("Task start time         : %Lf", startTime);
		XBT_INFO("Task end time           : %Lf", endTime);
		XBT_INFO("Task ready signal       : %d", taskReadySignal);
	};
};
