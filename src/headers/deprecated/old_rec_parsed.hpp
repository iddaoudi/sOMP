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
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <list>

#include "task.hpp"

#define TEST(f1, f2) (!strcmp(f1, f2))

int *parsedJobId;
int *parsedWorkerId;
// FIXME: use vector
std::vector<int> parsedDependanceVector;
int *parsedDependance1;
int *parsedDependance2;
int *parsedDependance3;
int *parsedDependance4;
int *parsedDependance5;
int *parsedDependance6;
// FIXME: use vector
std::vector<int> parsedSizeVector;
int *parsedSize1;
int *parsedSize2;
int *parsedSize3;
int *parsedMemoryNode;

double *parsedSubmitOrder;
double *parsedSubmitTime;
double *parsedStartTime;
double *parsedEndTime;

std::string *parsedName;
// FIXME: use vector
std::vector<int> parsedHandleVector;
std::string *parsedHandle1;
std::string *parsedHandle2;
std::string *parsedHandle3;
// FIXME: use vector
std::vector<int> parsedModeVector;
std::string *parsedMode1;
std::string *parsedMode2;
std::string *parsedMode3;

std::vector<std::string> parsedHandle;
std::vector<std::string> parsedMode;
std::vector<std::string> parsedSize;
std::vector<std::string> parsedDependance;

