#pragma once

#include "participant.h"
#include "node.h"
#include "Time.h"
#include "Request.h"
#include "reqnode.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

class reqNode;
class participant;
class node;
class request;
class Time;

class car {
public:
	string ID;
	participant* driver;
	participant* owner;
	int capacity;
	int speed;
	int darplike;
	//the real time during running, used to record the time the car reach a destination
	float realtime;
	//the location the car is in now
	node* nowLoc;
	//list of passenger on car
	int pList;
	//list of the routes passed through by a car
	vector <reqNode*> route;
	//record who are permitted to drive the car
	vector <participant*> driverList;
	//the list to record the service handled by a car
	vector <request*> service;
	//to record unserved request
	int unserved;
	float costs;
	int late;
	int early;
	bool qualify;
	int maxWait;

public:
	car() {
		ID = "000";
		capacity = 5;
		speed = 30;
		realtime = 0;
		darplike = 0;
		unserved = 0;
		costs = 0;
		late = 0;
		early = 0;
		pList = 0;
		qualify = true; 
		maxWait = 0;
		//driver = "driver";
	}

	//need update
	car(participant* p, string id, int cap) {
		ID = id;
		//driver = o;
		owner = p;
		capacity = cap;
		speed = 30;
		realtime = 0;
		darplike = 0;
		unserved = 0;
		costs = 0;
		late = 0;
		early = 0;
		pList = 0;
		qualify = true;
		maxWait = 0;
		//driver = "driver";
	}

	vector <request*> getSList() {
		return service;
	}

	void addSortService(request* r) {
		service.emplace_back(r);

		sort(service.begin(), service.end(), [&](request* i, request* j) {
			return i->startTime.compareTime(j->startTime);
			});
	}

	void sortService() {

		sort(service.begin(), service.end(), [&](request* i, request* j) {
			return i->startTime.compareTime(j->startTime);
			});
	}

	void setDriverList(participant* p) {
		driverList.push_back(p);
	}

	string getID() {
		return ID;
	}

	int getPList() {
		return pList;
	}

	vector <reqNode*> getRouteList() {
		return route;
	}

	reqNode* getRoute(int i) {
		return route[i];
	}

