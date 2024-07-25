#pragma once

#include "participant.h"
#include "node.h"
#include "Time.h"
#include <vector>

class node;
class Time;
class participant;

class reqNode {
public:
	node* n;
	Time t;
	participant* p;
	Time intended;
	int pList;
	//to know if this node is there because of darp drivers
	bool darp;
	//number of participant currently on car
	//this can be added while adding the services as reqnode into route list
	//no need to wait until when calculating actual time then do
	int pNum;
	//cost for each reqnode
	float cost;
	vector <participant*> passenger;
	participant* driver;
	char type;

	reqNode() {
		n = NULL;
		p = NULL;
		darp = false;
		pNum = 0;
		cost = 0;
		type = 's';
		pList = 0;
	}

	reqNode(node* a, participant* b) {
		n = a;
		p = b;
		darp = false;
		pNum = 0;
		cost = 0;
		type = 's';
		pList = 0;
	}

	void removePassenger(participant* p) {
		//auto p->getID()
		int s = passenger.size();

		auto it = find(passenger.begin(), passenger.end(), p);

		/*
		for (int i = 0; i < s; i++) {
			if (p->getID() == pList[i]->getID()) {
				pList.erase(pList.begin() + i);
				break;
			}
		}
		*/

		if (it != passenger.end()) passenger.erase(it);

	}
};