#pragma once

#include "car.h"
#include "node.h"
#include "participant.h"
#include "Solution.h"
#include "TestCaseGenerator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <windows.h>
#include <psapi.h>
#include <memory>

using namespace std;

class Solution;
class car;
class node;
class participant;

class GeneticAlgorithm {
	//solution pool
	//vector <Solution> pool;
	//vector storing all available nodes in the map
	//move the below attributes to main()
	//vector <node> ServicePoints;
	string id;
	vector <vector<pair<pair<car*, vector<request*>>, pair<car*, vector<request*>>>>> TLswap;
	vector <vector<pair<pair<car*, vector<request*>>, car*>>> TLmove;

public:
	GeneticAlgorithm() {
		id = "000";
	}

	GeneticAlgorithm(string s) {
		id = s;
	}

	vector <car*> initialization(Solution sol) {
		int sizeP = sol.getPlist().size();
		int sizeC = sol.getCList().size();

		int count = 0;
		//used to access request list of each participant
		int d = 0;
		//used to access the participant list
		int p = 0;
		//distribute each request of each participant evenly to each car
		//here got problem

		for (int i = 0; i < sol.req.size(); i++) {
			sol.CList[count]->addSortService(sol.req[i]);
			if (count + 1 == sol.CList.size()) count = 0;
			else count++;
		}

		for (auto c : sol.CList) {
			c->insertRoute(true, "initial_solution.txt");
		}


		return sol.CList;
	}

	void printSchedule(Solution sol) {
		ofstream out("output.txt");

		int sizeC = sol.getCList().size();

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}

		out << "Schedule:\n\n";

		out.close();