	void insertRoute(bool print, string filename) {
		vector <request*> interInsert;
		int s = service.size();
		int counter = 0;
		int seat = 1;
		Time t;
		//int c = 0;
		//sortService();
		//nowLoc = service[0]->source;

		while (counter < s) {
			//if car is not at requested node of the next service
			if (route.size() != 0 && service[counter]->source != nowLoc) {

				t = service[counter]->startTime;
				int diff = 0;
				reqNode* p = new reqNode();
				reqNode* p1 = new reqNode();
				//cout << "this\n";
				//if there is already delay in the previous trip, delay the time for the current request
				if (!route[route.size() - 1]->t.compareTime(t)) {
					diff = route[route.size() - 1]->t.calcDiff(t);
				}

				t.addTime(diff);


				participant* drv = new participant("Driver", 1);
				p->n = nowLoc;
				p->driver = drv;
				p->p = drv;
				//p->intended = Time(0, 0);
				p->t = t;
				p->type = 's';
				p->intended = t;
				route.emplace_back(p);

				//record it to do optimization on reducing this case
				//else the car will back at the same point everytime it finish its request
				darplike++;

				//add into the route list of car, signifying the car is able to serve that request
				//route.emplace_back(p);

				//register the passenger to the car

				pList++;
				//print out the point of the route
				//if (print) printRoute(p, filename);
				//cout << "this4\n";
				pair<vector<reqNode*>, int> temp = crossInsert(counter, 1);
				counter = temp.second;

				if (temp.first.size() != 0) {
					temp.first = calcReqNodeDet(p, temp.first, 1, print, filename);

					//insert the whole cross-inserted list into the route vector
					route.insert(route.end(), temp.first.begin(), temp.first.end());
				}
				///cout << "this5\n";
				// add the destination of the driver to the node
				float dist = route[route.size() - 1]->n->UCS(route[route.size() - 1]->n, nowLoc);
				//calculate the time needed from a node to a node (t = s/v)
				//*60 to change to minute
				int minuteDiff = (dist / speed) * 60;
				t = route[route.size() - 1]->t;
				t.addTime(minuteDiff);
				p1->t = t;
				p1->p = drv;
				p1->intended = t;
				p1->driver = drv;
				p1->n = p->n;
				//p1->passenger.emplace_back(route[route.size()-1]->p);
				p1->type = 'd';
				route.emplace_back(p1);

				//add the driver to the available list of destination node
				//addAvaiList(roundDown(t), nowLoc, driver);
				//print out the point of the route

				//remove passenger from car as he/she left
				//removePassenger(p->p);
				//cout << "this6\n";
			}
			//if the car is on the next requested node
			else {
				//cout << "here\n";
				reqNode* point = new reqNode();
				reqNode* point1 = new reqNode();
				point->n = service[counter]->source;
				//update nowLoc
				nowLoc = point->n;
				point->intended = service[counter]->startTime;
				t = service[counter]->startTime;
				//record the destination node and the intended reaching time for the driver
				node* end = service[counter]->destination;
				Time tt = service[counter]->endTime;
				//point->p = service[counter]->p;

				int diff = 0;

				//if there is already delay in the previous trip, delay the whole subsequent trip
				if (route.size() != 0 && route[route.size() - 1]->t.compareTime(t)) {
					diff = route[route.size() - 1]->t.calcDiff(t);
				}

				t.addTime(diff);
				point->t = t;
				//cout << "here2\n";
				//update the earliness time of a participant
				//point->p->addEarly(point->intended.calcDiff(point->t));
				early += point->intended.calcDiff(point->t);
				//update the available driver list of the node
				//removeAvaiList(roundDown(t), point->n, point->p);

				//update driver
				//driver = point->p;
				//change the state of this request as served
				service[counter]->served = true;
				//point->driver = point->p;
				point->type = 's';
				//place the point into the route list
				route.emplace_back(point);
				//register the passenger to the car
				pList++;
				//counter++;
				//print out the point of the route

				counter++;
				// see if the driver can carry anyone with similar request time
				pair<vector<reqNode*>, int> temp = crossInsert(counter, 0);
				//cout << "here3\n";
				counter = temp.second;

				if (temp.first.size() != 0) {
					temp.first = calcReqNodeDet(point, temp.first, 0, print, filename);

					//insert the whole cross-inserted list into the route vector
					route.insert(route.end(), temp.first.begin(), temp.first.end());

				}
				//cout << "here4\n";
				//at the destination of the driver into the route list
				point1->intended = tt;
				float dist = route[route.size() - 1]->n->UCS(route[route.size() - 1]->n, end);
				//calculate the time needed from a node to a node (t = s/v)
				//*60 to change to minute
				int minuteDiff = (dist / speed) * 60;
				t = route[route.size() - 1]->t;
				t.addTime(minuteDiff);
				point1->t = t;
				//update lateness
				//point->p->addLate(point->t.calcDiff(point->intended));
				late += point1->t.calcDiff(point1->intended);
				
				//update node
				point1->n = end;
				//point1->driver = point->p;
				//point1->p = point->p;
				point1->type = 'd';
				route.emplace_back(point1);
				//update the current location of the car
				nowLoc = end;
				//print out the point of the route
				//remove passenger from car as he/she left
				//removePassenger(point->p);
				//update the available driver list for the destination node
				//addAvaiList(roundDown(t), end, point->p);
				//cout << "here5\n";
			}
			//counter++;

		}
		avoidLate();
	}

	//intermix the request of several participants
	pair<vector<reqNode*>, int> crossInsert(int counter, int type) {
		vector <request*> interInsert;
		vector <reqNode*> r;
		reqNode* point = new reqNode();
		request* p = new request();
		int seat = 1;


		//cout << "\nservice list size: " << service.size() << "\n";
		//cout << "counter: " << counter << "\n\n";
		if (counter == service.size()) {
			//cout << "max already\n\n";
			return { r, counter };
		}
		//if driver have destination
		//seach for the request with near start time with the start time of the driver
		if (type == 0) {
			p = service[counter - 1];
		}
		//if driver don't have destination
		if (type == 1) {
			//seach for the request with near start time with the start time of the first passenger instead
			p = service[counter];
			counter++;
			interInsert.emplace_back(p);
			//as already emplaced back first passenger, seat++
			seat++;
		}

		//if (counter != service.size()) {
		//point->p = service[counter]->p;
		//point->n = service[counter]->source;
		if (counter != service.size()) point->t = service[counter]->startTime;
		//point->intended = service[counter]->startTime;

		//search for participant with near request time
		while (p->startTime.calcTimeDiff(12, point->t) && seat <= capacity) {
			if (counter == service.size()) break;
			//change the state of this request as served
			service[counter]->served = true;
			interInsert.emplace_back(service[counter]);
			counter++;
			seat++;
			
			//point->p = service[counter]->p;
			//point->n = service[counter]->source;
			if (counter != service.size()) point->t = service[counter]->startTime;
			//point->intended = service[counter]->startTime;
		}
		//}

		//cout << "point time: " << point->t.getTime() << "\n";

		//no participant with near request time
		if (interInsert.size() == 0) {
			//counter--;
			return { r, counter };
		}
		//as there is already driver there
		seat = 1;

		//register source to route list
		for (int i = 0; i < interInsert.size(); i++) {
			//initialize reqNode
			point = new reqNode();
			//point->p = interInsert[i]->p;
			point->n = interInsert[i]->source;
			point->t = interInsert[i]->startTime;
			point->intended = interInsert[i]->startTime;
			r.emplace_back(point);
		}

		//register destination to route list
		for (int i = 0; i < interInsert.size(); i++) {
			//initialize reqNode
			point = new reqNode();
			//point->p = interInsert[i]->p;
			point->n = interInsert[i]->destination;
			point->t = interInsert[i]->endTime;
			point->intended = interInsert[i]->endTime;

			r.emplace_back(point);
		}

		//r = calcReqNodeDet(r);

		return { r, counter };
	}