inline void recFileParser(int argc, char* argv[]){
	std::fstream recFile(argv[2]);
	std::string emptyLine;


	while(std::getline(recFile, emptyLine)){
		std::istringstream tokenizer(emptyLine);
		std::string x, y;
		std::getline(tokenizer, x, ':');
		std::getline(tokenizer, y, '\n');
		const char* key = x.c_str();
		if (TEST(key, "")){
			parsedTasks += 1;
		}
	}

	int handlesCounter;
	int modesCounter;
	int sizesCounter;
	int dependancesCounter;

	parsedJobId       = (int*)calloc(parsedTasks, sizeof(int));
	parsedWorkerId    = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance1 = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance2 = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance3 = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance4 = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance5 = (int*)calloc(parsedTasks, sizeof(int));
	parsedDependance6 = (int*)calloc(parsedTasks, sizeof(int));
	parsedSize1       = (int*)calloc(parsedTasks, sizeof(int));
	parsedSize2       = (int*)calloc(parsedTasks, sizeof(int));
	parsedSize3       = (int*)calloc(parsedTasks, sizeof(int));
    parsedMemoryNode  = (int*)calloc(parsedTasks, sizeof(int));

	parsedSubmitOrder = (double*)calloc(parsedTasks, sizeof(double));
	parsedSubmitTime  = (double*)calloc(parsedTasks, sizeof(double));
	parsedStartTime   = (double*)calloc(parsedTasks, sizeof(double));
	parsedEndTime     = (double*)calloc(parsedTasks, sizeof(double));

	parsedName        = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedHandle1     = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedHandle2     = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedHandle3     = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedMode1       = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedMode2       = (std::string*)calloc(parsedTasks, sizeof(std::string));
	parsedMode3       = (std::string*)calloc(parsedTasks, sizeof(std::string));

	// First task doesn't have any dependancy
	parsedDependance1[0] = 0;
	parsedDependance2[0] = 0;
	parsedDependance3[0] = 0;
	parsedDependance4[0] = 0;
	parsedDependance5[0] = 0;
	parsedDependance6[0] = 0;
    parsedDependanceVector.push_back(0);

	// FIXME: use loops over number of dependencies, number of data
	std::fstream file(argv[2]);
	std::string line;
	int counter = 0;
	while(std::getline(file, line)){
		std::istringstream tokenizer(line);
		std::string x;
		std::string y;
		std::getline(tokenizer, x, ':');
		std::getline(tokenizer, y);
		const char* key = x.c_str();

		if(strcmp(key, "") != 0){

			if(TEST(key, "Name")){
				y.erase(std::remove_if(y.begin(), y.end(), isspace));
				parsedName[counter] = y;
			}
			else if(TEST(key, "JobId")){
				parsedJobId[counter] = std::stoi(y);
			}
            else if(TEST(key, "WorkerId")){
				parsedWorkerId[counter] = std::stoi(y);
			}
			else if(TEST(key, "SubmitOrder")){
				parsedSubmitOrder[counter] = std::stoi(y);
			}
			else if(TEST(key, "SubmitTime")){
				parsedSubmitTime[counter] = std::stod(y);
			}
			else if(TEST(key, "StartTime")){
				parsedStartTime[counter] = std::stod(y);
			}
			else if(TEST(key, "EndTime")){
				parsedEndTime[counter] = std::stod(y);
			}
            else if(TEST(key, "MemoryNode")){
				parsedMemoryNode[counter] = std::stoi(y);
			}
			// CHANTIER
            else if(TEST(key, "Handles")){
				std::istringstream ss(y);
				std::string str;
				while(getline(ss, str)){
					std::istringstream ss2(str);
					std::string str2;
					while(ss2 >> str2){
						parsedHandle.push_back(str2);
					}
				}
				int insideHandlesCounter = parsedHandle.size() - handlesCounter;
				std::cout << "insideHandlesCounter : " << insideHandlesCounter << std::endl;
                std::cout << "pardedHandle.size : " << parsedHandle.size() << std::endl;
                std::cout << "handlesCounter : " << handlesCounter << std::endl;
                if (insideHandlesCounter == 1){
					parsedHandle1[counter] = parsedHandle[handlesCounter];
				}
				if (insideHandlesCounter == 2){
					parsedHandle1[counter] = parsedHandle[handlesCounter];
					parsedHandle2[counter] = parsedHandle[handlesCounter + 1];
				}
				if (insideHandlesCounter == 3){
					parsedHandle1[counter] = parsedHandle[handlesCounter];
					parsedHandle2[counter] = parsedHandle[handlesCounter + 1];
					parsedHandle3[counter] = parsedHandle[handlesCounter + 2];
				}
			}
			// CHANTIER
			else if(TEST(key, "Modes")){
				std::istringstream ss(y);
				std::string str;
				while(getline(ss, str)){
					std::istringstream ss2(str);
					std::string str2;
					while(ss2 >> str2){
						parsedMode.push_back(str2);
					}
				}
				int insideModesCounter = parsedMode.size() - modesCounter;
				if (insideModesCounter == 1){
					parsedMode1[counter] = parsedMode[modesCounter];
				}
				if (insideModesCounter == 2){
					parsedMode1[counter] = parsedMode[modesCounter];
					parsedMode2[counter] = parsedMode[modesCounter + 1];
				}
				if (insideModesCounter == 3){
					parsedMode1[counter] = parsedMode[modesCounter];
					parsedMode2[counter] = parsedMode[modesCounter + 1];
					parsedMode3[counter] = parsedMode[modesCounter + 2];
				}
			}
			// CHANTIER
			else if(TEST(key, "Sizes")){
				std::istringstream ss(y);
				std::string str;
				while(getline(ss, str)){
					std::istringstream ss2(str);
					std::string str2;
					while(ss2 >> str2){
						parsedSize.push_back(str2);
					}
				}
				int insideSizesCounter = parsedSize.size() - sizesCounter;
				if (insideSizesCounter == 1){
					parsedSize1[counter] = std::stoi(parsedSize[sizesCounter]);
					parsedSize2[counter] = 0;
					parsedSize3[counter] = 0;
				}
				if (insideSizesCounter == 2){
					parsedSize1[counter] = std::stoi(parsedSize[sizesCounter]);
					parsedSize2[counter] = std::stoi(parsedSize[sizesCounter]);
					parsedSize3[counter] = 0;
				}
				if (insideSizesCounter == 3){
					parsedSize1[counter] = std::stoi(parsedSize[sizesCounter]);
					parsedSize2[counter] = std::stoi(parsedSize[sizesCounter]);
					parsedSize3[counter] = std::stoi(parsedSize[sizesCounter]);
				}
			}
			// CHANTIER
			else if(TEST(key, "DependsOn")){
				std::istringstream ss(y);
				std::string str;
				while(getline(ss, str)){
					std::istringstream ss2(str);
					std::string str2;
					while(ss2 >> str2){
						parsedDependance.push_back(str2);
					}
				}
				int insideDependancesCounter = parsedDependance.size() - dependancesCounter;
				if (insideDependancesCounter == 1){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = 0;
					parsedDependance3[counter] = 0;
					parsedDependance4[counter] = 0;
					parsedDependance5[counter] = 0;
					parsedDependance6[counter] = 0;
				}
				if (insideDependancesCounter == 2){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = std::stoi(parsedDependance[dependancesCounter + 1]);
					parsedDependance3[counter] = 0;
					parsedDependance4[counter] = 0;
					parsedDependance5[counter] = 0;
					parsedDependance6[counter] = 0;
				}
				if (insideDependancesCounter == 3){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = std::stoi(parsedDependance[dependancesCounter + 1]);
					parsedDependance3[counter] = std::stoi(parsedDependance[dependancesCounter + 2]);
					parsedDependance4[counter] = 0;
					parsedDependance5[counter] = 0;
					parsedDependance6[counter] = 0;
				}
				if (insideDependancesCounter == 4){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = std::stoi(parsedDependance[dependancesCounter + 1]);
					parsedDependance3[counter] = std::stoi(parsedDependance[dependancesCounter + 2]);
					parsedDependance4[counter] = std::stoi(parsedDependance[dependancesCounter + 3]);
					parsedDependance5[counter] = 0;
					parsedDependance6[counter] = 0;
				}
				if (insideDependancesCounter == 5){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = std::stoi(parsedDependance[dependancesCounter + 1]);
					parsedDependance3[counter] = std::stoi(parsedDependance[dependancesCounter + 2]);
					parsedDependance4[counter] = std::stoi(parsedDependance[dependancesCounter + 3]);
					parsedDependance5[counter] = std::stoi(parsedDependance[dependancesCounter + 4]);
					parsedDependance6[counter] = 0;
				}
				if (insideDependancesCounter == 6){
					parsedDependance1[counter] = std::stoi(parsedDependance[dependancesCounter]);
					parsedDependance2[counter] = std::stoi(parsedDependance[dependancesCounter + 1]);
					parsedDependance3[counter] = std::stoi(parsedDependance[dependancesCounter + 2]);
					parsedDependance4[counter] = std::stoi(parsedDependance[dependancesCounter + 3]);
					parsedDependance5[counter] = std::stoi(parsedDependance[dependancesCounter + 4]);
					parsedDependance6[counter] = std::stoi(parsedDependance[dependancesCounter + 5]);
				}
			}
			// Update spesific counters
			handlesCounter = parsedHandle.size();
			modesCounter = parsedMode.size();
			sizesCounter = parsedSize.size();
			dependancesCounter = parsedDependance.size();
		} else {
			counter++;
		}
	}

#ifdef DEBUG
	XBT_INFO("Number of parsed tasks from .rec file: %d", parsedTasks);
	for (int i = 0; i < parsedTasks; i++){
		std::cout << "######################################" << std::endl;
		std::cout << "JobId  : " << parsedJobId[i] << std::endl;
		std::cout << "Name   : " << parsedName[i] << std::endl;
		std::cout << "Order  : " << parsedSubmitOrder[i] << std::endl;
		std::cout << "Submit : " << parsedSubmitTime[i] << std::endl;
		std::cout << "Start  : " << parsedStartTime[i] << std::endl;
		std::cout << "End    : " << parsedEndTime[i] << std::endl;
		std::cout << "Handles1 : " << parsedHandle1[i] << std::endl;
		std::cout << "Handles2 : " << parsedHandle2[i] << std::endl;
		std::cout << "Handles3 : " << parsedHandle3[i] << std::endl;
		std::cout << "Modes1 : " << parsedMode1[i] << std::endl;
		std::cout << "Modes2 : " << parsedMode2[i] << std::endl;
		std::cout << "Modes3 : " << parsedMode3[i] << std::endl;
		std::cout << "Sizes1 : " << parsedSize1[i] << std::endl;
		std::cout << "Sizes2 : " << parsedSize2[i] << std::endl;
		std::cout << "Sizes3 : " << parsedSize3[i] << std::endl;
		std::cout << "Depend1: " << parsedDependance1[i] << std::endl;
		std::cout << "Depend2: " << parsedDependance2[i] << std::endl;
		std::cout << "Depend3: " << parsedDependance3[i] << std::endl;
		std::cout << "Depend4: " << parsedDependance4[i] << std::endl;
		std::cout << "Depend5: " << parsedDependance5[i] << std::endl;
		std::cout << "Depend6: " << parsedDependance6[i] << std::endl;
		std::cout << "######################################" << std::endl;
	}
#endif
}
