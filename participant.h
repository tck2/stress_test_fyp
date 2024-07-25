#pragma once

#include "node.h"
#include "Time.h"
#include "Request.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

class request;
class Time;

class participant {
public:
	string id;
	//modify it so that a participant will not be assigned as driver for 2 consecutive hours or times
	int beenDriver;
	//record the request of each passenger
	vector <request*> req;
	int timesBeDriver;
	//indicate if a participant can drive
	// 0 indicate cannot, 1 indicate can
	int canDrive;
	//record the time when the participant is free (from when to when), will not be included at first
	//vector <pair<float, float>> freeTime;
	//cost to be paid by a participant
	float cost;
	//exsecive wait time to be served
	int earliness;
	int lateness;

public:
	participant() {
		id = "000";
		beenDriver = 0;
		canDrive = 0;
		timesBeDriver = 0;
		cost = 0;
		earliness = 0;
		lateness = 0;
	}

	participant(string ID, int c) {
		id = ID;
		beenDriver = 0;
		canDrive = c;
		timesBeDriver = 0;
		cost = 0;
		earliness = 0;
		lateness = 0;
	}

	string getID() {
		return this->id;
	}

	void addEarly(int t) {
		earliness += t;
	}

	void addLate(int t) {
		lateness += t;
	}

	int getCanDrive() {
		return canDrive;
	}

	int getBeenDriver() {
		return this->beenDriver;
	}

	void setBeenDriver(int b) {
		this->beenDriver = b;
	}

	vector<request*> getRequest() {
		return req;
	}

	request* getReq(int i) {
		return req[i];
	}

	void addSortRequest(request* a) {
		req.emplace_back(a);
		
		sort(req.begin(), req.end(), [&](request* i, request* j) {
			return i->startTime.compareTime(j->startTime);
		});
	}

	//print out request list
	void printReqList() {

		ofstream out;

		out.open("request.txt", ios::app);

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}

		out << "P " << id << " : ";

		for (auto i : req) {
			out << "{ Start: " << i->startTime.getTime();
			out << ", End: " << i->endTime.getTime();
			out << ", from " << i->source->name << " to " << i->destination->name << " };\t";
		}

		out << "\n\n";

		out.close();
	}

	void adjustReq() {
		for (int i = 1; i < req.size(); i++) {
			if (req[i]->startTime.compareTime(req[i - 1]->endTime)) {
				req[i]->startTime.addTime(30);
				req[i]->endTime.addTime(30);
			}
		}
	}

};


