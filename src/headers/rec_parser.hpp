/*
 * =====================================================================================
 *
 *       Filename:  rec_parser.hpp
 *    Description:  Trace file parser (.rec format)
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstring>

#include "task.hpp"

#define TEST(f1, f2) (!strcmp(f1, f2))

int *parsedJobId;
int *parsedWorkerId;
int *parsedMemoryNode;

double *parsedSubmitOrder;
double *parsedSubmitTime;
double *parsedStartTime;
double *parsedEndTime;
std::string *parsedName;

std::vector<std::vector<std::string>> parsedHandleVector;
std::vector<std::vector<std::string>> parsedModeVector;
std::vector<std::vector<int>> parsedSizeVector;
std::vector<std::vector<int>> parsedDependanceVector;

inline void recFileParser(int argc, char *argv[])
{
	std::fstream recFile(argv[2]);
	std::string emptyLine;

	while (std::getline(recFile, emptyLine))
	{
		std::istringstream tokenizer(emptyLine);
		std::string x, y;
		std::getline(tokenizer, x, ':');
		std::getline(tokenizer, y, '\n');
		const char *key = x.c_str();
		if (TEST(key, ""))
		{
			parsedTasks += 1;
		}
	}
	if (parsedTasks == 0)
	{
		throw std::invalid_argument("\nUnable to read trace file. Does this file exist?");
	}

	parsedJobId = (int *)calloc(parsedTasks, sizeof(int));
	parsedWorkerId = (int *)calloc(parsedTasks, sizeof(int));
	parsedMemoryNode = (int *)calloc(parsedTasks, sizeof(int));

	parsedSubmitOrder = (double *)calloc(parsedTasks, sizeof(double));
	parsedSubmitTime = (double *)calloc(parsedTasks, sizeof(double));
	parsedStartTime = (double *)calloc(parsedTasks, sizeof(double));
	parsedEndTime = (double *)calloc(parsedTasks, sizeof(double));

	parsedName = (std::string *)calloc(parsedTasks, sizeof(std::string));

	std::fstream file(argv[2]);
	std::string line;

	int counter = 0;
	int modeCounter = 0;
	int sizeCounter = 0;
	int handleCounter = 0;
	int dependanceCounter = 0;

	while (std::getline(file, line))
	{
		std::istringstream tokenizer(line);
		std::string x;
		std::string y;
		std::getline(tokenizer, x, ':');
		std::getline(tokenizer, y);
		const char *key = x.c_str();

		if (strcmp(key, "") != 0)
		{

			if (TEST(key, "Name"))
			{
				y.erase(std::remove_if(y.begin(), y.end(), isspace));
				parsedName[counter] = y;
			}
			else if (TEST(key, "JobId"))
			{
				parsedJobId[counter] = std::stoi(y);
			}
			else if (TEST(key, "WorkerId"))
			{
				parsedWorkerId[counter] = std::stoi(y);
			}
			else if (TEST(key, "SubmitOrder"))
			{
				parsedSubmitOrder[counter] = std::stoi(y);
			}
			else if (TEST(key, "SubmitTime"))
			{
				parsedSubmitTime[counter] = std::stod(y);
			}
			else if (TEST(key, "StartTime"))
			{
				parsedStartTime[counter] = std::stod(y);
			}
			else if (TEST(key, "EndTime"))
			{
				parsedEndTime[counter] = std::stod(y);
			}
			else if (TEST(key, "MemoryNode"))
			{
				parsedMemoryNode[counter] = std::stoi(y);
			}
			// CHANTIER
			else if (TEST(key, "Handles"))
			{
				while (handleCounter != counter)
				{
					parsedHandleVector.push_back(std::vector<std::string>());
					handleCounter += 1;
				}
				std::vector<std::string> parsedHandle;
				std::istringstream ss(y);
				std::string str;
				while (getline(ss, str))
				{
					std::istringstream ss2(str);
					std::string str2;
					while (ss2 >> str2)
					{
						parsedHandle.push_back(str2);
					}
				}
				parsedHandleVector.push_back(parsedHandle);
				handleCounter += 1;
			}
			// CHANTIER
			else if (TEST(key, "Modes"))
			{
				while (modeCounter != counter)
				{
					parsedModeVector.push_back(std::vector<std::string>());
					modeCounter += 1;
				}
				std::vector<std::string> parsedMode;
				std::istringstream ss(y);
				std::string str;
				while (getline(ss, str))
				{
					std::istringstream ss2(str);
					std::string str2;
					while (ss2 >> str2)
					{
						parsedMode.push_back(str2);
					}
				}
				parsedModeVector.push_back(parsedMode);
				modeCounter += 1;
			}
			// CHANTIER
			else if (TEST(key, "Sizes"))
			{
				while (sizeCounter != counter)
				{
					parsedSizeVector.push_back(std::vector<int>());
					sizeCounter += 1;
				}
				std::vector<std::string> parsedSize;
				std::vector<int> tmpParsedSize;
				std::istringstream ss(y);
				std::string str;
				while (getline(ss, str))
				{
					std::istringstream ss2(str);
					std::string str2;
					while (ss2 >> str2)
					{
						parsedSize.push_back(str2);
					}
				}
				for (auto it = parsedSize.begin(); it != parsedSize.end(); it++)
				{
					tmpParsedSize.push_back(std::stoi(*it));
				}
				parsedSizeVector.push_back(tmpParsedSize);
				sizeCounter += 1;
			}
			// CHANTIER
			else if (TEST(key, "DependsOn"))
			{
				while (dependanceCounter != counter)
				{
					parsedDependanceVector.push_back(std::vector<int>());
					dependanceCounter += 1;
				}
				std::vector<std::string> parsedDependance;
				std::vector<int> tmpParsedDependance;
				std::istringstream ss(y);
				std::string str;
				while (getline(ss, str))
				{
					std::istringstream ss2(str);
					std::string str2;
					while (ss2 >> str2)
					{
						parsedDependance.push_back(str2);
					}
				}
				for (auto it = parsedDependance.begin(); it != parsedDependance.end(); it++)
				{
					tmpParsedDependance.push_back(std::stoi(*it));
				}
				parsedDependanceVector.push_back(tmpParsedDependance);
				dependanceCounter += 1;
			}
			// Update counter
		}
		else
		{
			counter++;
			while (handleCounter != counter)
			{
				parsedHandleVector.push_back(std::vector<std::string>());
				handleCounter++;
			}
			while (modeCounter != counter)
			{
				parsedModeVector.push_back(std::vector<std::string>());
				modeCounter++;
			}
			while (sizeCounter != counter)
			{
				parsedSizeVector.push_back(std::vector<int>());
				sizeCounter++;
			}
			while (dependanceCounter != counter)
			{
				parsedDependanceVector.push_back(std::vector<int>());
				dependanceCounter++;
			}
		}
	}
}