	//calculate the detail of reqNode vector: waiting time, actual time of reaching destination, etc.
	vector<reqNode*> calcReqNodeDet(reqNode* rq, vector<reqNode*> r, int type, bool print, string filename) {
		//type = 0, driver have destination, type = 1, driver don't have destination

		//r is the vector to record all passenger on the car
		int s = r.size();
		Time t;
		int minuteDiff = 0;
		//distance between two point
		float dest = 0;
		float cost = 0;

		//to solve between the driver and the first passenger request, r[0]
		//no need to calculate cost here, as this is only to calculate the distance between the driver and the first passenger
		if (rq != NULL) {
			dest = rq->n->UCS(rq->n, r[0]->n);
			//calculate the time needed from a node to a node (t = s/v)
			//*60 to change to minute
			minuteDiff = (dest / speed) * 60;
			t = rq->t;
			t.addTime(minuteDiff);
			r[0]->t = t;
			//Time temp;
			//add earliness time
			early += r[0]->intended.calcDiff(r[0]->t);

			//if (rq->driver == NULL) cout << "\nhuhwhy1\n";
			if (rq->driver != NULL) r[0]->driver = rq->driver;
			//r[0]->passenger.emplace_back(r[0]->p);

			r[0]->type = 's';
			
			r[0]->pList++;
			//print out the point of the route

		}
		//cout << "The size of the interInsert list: " << s << "\n\n";
		for (int i = 1; i < s; i++) {
			//find the disance from a node to a node
			dest = r[i - 1]->n->UCS(r[i - 1]->n, r[i]->n);
			//calculate the time needed from a node to a node (t = s/v)
			//*60 to change to minute
			minuteDiff = (dest / speed) * 60;
			//cout << "minute differences: (" << dest << ", " << minuteDiff << ")\n";
			t = r[i - 1]->t;
			t.addTime(minuteDiff);
			r[i]->t = t;
			//to keep track of all current passengers on the car
			//r[i]->passenger.insert(r[i]->passenger.end(), r[i - 1]->passenger.begin(), r[i - 1]->passenger.end());
			//if (rq->driver == NULL) cout << "\nhuhwhy\n";
			if (rq->driver != NULL) r[i]->driver = rq->driver;
			//Time temp;
			//if is at source node, calculate if the car arrive too early
			if (i < (s / 2)) {
				//intended time - actual start time
				//add earliness
				//r[i]->p->addEarly(r[i]->intended.calcDiff(r[i]->t));
				early += r[i]->intended.calcDiff(r[i]->t);

				//update the available list of driver at the node
				//removeAvaiList(roundDown(t), r[i]->n, r[i]->p);
				//register passenger to the car
				//pList.emplace_back(r[i]->p);				
				r[i]->pList = r[i - 1]->pList + 1;
				//r[i]->passenger.emplace_back(r[i]->p);
				r[i]->type = 's';

				//calculate cost for each passenger
				cost = float(dest * 3.0);
				if (type == 1) cost *= 1.5;
				costs += round((cost / s) * 100) / 100;
				//this is to specify divide with how many people
				//int s = r[i]->passenger.size() - 1;
				

				//for (int j = 1; j < s; j++) {
					//if driver don't have destination
					//if (type == 1) cost *= 1.5;
					//costs = costs + cost;
					//round up to two decimal points
					//update the cost, use +=
					//pList[j]->cost += cost/s;
					//r[i]->passenger[j]->cost += round((cost / s) * 100) / 100;
					//cout << "\n" << "Passenger: " << r[i]->passenger[j]->id << ", cost: " << r[i]->passenger[j]->cost << "\n";
				//}
			}
			//if is at destination node, calculate if the car arrive too late
			else {
				//calculate and add lateness
				//r[i]->p->addLate(r[i]->t.calcDiff(r[i]->intended));
				late += r[i]->t.calcDiff(r[i]->intended);
				r[i]->pList = r[i - 1]->pList - 1;
				//update the available list of driver at the node
				//addAvaiList(roundDown(t), r[i]->n, r[i]->p);
				//r[i]->n->addAvaiList(roundDown(t), r[i]->p);
				r[i]->type = 'd';
				//calculate cost for each passenger
				cost = float(dest * 3.0);
				if (type == 1) cost *= 1.5;
				costs += round((cost / s) * 100) / 100;
				//cout << "\nCar " << ID << ", current cost is " << cost << "\n";
			}
			//cout << "\nCar " << ID << ", current cost is " << costs << "\n";

		}

		//total travelling time
		//int totalTime = r[r.size() - 1]->t.calcDiff(r[0]->t);

		return r;
	}