		for (auto i : sol.getCList()) {
			out << "Car " << i->getID() << ": ";
			i->insertRoute(true, "output.txt");
			out << "\n\n";
		}

	}

	//in all functions below, the "act" means the swap/move request will be performed between how many set of car in an optimization loop
	vector<car*> swapRequest(Solution sol, int act, int iter) {
		random_device rd;
		mt19937 gen(rd());
		int csize = sol.CList.size() - 1;
		uniform_int_distribution<int> carChoice(0, csize);
		//swap how many request
		uniform_int_distribution<int> actCount(1, 10);
		uniform_int_distribution<int> swapcount(1, 1);
		//uniform_int_distribution<int> actProb(1, 20);
		//record which vehicle had been swapped
		vector<int> swapped;
		vector <int> accessed1;
		vector <int> accessed2;

		int a, b;
		int req1, req2;

		int act1 = swapcount(rd);

		for (int i = 0; i < act1; i++) {
			//cout << "1 - looping now...\n";
			//generate random number of count the swap will be taken
			//50% - 2, 30% - 3, 20% - 1
			int swapProb = actCount(gen);
			int swapCount = 0;
			if (swapProb <= 10) swapCount = 1;
			//else if (swapProb <= 8 && swapProb > 5) swapCount = 2;
			else swapCount = 2;
			//else if (swapProb >= 6 && swapProb < 9) swapCount = 3;
			//else swapCount = 1;
			//choose cars to swap request
			
			if (swapped.size() != 0) {
				
					a = carChoice(gen);
					auto it = find(swapped.begin(), swapped.end(), a);
					//avoid doing swapping on accessed car
					while (it != swapped.end() || sol.CList[a]->service.size() <= swapCount) {
						//cout << "1-trying...\n";
						a = carChoice(gen);
						it = find(swapped.begin(), swapped.end(), a);
					}

					b = carChoice(gen);
					auto it2 = find(swapped.begin(), swapped.end(), b);
					//avoid the same thing as previous on a, and avoid a = b
					while ((b == a || it2 != swapped.end()) || sol.CList[b]->service.size() <= swapCount) {
						//cout << "2-trying...\n";
						b = carChoice(gen);
						it2 = find(swapped.begin(), swapped.end(), b);
					}
			}
			else {
				a = carChoice(gen);
				while (sol.CList[a]->service.size() <= swapCount) {
					a = carChoice(gen);
				}
				b = carChoice(gen);
				while (b == a || sol.CList[b]->service.size() <= swapCount) {
					//cout << "3-trying...\n";
					b = carChoice(gen);
				}
				//record the swapped vehicle number into the swapped list
				swapped.emplace_back(a);
				swapped.emplace_back(b);
			}
			//cout << "\nreach 1\n";
			//cout << "\na" << a << ", b" << b << "\n";

			//start doing swapping
			//minus with 1 to avoid vector subscript out of range
			int size1 = sol.CList[a]->service.size() - 1;
			int size2 = sol.CList[b]->service.size() - 1;

			

			//random request to be swapped
			uniform_int_distribution<int> reqA(0, size1);
			uniform_int_distribution<int> reqB(0, size2);

			for (int j = 0; j < swapCount; j++) {
				//cout << "2 - looping now...\n";
				//for car A
				req1 = reqA(gen);
				
				//check if the request was accessed
				if (accessed1.size() != 0) {
					auto it = find(accessed1.begin(), accessed1.end(), req1);
					while (it != accessed1.end()) {
						//cout << "4-trying...\n";
						req1 = reqA(gen);
						it = find(accessed1.begin(), accessed1.end(), req1);
					}
				}

				//for car B
				req2 = reqB(gen);

				//check if the request is accessed
				if (accessed2.size() != 0) {
					auto it2 = find(accessed2.begin(), accessed2.end(), req2);
					while (it2 != accessed2.end()) {
						//cout << "5-trying...\n";
						req2 = reqB(gen);
						it2 = find(accessed2.begin(), accessed2.end(), req2);
					}
				}

					
				//} while (checkTLswap(sol.CList[a], sol.CList[a]->service[req1], sol.CList[b], sol.CList[b]->service[req2]));

				accessed1.emplace_back(req1);
				accessed2.emplace_back(req2);



				//swap the request between them
				swap(sol.CList[a]->service[req1], sol.CList[b]->service[req2]);

				//sort the requests assigned to each car by request start time
				sol.CList[a]->sortService();
				sol.CList[b]->sortService();
			}
			//cout << "reach 2\n";

		}

		return sol.CList;
	}

	vector<car*> moveRequest(Solution sol, int act, int iter) {
		random_device rd;
		mt19937 gen(rd());
		int csize = sol.CList.size() - 1;
		uniform_int_distribution<int> carChoice(0, csize);
		//move how many request
		//uniform_int_distribution<int> actCount(1, 2);
		//record which vehicle had been moved
		vector<int> moved;

		int a, b;

		for (int i = 0; i < 1; i++) {
			if (moved.size() != 0) {
				a = carChoice(gen);
				auto it = find(moved.begin(), moved.end(), a);
				//avoid doing swapping on accessed car
				while (it != moved.end() || sol.CList[a]->service.size() == 0) {
					a = carChoice(gen);
					it = find(moved.begin(), moved.end(), a);
				}

				b = carChoice(gen);
				auto it2 = find(moved.begin(), moved.end(), b);
				//avoid the same thing as previous on a, and avoid a = b
				while (b == a || it2 != moved.end()) {
					b = carChoice(gen);
					it2 = find(moved.begin(), moved.end(), b);
				}
			}
			else {
				a = carChoice(gen);
				b = carChoice(gen);
				while (sol.CList[a]->service.size() == 0) {
					a = carChoice(gen);
				}
				while (b == a) {
					b = carChoice(gen);
				}
				//record the swapped vehicle number into the swapped list
				moved.emplace_back(a);
				moved.emplace_back(b);
			}

			//start moving request
			//generate random number of count of movement
			//int moveCount = actCount(gen);
			//single sided: only a move to b within a movement action
			for (int j = 0; j < 1; j++) {
				//minus with 1 to avoid vector subscript out of range
				int size1 = sol.CList[a]->service.size() - 1;
				uniform_int_distribution<int> moveA(0, size1);

				int toMove = moveA(gen);
				//find the request to move
				auto it2 = find(sol.CList[a]->service.begin(), sol.CList[a]->service.end(), sol.CList[a]->service[toMove]);
				//move the request to another car
				sol.CList[b]->service.emplace_back(sol.CList[a]->service[toMove]);
				//remove the request from the first car
				sol.CList[a]->service.erase(it2);
				//sort the request of the car which request had been move to
				sol.CList[b]->sortService();
			}

		}



		return sol.CList;
	}

	vector<car*> methodChooser(Solution sol, int act, int iter) {
		random_device rd;
		mt19937 gen(rd());

		uniform_int_distribution<int> prob(1, 20);

		int choice = prob(gen);

		//update tabu list
		//if ((iter + 1) % 5 == 0) {
			//TLmove.erase(TLmove.begin());
			//TLswap.erase(TLswap.begin());

		//}

		//probability, 50% choose swap request, 30% choose move request, 20% choose combination between both
		if (choice <= 10) {
			//cout << "1 is chosen\n";
			sol.CList = swapRequest(sol, act, iter);
		}
		else {
			//cout << "2 is chosen\n";
			sol.CList = moveRequest(sol, act, iter);
		}

		/*
		* else if (choice > 16 && choice <= 19) {
			cout << "2 is chosen\n";
			sol.CList = moveRequest(sol, act);
		}
		* else {
			cout << "3 is chosen\n";
			sol.CList = swapRequest(sol, act);
			sol.CList = moveRequest(sol, act);
		}
		*/


		return sol.CList;
	}

	/*
	pair <Solution, vector<vector<int>>> optimization(Solution initial, int act, int loopCount, vector<int> w, string optim) {
		Solution best = initial;
		Solution curr;

		random_device rd;
		mt19937 gen(rd());

		int counter = 0;
		int iter = 0;
		float best_score = 100000000.00;
		int temperature = 6;

		uniform_int_distribution<int> prob(1, 10);
		//initial.calcIndicators();
		//best.calcIndicators();
		//best.calcFitness(w[0], w[1], w[2], w[3]);

		vector<Solution> solList;
		//curr.calcIndicators();

		for (int i = 0; i < 5; i++) {
			solList.emplace_back(initial);
		}

		vector <vector<int>> ver;

		while (counter < 200) {
			iter++;
			for (int i = 0; i < 5; i++) {
				//clear the existing record in the solution to be optimized
				for (int j = 0; j < solList[i].CList.size(); j++) {
					solList[i].CList[j]->route.clear();
					solList[i].CList[j]->pList.clear();
					solList[i].CList[j]->darplike = 0;
				}
				for (int k = 0; k < solList[i].PList.size(); k++) {
					solList[i].PList[k]->cost = 0;
					solList[i].PList[k]->earliness = 0;
					solList[i].PList[k]->lateness = 0;
				}

				solList[i].totalCost = 0;
				solList[i].totaldarpcase = 0;
				solList[i].totalEarly = 0;
				solList[i].totalLate = 0;

				//solList[i].calcIndicators();

				//cout << "after clearing, fitness = " << solList[i].totalCost + solList[i].totaldarpcase + solList[i].totalEarly + solList[i].totalLate << "\n";

				curr = solList[i];
				curr.CList = methodChooser(curr, act);
				//cout << "\nreach 111\n";
				//print out the best route schedule
				ofstream o1("output.txt");

				if (!o1.is_open()) {
					cout << "Failed to open file\n";
				}

				o1 << "Route Schedule:\n\n";

				o1.close();
				//arrange the route for CList
				for (auto c : curr.CList) {
					c->insertRoute(true, "output.txt");
				}

				curr.calcIndicators();
				curr.calcFitness(w[0], w[1], w[2], w[3]);



				if (curr.fitness > solList[i].fitness) {
					//best = curr;
					solList[i] = curr;
					//cout << "fitness: " << best.fitness <<"curr: " << curr.fitness << "\n";
				}
				else {
					int update = prob(gen);

					if ((iter != 0) && (iter % 150 == 0)) {
						temperature++;
					}

					if (update > temperature) {
						solList[i] = curr;
					}
				}
				//if (update < 4) curr = best;
				//if the new solution is not better, start optimize again starting from the current best solution

			}

			sort(solList.begin(), solList.end(), [&](Solution j, Solution k) {
				return j.fitness < k.fitness;
				});

			float best_score_curr = solList[0].fitness;


			if (best_score_curr < best_score) {
				best_score = best_score_curr;
				counter = 0;
				best = solList[0];
				vector <int> vv;
				vv.emplace_back(int(best.fitness));
				vv.emplace_back(best.totalEarly);
				vv.emplace_back(best.totaldarpcase);
				vv.emplace_back(best.totalLate);
				vv.emplace_back(int(best.totalCost));
				//, , , int
				ver.emplace_back(vv);
				if (ver.size() > 1) {
					//sort the verification metrics
					sort(ver.begin(), ver.end(), [&](vector<int> i, vector<int> j) {
						return i[0] < j[0];
						});
				}
			}
			else counter++;

			//every time only retain the top 3 ranked solution metrics
			if (ver.size() > 3) {
				vector<vector<int>> vc;
				vc.emplace_back(ver[0]);
				vc.emplace_back(ver[1]);
				vc.emplace_back(ver[2]);
				ver.clear();
				ver = vc;
			}

			if (optim == "yes") {
				if (iter % 5 == 0) {
					cout << "Iteration: " << iter << "; lowest inconvenience score: " << best_score << "\n";
					//cout << "Counter is now " << counter << "\n";
				}
			}

		}
		pair <Solution, vector<vector<int>>> ret;

		vector<int> iterRec;
		iterRec.emplace_back(iter);

		ver.emplace_back(iterRec);

		ret = { best, ver };

		//best = solList[0];

		return ret;
	}
	*/

	Solution optimization(Solution s, Solution initial, int loopCount, int Iter, vector<int> w, bool optim) {
		int ii = 0;
		vector <Solution> solList;
		Solution best;
		int temperature = 7;
		vector <vector<float>> ver;
		float best_score = s.fitness;

		for (int i = 0; i < 5; i++) {
			solList.emplace_back(s);
		}

		while (ii < loopCount) {

			//clean(initialize) the solution
			s = clearAll(s);

			//cout << "fitness score: " << s.fitness << "\n";

			cout << "Score of initial solution: " << s.fitness / s.req.size() << "\n";

			//Solution best = s;
			Solution cur;

			random_device rd;
			mt19937 gen(rd());

			int counter = 0;
			int iter = 0;


			uniform_int_distribution<int> prob(1, 10);
			uniform_int_distribution<int> u_prob(1, 17);
			//initial.calcIndicators();
			//best.calcIndicators();
			//best.calcFitness(w[0], w[1], w[2], w[3]);

			//curr.calcIndicators();
			clock_t start = clock();

			//vector <Solution> best_list;

			while (counter < Iter) {
				iter++;
				//vector <Solution> sol1;


				for (int i = 0; i < solList.size(); i++) {
					//clear the existing record in the solution to be optimized
					solList[i] = clearAll(solList[i]);

					//cout << "after clearing, fitness = " << solList[i].totalCost + solList[i].totaldarpcase + solList[i].totalEarly + solList[i].totalLate << "\n";

					cur = solList[i];
					if (!cur.acc) cout << "Something wrong here, 1...\n";
					for (int j = 0; j < cur.CList.size(); j++) {
						if (!cur.CList[j]->qualify) {
							cout << "Something wrong here, 2...\n";
							break;
						}
						if (cur.CList[j]->maxWait != 0) {
							cout << "Something wrong here, 3...\n";
							break;
						}
					}
					cur.CList = methodChooser(cur, 1, iter);

					for (auto c : cur.CList) {
						c->insertRoute(false, "output.txt");
					}

					cur.calcIndicators();
					cur.calcFitness(w[0], w[1], w[2], w[3]);


					if (cur.fitness < solList[i].fitness) {
						//best = curr;
						for (int q = 0; q < solList[i].CList.size(); q++) {
							if (!solList[i].CList[q]->qualify) {
								solList[i].acc = false;
								break;
							}
						}
						if (solList[i].acc) solList[i] = cur;
						//else cout << "Rejected\n";
						//cout << "fitness: " << best.fitness <<"curr: " << curr.fitness << "\n";
					}
					else {
						for (int q = 0; q < solList[i].CList.size(); q++) {
							if (!solList[i].CList[q]->qualify) {
								solList[i].acc = false;
								break;
							}
						}

						if (solList[i].acc) {
							int update = prob(gen);
							//cout << "Probability of update: " << update << "\n";
							if (update > temperature) {
								solList[i] = cur;
							}
						}
						//else cout << "Rejected\n";
					}
					//if (update < 4) curr = best;
					//if the new solution is not better, start optimize again starting from the current best solution

				}

				if ((iter != 0) && (iter % 15 == 0)) {
					temperature += 1;
					//cout << "current temp: " << temperature << "\n";
				}

				sort(solList.begin(), solList.end(), [&](Solution j, Solution k) {
					return j.fitness < k.fitness;
					});

				float best_score_curr = solList[0].fitness;


				if (best_score_curr < best_score) {
					best_score = best_score_curr;
					counter = 0;
					best = solList[0];

					vector <float> vv;
					ver.emplace_back(vv);

					ver[ver.size() - 1].emplace_back(best.fitness);
					ver[ver.size() - 1].emplace_back(float(best.totalEarly));
					ver[ver.size() - 1].emplace_back(float(best.totaldarpcase));
					ver[ver.size() - 1].emplace_back(float(best.totalLate));
					ver[ver.size() - 1].emplace_back(best.totalCost);
					ver[ver.size() - 1].emplace_back(float(best.maxWait));
					//, , , int

					if (ver.size() > 1) {
						//sort the verification metrics
						sort(ver.begin(), ver.end(), [&](vector<float> i, vector<float> j) {
							return i[0] < j[0];
							});
					}
				}
				else counter++;

				//every time only retain the top 3 ranked solution metrics
				if (ver.size() > 3) {
					ver.erase(ver.begin() + 3, ver.end());
				}

				if (optim) {
					if (iter % 1 == 0) {
						cout << "Size of solList: " << solList.size() << ", ";
						cout << "Iteration: " << iter << "; lowest inconvenience score: " << solList[0].fitness / best.req.size() << "; " << solList[1].fitness / best.req.size()
							<< "; " << solList[2].fitness / best.req.size() << "; " << solList[3].fitness / best.req.size() << "; " << solList[4].fitness / best.req.size() << "\n";
						//cout << "Counter is now " << counter << "\n";
					}
				}
				//solList[0].fitness
				//best_score

				//try to put the delete here
				for (int i = 0; i < solList.size(); i++) {
					for (int j = 0; j < solList[i].CList.size(); j++) {
						for (int k = 0; k < solList[i].CList[j]->route.size(); k++) {
							//cout << "hahahaha1\n";
							delete solList[i].CList[j]->route[k];
							solList[i].CList[j]->route[k] = NULL;
						}
					}
				}

			}
			clock_t stop = clock();

			cout << "\nTime taken: " << ((double)stop - start) / CLOCKS_PER_SEC << " seconds";
			cout << "\nNumber of iterations: " << iter;
			cout << "\n======================================================================\n";
			cout << "Initial solution: ";
			cout << "\nInconvenience score: " << initial.fitness / initial.req.size();
			cout << "\nTotal earliness wait time: " << initial.totalEarly;
			cout << "\nTotal darp-like cases: " << initial.totaldarpcase;
			cout << "\nTotal lateness wait time: " << initial.totalLate / 5000;
			cout << "\nTotal costs: " << initial.totalCost;
			cout << "\nMaximum single request wait time: " << initial.maxWait;
			cout << "\n----------------------------------------------------------------------\n";
			for (int z = 0; z < ver.size(); z++) {
				cout << "Solution Rank " << z + 1 << ": ";
				cout << "\nInconvenience score: " << ver[z][0] / initial.req.size();
				cout << "\nTotal earliness wait time: " << ver[z][1];
				cout << "\nTotal darp-like cases: " << ver[z][2];
				cout << "\nTotal lateness wait time: " << ver[z][3] / 5000;
				cout << "\nTotal costs: " << ver[z][4];
				cout << "\nMaximum single request wait time: " << ver[z][5];
				cout << "\n----------------------------------------------------------------------\n";
			}

			//print the satisfication metrics for the best solution
			best.printPenalty();
			best = clearAll(best);
			//cout << "haha1\n";
			for (auto c : best.CList) {
				//cout << "Running..\n";
				//c->route.clear();
				c->insertRoute(false, "x.txt");
			}
			//cout << "haha12\n";
			printRoute(best, "output.txt");
			//cout << "haha12\n";
			best.printWaitTimeCostDarp("output.txt");
			//cout << "haha13\n";
			for (auto c : best.CList) {

				for (auto r : c->route) {
					delete r;
					r = NULL;
				}
				c->route.clear();
			}
			printMemoryUsage();
			//cout << "haha15\n";

			//s = solList[0];
			ii++;
			temperature = 7;
			//ver.clear();
			//cout << "hahahaha1\n";
			//solList.clear();
			//cout << "hahahaha3\n";
		}
		return best;
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
				else out << "D: Norm" << ", " << r->pList << "P ";

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
	
};