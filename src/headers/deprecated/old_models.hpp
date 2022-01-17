#include <simgrid/s4u.hpp>
#include <list>
#include <iostream>
#include <string>


inline static void gemmFunction(Task* someTask, int nMachines, int taskNodeLocation, int nodeNumber, float taskTotalTime){
	if (nodeNumber == 0 && taskNodeLocation == 0){
		simgrid::s4u::this_actor::sleep_for(taskTotalTime*0.001);
	}
	else if (nodeNumber == 0 && taskNodeLocation == 1){
		double localityTax = 0.01313326*nMachines + 1.02208052;
		simgrid::s4u::this_actor::sleep_for(taskTotalTime*0.001*localityTax);
	}
	else if (nodeNumber == 1 && taskNodeLocation == 0){
		double localityTax = 0.01313326*nMachines + 1.02208052;
		simgrid::s4u::this_actor::sleep_for(taskTotalTime*0.001*localityTax);
	}
	else if (nodeNumber == 1 && taskNodeLocation == 1){
		simgrid::s4u::this_actor::sleep_for(taskTotalTime*0.001);
	}
}


inline static void communicationsFunction(Task* someTask, int taskPayload, int taskNodeLocation){

	/* Own version of SimGrids' Sent_to */
	auto send_to = [](auto destination, auto byte_amount){
		simgrid::s4u::Host* source = simgrid::s4u::Host::current();
		std::vector<simgrid::s4u::Host*> m_host_list = {source, destination  };
		std::vector<double> flops_amount = {0, 0};
		std::vector<double> bytes_amount = {0, byte_amount, 0, 0};
		return simgrid::s4u::this_actor::exec_init(m_host_list, flops_amount, bytes_amount);
	};

	simgrid::s4u::ExecPtr sender;
	simgrid::s4u::ExecPtr sender1;
	simgrid::s4u::ExecPtr sender2;
	simgrid::s4u::ExecPtr sender3;
	int distant = 0;
	int distant1 = 0;
	int distant2 = 0;
	int distant3 = 0;

	if (!someTask->name.empty() && someTask->name != "dplgsy"){
		if (taskPayload == 1){
			simgrid::s4u::Host* cpuReceiver;
			if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 0){
				cpuReceiver = simgrid::s4u::Host::by_name(machines[nMachines-1]);
				sender = send_to(cpuReceiver, bSize*bSize*8);
				sender->start();
				sender->wait();
			} else if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 1){
				cpuReceiver = simgrid::s4u::Host::by_name(machines[0]);
				sender = send_to(cpuReceiver, bSize*bSize*8);
				sender->start();
				sender->wait();
			} else if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 0){
				cpuReceiver = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
				sender = send_to(cpuReceiver, bSize*bSize*8);
				sender->start();
				sender->wait();
			} else if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 1){
				cpuReceiver = simgrid::s4u::Host::by_name(machines[nMachines-1]);
				sender = send_to(cpuReceiver, bSize*bSize*8);
				sender->start();
				sender->wait();
			}
		}
		else if (taskPayload == 2){
			simgrid::s4u::Host* cpuReceiver1;
			// RW
			if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 0){
				distant1 = 1;
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
			} else if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 1){
				distant1 = 1;
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[0]);
			}
			if (distant1 == 1){
				sender1 = send_to(cpuReceiver1, taskPayload*bSize*bSize*8);
				sender1->start();
				sender1->wait();
			} else if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 0){
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
				sender = send_to(cpuReceiver1, bSize*bSize*8);
				sender->start();
				sender->wait();
			} else if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 1){
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
				sender = send_to(cpuReceiver1, bSize*bSize*8);
				sender->start();
				sender->wait();
			}
		}
		else if (taskPayload == 3 || taskPayload == 4){
			simgrid::s4u::Host* cpuReceiver1;
			simgrid::s4u::Host* cpuReceiver2;
			simgrid::s4u::Host* cpuReceiver3;
			if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 0){
				distant1 = 1;
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
			} else if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 1){
				distant1 = 1;
				cpuReceiver1 = simgrid::s4u::Host::by_name(machines[0]);
			}
			if (someTask->memoryOpNode2 == 1 && taskNodeLocation == 0){
				distant2 = 1;
				cpuReceiver2 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
			} else if (someTask->memoryOpNode2 == 0 && taskNodeLocation == 1){
				distant2 = 1;
				cpuReceiver2 = simgrid::s4u::Host::by_name(machines[0]);
			}
			if (someTask->memoryOpNode3 == 1 && taskNodeLocation == 0){
				distant3 = 1;
				cpuReceiver3 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
			} else if (someTask->memoryOpNode3 == 0 && taskNodeLocation == 1){
				distant3 = 1;
				cpuReceiver3 = simgrid::s4u::Host::by_name(machines[0]);
			}
			if (taskPayload == 3){
				if (distant1 == 0 && distant2 == 1){
					if (someTask->mode3.empty() && someTask->mode2 == "RW"){   // R+RW
						sender1 = send_to(cpuReceiver2, 2*bSize*bSize*8);
						sender1->start();
						sender1->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				}
				else if (distant1 == 1 && distant2 == 0){
					if (someTask->mode3.empty() && someTask->mode2 == "RW"){   // R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender1->start();
						sender1->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				}
				else if (distant1 == 1 && distant2 == 1){
					if (someTask->mode3.empty() && someTask->mode2 == "RW"){   // R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender2 = send_to(cpuReceiver2, 2*bSize*bSize*8);
						sender1->start();
						sender2->start();
						sender1->wait();
						sender2->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else {
					if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 0){
						cpuReceiver1 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
					} else if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 1){
						cpuReceiver1 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
					}
					if (someTask->memoryOpNode2 == 0 && taskNodeLocation == 0){
						cpuReceiver2 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
						sender2 = send_to(cpuReceiver2, 2*bSize*bSize*8);
					} else if (someTask->memoryOpNode2 == 1 && taskNodeLocation == 1){
						cpuReceiver2 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
						sender2 = send_to(cpuReceiver2, 2*bSize*bSize*8);
					}
					sender1->start();
					sender2->start();
					sender1->wait();
					sender2->wait();
				}
			} else if (taskPayload == 4){
				if (distant1 == 1 && distant2 == 1 && distant3 == 1){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender2 = send_to(cpuReceiver2, bSize*bSize*8);
						sender3 = send_to(cpuReceiver3, 2*bSize*bSize*8);
						sender1->start();
						sender2->start();
						sender3->start();
						sender1->wait();
						sender2->wait();
						sender3->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 1 && distant2 == 1 && distant3 == 0){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender2 = send_to(cpuReceiver2, bSize*bSize*8);
						sender1->start();
						sender2->start();
						sender1->wait();
						sender2->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 1 && distant2 == 0 && distant3 == 1){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender2 = send_to(cpuReceiver3, 2*bSize*bSize*8);
						sender1->start();
						sender2->start();
						sender1->wait();
						sender2->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 0 && distant2 == 1 && distant3 == 1){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver2, bSize*bSize*8);
						sender2 = send_to(cpuReceiver3, 2*bSize*bSize*8);
						sender1->start();
						sender2->start();
						sender1->wait();
						sender2->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 0 && distant2 == 0 && distant3 == 1){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver3, 2*bSize*bSize*8);
						sender1->start();
						sender1->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 0 && distant2 == 1 && distant3 == 0){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver2, bSize*bSize*8);
						sender1->start();
						sender1->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else if (distant1 == 1 && distant2 == 0 && distant3 == 0){
					if (someTask->mode3 == "RW"){                           // R+R+RW
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
						sender1->start();
						sender1->wait();
					} else {
						XBT_INFO("Error: check dependancies and memory operations.");
					}
				} else {
					if (someTask->memoryOpNode1 == 0 && taskNodeLocation == 0){
						cpuReceiver1 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
					} else if (someTask->memoryOpNode1 == 1 && taskNodeLocation == 1){
						cpuReceiver1 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
						sender1 = send_to(cpuReceiver1, bSize*bSize*8);
					}
					if (someTask->memoryOpNode2 == 0 && taskNodeLocation == 0){
						cpuReceiver2 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
						sender2 = send_to(cpuReceiver2, 2*bSize*bSize*8);
					} else if (someTask->memoryOpNode2 == 1 && taskNodeLocation == 1){
						cpuReceiver2 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
						sender2 = send_to(cpuReceiver2, 2*bSize*bSize*8);
					}
					if (someTask->memoryOpNode3 == 0 && taskNodeLocation == 0){
						cpuReceiver3 = simgrid::s4u::Host::by_name(machines[(nMachines/2) - 1]);
						sender3 = send_to(cpuReceiver3, 2*bSize*bSize*8);
					} else if (someTask->memoryOpNode3 == 1 && taskNodeLocation == 1){
						cpuReceiver3 = simgrid::s4u::Host::by_name(machines[nMachines-1]);
						sender3 = send_to(cpuReceiver3, 2*bSize*bSize*8);
					}
					sender1->start();
					sender2->start();
					sender3->start();
					sender1->wait();
					sender2->wait();
					sender3->wait();
				}
			}
		}
	}
}
