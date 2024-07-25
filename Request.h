#pragma once


#include <vector>
#include <iostream>
#include "Time.h"
#include "node.h"
#include "participant.h"

using namespace std;

class Time;
class participant;
class node;

class request {
public:
	int id;
	node* source;
	node* destination;
	Time startTime;
	Time endTime;
	participant* p;
	bool served;
	//type of request, 0 indicating to request, 1 indicating back request
	//int type;

	request() {
		id = 0;
		source = NULL;
		destination = NULL;
		p = NULL;
		served = false;
	}

	request(int i, node* s, node* d, Time st, Time et, participant* par) {
		id = i;
		source = s;
		destination = d;
		startTime = st;
		endTime = et;
		p = par;
		served = false;
	}
};