	void avoidLate() {
		int s = route.size() - 1;
		//int pointer = s;
		bool cont = true;
		int i = s;
		
		//for (int i = s; i >= 0; i--) {
		while (i >= 0) {
			int l = 0;
			if (route[i]->type == 'd') {
				l = route[i]->t.calcDiff(route[i]->intended);
			}
			
			if (l > 0) {
				for (int j = i; j >= 0; j--) {
					route[j]->t.subtractTime(l);
					if (route[j]->type == 's') {
						if (j == 0) break;
						else if (route[j - 1]->type == 'd') {
							if (!route[j]->t.compareTime(route[j - 1]->t)) break;
						}
					}
				}
			}
			i--;
		}
		
		
		
		late = 0;
		early = 0;
		//int ear = 0;
		//int la = 0;
		
		for (int i = 0; i < s + 1; i++) {
			if (route[i]->type == 'd') late += route[i]->t.calcDiff(route[i]->intended);
			else if (route[i]->type == 's') {
				int e = route[i]->intended.calcDiff(route[i]->t);
				early += e;
				if (e > 10) qualify = false;
				if (e > maxWait) maxWait = e;
			}
			else cout << "\nWe got some error here.\n";
		}

		//cout << "\nLateness: " << late << ", Calc: " << la;
		//cout << "\nEarliness: " << early << ", Calc: " << ear;

	}

	Time subTime(Time t, int m) {
		Time a;
		a.minute = t.minute;
		a.hour = t.hour;

		a.minute -= m;

		if (a.minute < 0) {
			a.hour--;
			a.minute = 60 + a.minute;
		}

		return a;
	}

	Time roundDown(Time t) {

		Time a;
		a.minute = t.minute;
		a.hour = t.hour;

		if (a.minute < 30) a.minute = 0;
		else a.minute = 30;

		return a;
	}

	//to print out the details of route list
	void printRoute(reqNode* r, string filename) {

		ofstream out(filename, ios::app);
		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}
		if (pList == 0) return;
		out << "{" << r->t.getTime() << "; Node " << r->n->getName() << "; ";
		//print out driver
		out << "D: " << "," << pList << " P";

		//print out the passenger
		int s = pList;

		if (s == 1) {
			out << " } --> ";

			out.close();
			return;
		}
		//cout << "size of pList: " << s << "\n\n";

		
		out << " } --> ";

		out.close();
	}

	//used when a passenger disembark from a car
	void removePassenger(participant* p) {
		
		pList--;

	}

	//used to add a participant on a car as passenger
	void addPassenger(participant* p) {
		pList++;
	}

	// to check if a participant is permitted to drive the car
	bool checkEligible(participant p) {
		int s = driverList.size();
		bool result = false;

		for (int i = 0; i < s; i++) {
			if (p.getID() == driverList[i]->getID()) {
				result = true;
				break;
			}
		}

		return result;
	}

	void assignDriver(participant* p) {
		driver = p;
	}

	void setOwner(participant* p) {
		owner = p;
	}

	void calcUnserved() {
		for (int i = 0; i < service.size(); i++) {
			if (!service[i]->served) unserved++;
		}
	}

};