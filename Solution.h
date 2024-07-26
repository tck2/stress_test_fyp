#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "car.h"
#include "node.h"
#include "participant.h"
#include "Time.h"
#include "Request.h"

using namespace std;

class car;
class node;
class participant;
class Time;
class request;

class Solution {
public:
	//the identifier for a solution
	string id;
	//vector<car*> SolutionList;
	//vector<pair<car*, vector<node*>>> SolutionList;
	//total distance travelled for each solution
	float TotalDistance;
	float totalCost;
	int totalEarly;
	int totalLate;
	int totaldarpcase;
	//vector storing all participants
	vector <participant*> PList;
	//vector storing all requests
	//vector storing all cars
	vector <car*> CList;
	//vector <node*> NList;
	float fitness;
	//record the unserved request
	int unserved;
	//record all of the requests
	vector<request*> req;
	//whether to accept the solution or not
	bool acc;
	int maxWait;

public:
	Solution() {
		id = "000";
		TotalDistance = 0;
		totalCost = 0;
		totalEarly = 0;
		totalLate = 0;
		fitness = 0.0;
		totaldarpcase = 0;
		unserved = 0;
		acc = true;
		maxWait = 0;
	}

	Solution(string s) {
		id = s;
		TotalDistance = 0;
		totalCost = 0;
		totalEarly = 0;
		totalLate = 0;
		fitness = 0.0;
		totaldarpcase = 0;
		unserved = 0;
		acc = true;
		maxWait = 0;
	}

	void addSortRequest(request* a) {
		req.emplace_back(a);

		sort(req.begin(), req.end(), [&](request* i, request* j) {
			return i->startTime.compareTime(j->startTime);
		});
	}

	vector<participant*> getPlist() {
		return PList;
	}

	participant* getP(int i) {
		return PList[i];
	}

	void setPList(participant* p) {
		PList.push_back(p);
	}

	void setCList(car* c) {
		CList.push_back(c);
	}

	vector <car*> getCList() {
		return CList;
	}

	car* getC(int i) {
		return CList[i];
	}

	int totalRequest() {
		int req = 0;

		for (auto a : PList) {
			req += a->getRequest().size();
		}

		return req;
	}

	void calcTotalDistance() {
		int s = CList.size();
		TotalDistance = 0;

		for (int i = 0; i < s; i++) {
			int sR = CList[i]->getRouteList().size();
			for (int j = 0; j < sR - 1; j++) {
				TotalDistance += CList[i]->getRoute(j)->n->UCS(CList[i]->getRoute(j)->n, CList[i]->getRoute(j+1)->n);
			}
		}
	}

	void printPList() {
		ofstream out("request.txt");

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}

		out << "Requests of each participant:\n\n";

		out.close();

		for (auto i : PList) {
			i->printReqList();
			out << "\n\n";
		}
	}

	//calculate total cost
	void calcTotalCost() {
		//for (int i = 0; i < PList.size(); i++) {
		//	totalCost += PList[i]->cost;
		//}
		for (int i = 0; i < CList.size(); i++) {
			totalCost += CList[i]->costs;
			//cout << "\nCosts of car " << CList[i]->ID << " is " << CList[i]->costs << "\n";
		}

		penalty();

	}
	//calculate total earliness time
	void calcTotalEarly() {
		for (int i = 0; i < CList.size(); i++) {
			totalEarly += CList[i]->early;
		}
		
	}

	//calculate total lateness time
	void calcTotalLate() {
		//for (int i = 0; i < PList.size(); i++) {
		//	totalLate += PList[i]->lateness;
		//}
		for (auto i : CList) {
			totalLate += i->late;
		}
		totalLate *= 5000;
	}

	void SortRequest() {
		//req.emplace_back(a);

		sort(req.begin(), req.end(), [&](request* i, request* j) {
			return i->startTime.compareTime(j->startTime);
			});
	}

	void printWaitTimeCostDarp(string filename) {

		//calcTotalCost();
		//calcTotalWaitTime();

		ofstream out(filename, ios::app);

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}

		//totalEarly = totalEarly / req.size();
		//totaldarpcase = totaldarpcase / req.size();
		//totalCost = totalCost / req.size();

		out << "\nHighest single request earliness waiting time: " << maxWait << " minutes\n";
		out << "\nAverage earliness waiting time for each request: " << totalEarly / req.size() << " minutes\n";
		out << "\nTotal lateness waiting time for each request: " << totalLate / 5000 << " minutes\n";
		out << "\nAverage cost for each request: RM" << totalCost / req.size() << "\n";
		out << "\nAverage number of darp-like cases: " << totaldarpcase / req.size() << "\n";

		out.close();
	}

	//print out the cost and waiting time of each participant
	void printPSatisf() {
		ofstream out("satisfaction.txt");

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}

		out << "Satisfication report: \n\n\n";

		for (auto i : PList) {
			out << "P" << i->id << ": ";
			out << "Total earliness waiting time is " << i->earliness << "minutes, ";
			out << "Total lateness waiting time is " << i->lateness << "minutes, ";
			out << "Total cost is RM" << i->cost << "\n\n";
		}

		out.close();
	}

	void calcTotalDarp() {
		for (int i = 0; i < CList.size(); i++) {
			totaldarpcase += CList[i]->darplike;
		}

	}
	
	//calculate cost, waiting time, and darp time
	void calcIndicators() {
		calcTotalLate();
		calcTotalCost();
		calcTotalDarp();
		calcTotalEarly();
		updateMaxWait();
		
	}


	//incur penalty on the cost to handle unserved request
	void penalty() {
		int totalUnserved = 0;
		for (int i = 0; i < CList.size(); i++) {
			totalUnserved += CList[i]->unserved;
		}

		unserved = totalUnserved;

		totalCost += (totalUnserved * 5000);

		//calcTotalLate();

		//totalCost += (totalLate * 5000);
	}

	void printPenalty() {
		//penalty();
		cout << "\nTotal unserved requests: " << unserved << '\n';
	}


	//calculate fitness score
	//pass in the time, cost, and darpcase from the initial solution
	void calcFitness(int e, int d, int l, int c) {


		fitness = (totalEarly * e) + (totaldarpcase * d) + (totalCost * c) + (totalLate * l);

		//fitness = fitness / req.size();

	}

	void updateMaxWait() {
		for (int i = 0; i < CList.size(); i++) {
			if (CList[i]->maxWait > maxWait) maxWait = CList[i]->maxWait;
		}
	}
	
};

