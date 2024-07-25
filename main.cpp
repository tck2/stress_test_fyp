#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "car.h"
#include "node.h"
#include "participant.h"
#include "Solution.h"
#include "TestCaseGenerator.h"
#include "GeneticAlgorithm.h"
#include "reqnode.h"
#include "Request.h"
#include "Time.h"
#include "omp.h"
#include <ctime>
#include <windows.h>
#include <psapi.h>
#include <atomic>
#include <memory>

using namespace std;
//typedef unsigned char byte;
//typedef std::byte cs_byte;
//vector <vector<pair<pair<car*, vector<request*>>, pair<car*, vector<request*>>>>> tabuList; 

void printRoute(Solution, string);
void printMemoryUsage();
Solution clearAll(Solution);
Solution demoDynamic(Solution);

int main() {
	string cont = "yes";

	Solution s = Solution(to_string(0));

	vector <node*> nList;

	testCase tc = testCase("0");

	//generate nodes
	nList = tc.genNode(nList);
	//cout << nList.size() << "\n";
	//generate participant
	//pair <vector<participant*>, vector<node*>> curr;
	s.req = tc.generateP(nList, s.req);
	s.SortRequest();
	//s.PList = curr.first;
	//nList = curr.second;
	//cout << s.PList.size() << "\n";
	//generate car
	s.CList = tc.genC();
	//cout << s.CList.size() << "\n";

	GeneticAlgorithm ga = GeneticAlgorithm("0");

	ofstream out("node.txt");

	if (!out.is_open()) {
		cout << "Failed to open file\n";
	}

	out << "Node list:\n\n";

	out.close();

	for (int i = 0; i < nList.size(); i++) {

		nList[i]->printNode();
		//out << "\n";
	}

	//restrict that the start time cannot less than the previous end time
	for (auto p : s.PList) {
		p->adjustReq();
	}

	s.printPList();


	//weights for objective functions
	int e = 250;
	int d = 15;
	int l = 1;
	int c = 35;

	s.CList = ga.initialization(s);
	s.calcIndicators();
	s.calcFitness(e, d, l, c);
	s.printPenalty();

	do {
		s.acc = true;
		for (int i = 0; i < s.CList.size(); i++) {
			if (!s.CList[i]->qualify) {
				s.acc = false;
				break;
			}
		}
		if (s.acc) break;
		else {
			//cout << "\nReaching here1\n";
			for (int i = 0; i < s.CList.size(); i++) {
				s.CList[i]->route.clear();
				s.CList[i]->pList = 0;
				s.CList[i]->darplike = 0;
				s.CList[i]->costs = 0;
				s.CList[i]->early = 0;
				s.CList[i]->late = 0;
				s.CList[i]->qualify = true;
				s.CList[i]->maxWait = 0;
			}
			for (int k = 0; k < s.PList.size(); k++) {
				s.PList[k]->cost = 0;
				s.PList[k]->earliness = 0;
				s.PList[k]->lateness = 0;
			}
			//cout << "\nReaching here2\n";
			s.totalCost = 0;
			s.totaldarpcase = 0;
			s.totalEarly = 0;
			s.totalLate = 0;
			s.maxWait = 0;
			s.acc = true;

			s.CList = ga.methodChooser(s, 1, 100);
			//cout << "\nReaching here3\n";
			for (auto i : s.CList) {
				i->insertRoute(false, "n.txt");
			}

			s.calcIndicators();
			s.calcFitness(e, d, l, c);
			//s.printPenalty();
		}
		//cout << "\nReaching here4\n";
	} while (!s.acc);

	ofstream o("initial_solution.txt");

	if (!o.is_open()) {
		cout << "Failed to open file\n";
	}

	o << "Route Schedule:\n\n";

	o.close();

	printRoute(s, "initial_solution.txt");
	s.printWaitTimeCostDarp("initial_solution.txt");
	//print the satisfication report of all participants
	s.printPSatisf();
	Solution initial = s;

	cout << "Score of initial solution: " << s.fitness << "\n";
	Solution best = s;
	int temperature = 7;

	while (cont == "yes") {
		int ii = 0;
		s = initial;

		vector <int> w;
		w.emplace_back(e);
		w.emplace_back(d);
		w.emplace_back(l);
		w.emplace_back(c);
		

		string optim = "yes";
		string cont1 = "no";

		bool cont2 = true;
		vector <vector<int>> ver;
		vector<Solution> solList;

		//s = clearAll(s);
			
		for (int j = 0; j < s.CList.size(); j++) {
			for (int k = 0; k < s.CList[j]->route.size(); k++) {
				//cout << "hahahaha1\n";
				delete s.CList[j]->route[k];
				s.CList[j]->route[k] = NULL;
			}
		}
			
		s = clearAll(s);

		best = ga.optimization(s, initial, 1, 2, w, true);

		cout << "Continue? (yes/no) ";
		cin >> cont;
		ver.clear();
		best = initial;
		

	}
	string dyna;

	cout << "Enter into dynamic scenario simulation? (yes/no) ";
	cin >> dyna;

	//simulation of dynamic scenario
	if (dyna == "yes") {
		best = demoDynamic(best);
	}

	return 0;
}

void printRoute(Solution s, string filename) {

	ofstream out(filename);
	if (!out.is_open()) {
		cout << "Failed to open file\n";
	}
	out << "Route Schedule: \n\n";
	
	for (int i = 0; i < s.CList.size(); i++) {
		out << "Car " << s.CList[i]->ID << ": ";
		for (int j = 0; j < s.CList[i]->route.size(); j++) {
			reqNode* r = s.CList[i]->route[j];
			out << "{ Actual: " << r->t.getTime() << ", Intended: " << r->intended.getTime() << "; Node " << r->n->name << "; ";
			out << "Type: " << r->type << " ; ";
			if (r->driver != NULL) {
				out << "D: DARP" << ", " << r->pList << "P ";
			}
			else out << "D: Norm" << ", " << r->pList<< "P ";
			
			//int size = r->passenger.size();
			//for (int k = 0; k < size; k++) {
			//	if (k == size - 1) out << r->passenger[k]->id;
			//	else out << r->passenger[k]->id << ", ";
			//}
			out << " } --> ";
		}
		out << "\n\n";
	}

	out.close();
}


void printMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE process = GetCurrentProcess();

	if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
	{
		cout << "\nMemory usage: " << pmc.WorkingSetSize / (1048576) << " megabytes\n";
		cout << "----------------------------------------------------------------------\n\n";
	}
	CloseHandle(process);
}


Solution clearAll(Solution s) {
	for (int i = 0; i < s.CList.size(); i++) {
		s.CList[i]->route.clear();
		s.CList[i]->pList = 0;
		s.CList[i]->darplike = 0;
		s.CList[i]->costs = 0;
		s.CList[i]->early = 0;
		s.CList[i]->late = 0;
		s.CList[i]->qualify = true;
		s.CList[i]->maxWait = 0;
	}
	for (int k = 0; k < s.PList.size(); k++) {
		s.PList[k]->cost = 0;
		s.PList[k]->earliness = 0;
		s.PList[k]->lateness = 0;
	}
	//cout << "\nReaching here2\n";
	s.totalCost = 0;
	s.totaldarpcase = 0;
	s.totalEarly = 0;
	s.totalLate = 0;
	s.acc = true;
	s.maxWait = 0;

	return s;
}

Solution demoDynamic(Solution s) {
	Time t = Time(0, 8);
	int remain = 0;
	cout << t.getTime() << "\n";
	while (t.hour != 18 || t.minute != 45) {
		
		for (auto c : s.CList) {
			for (int s = 0; s < c->service.size(); s++) {
				if (c->service[s] == NULL) {
					if (s == 0) {
						c->service.erase(c->service.begin());
						//cout << "Not removed properly.\n";
					}
				}
				else if (c->service[s]->endTime.compSameTime(t)) {
					delete c->service[s];
					c->service[s] = NULL;
					c->service.erase(c->service.begin());
					//break;
				}
			}
		}
		if (t.minute % 15 == 0) {
			for (auto c : s.CList) {
				remain += c->service.size();
			}
			cout << "Now is " << t.getTime()<< "; remaining " << remain << " requests to be served.\n";
			remain = 0;
		}
		t.addTime(1);
		
		//won't allow lateness, so as long as a Request's end time is reached, can just dlt that Request
	}
	for (auto c : s.CList) {
		remain += c->service.size();
		if (c->service.size() > 0) {
			for (auto ser : c->service) {
				cout << "Error handling " << ser->endTime.getTime() << "\n";
			}
		}
	}
	cout << "Now is " << t.getTime() << "; remaining " << remain << " requests to be served.\n";
	
	return s;
}
