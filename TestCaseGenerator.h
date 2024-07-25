#pragma once

#include "car.h"
#include "node.h"
#include "participant.h"
#include "Solution.h"
#include "Time.h"
#include "Request.h"
#include <iostream>
#include <random>
#include <vector>
#include <atomic>

using namespace std;

class car;
class node;
class participant;
class Solution;
class Time;
class request;

class testCase {

	string id;

public:

	testCase() {
		id = "000";
	}

	testCase(string s) {
		id = s;
	}

	//generate random number of nodes


	//nList is a public resource, else everytime create a new solution need to duplicate
	vector<request*> generateP(vector<node*> nList, vector<request*> req) {
		random_device rd;
		mt19937 generator(rd());
		//for generating number of participants
		uniform_int_distribution<int> distro(40, 50);
		//distribution for nodes
		uniform_int_distribution<int> nodeDis(0, nList.size() - 1);
		//for the rate of request generation in specific time frame
		//uniform_int_distribution<int> distroT(1, 34);
		uniform_real_distribution<float> distroT(0, 1);
		//dispute between same rate
		uniform_int_distribution<int> distroFF(1, 4);

		// determine whether can a participant drive
		//uniform_int_distribution<int> distroCD(0, 1);
		//generate participant request

		//for random number of request
		//uniform_int_distribution<int> distroReqnum(3, 3);

		//for random max number of requests
		uniform_int_distribution<int> distroMaxReq(300, 350);

		int count1 = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		int count5 = 0;

		//vector <participant*> pList;

		//int ran_num = distro(generator);
		int max_req = distroMaxReq(generator);
		int max = 0;
		int timeCounter = 1;

		cout << "Total number of requests: " << max_req << "\n";
		//cout << "Total number of participants: " << ran_num << "\n";

		//for (int i = 0; i < ran_num; i++) {
			//whether can drive or not
			//int CD = distroCD(generator);

			//add new participant into participant list
		//	pList.emplace_back(new participant(to_string(i), 0));
		//}

		//cout << "total of request: " << ran_num << "\n";
		//int s = pList.size();

		for (int i = 0; i < max_req; i++) {

			int start, end;

			float cap = distroT(generator);
			if (cap < 0.51) {
				int disp50 = distroFF(generator);
				if (disp50 % 2 == 0 && count1 != 90) {
					timeCounter = 1;
					count1++;
				}
				else if (disp50 % 2 != 0 && count5 != 90) {
					timeCounter = 5;
					count5++;
				}
			}
			else if (cap > 0.5 && cap < 0.86) {
				int disp50 = distroFF(generator);
				if (disp50 % 2 == 0 && count2 != 90) {
					timeCounter = 2;
					count2++;
				}
				else if (disp50 % 2 != 0 && count4 != 90) {
					timeCounter = 4;
					count4++;
				}
			}
			else {
				if (count3 != 90) {
					timeCounter = 3;
					count3++;
				}
			}



			//distribute time into 5 groups
			if (timeCounter == 1) {
				start = 8;
				end = 9;
			}
			else if (timeCounter == 2) {
				start = 10;
				end = 11;
			}
			else if (timeCounter == 3) {
				start = 12;
				end = 13;
			}
			else if (timeCounter == 4) {
				start = 14;
				end = 15;
			}
			else if (timeCounter == 5) {
				start = 16;
				end = 18;
			}

			//generate random time
			uniform_int_distribution<int> distroReqHour(start, end);
			int maxMin = 0;
			int hour = distroReqHour(generator);
			if (hour != 18) maxMin = 59;
			else maxMin = 10;
			uniform_int_distribution<int> distroReqMnt(0, maxMin);

			//if (hour == 18) minute = 0;
			//else 
			int minute = distroReqMnt(generator);

			//request start time cannot be later than 6 pm
			//if (hour == 18) minute = 0;
			//start time
			Time t = Time(minute, hour);

			bool same = false;

			/*
			if (j != 0) {
				do {
					same = false;
					for (int k = 0; k < j; k++) {
						//cout << "haha\n\n";

						//also restrict that next request cannot be earlier than preferred end time
						if (t.compSameTime(pList[i]->req[k]->startTime)) {
							same = true;
							minute = distroReqMnt(generator);
							hour = distroReqHour(generator);
							t = Time(minute, hour);
						}
					}
				} while (same);
			}
			*/
			


			//end time
			Time tt = Time(minute + 25 , hour);
			//cout << i << "\n";
			//cout << "size is " << pList.size() << "\n";

			//source node
			//cout << i << "\n";
			int ran_s = nodeDis(generator);

			//destination
			int ran_d = nodeDis(generator);

			//so destination != source
			while (ran_s == ran_d) {
				ran_d = nodeDis(generator);
			}

			request* r = new request();
			//if not the first request to be generated
			r = new request(i+1, nList[ran_s], nList[ran_d], t, tt, NULL);
			req.emplace_back(r);

		}

		cout << "\n The number of requests in the first time frame is " << count1 << "\n";
		cout << " The number of requests in the second time frame is " << count2 << "\n";
		cout << " The number of requests in the third time frame is " << count3 << "\n";
		cout << " The number of requests in the fourth time frame is " << count4 << "\n";
		cout << " The number of requests in the fifth time frame is " << count5 << "\n\n";

		return req;
	}


	//generate cars
	vector<car*> genC() {
		random_device rd;
		mt19937 generator(rd());
		//a little bit less than the lowest passenger number
		//to test if the initialization still works even if there is not enough car

		//'distro' random geenrator is used to generate a random number of count of car
		uniform_int_distribution<int> distro(10, 10);
		uniform_int_distribution<int> distroSeat(1, 10);
		//uniform_int_distribution<int> distroP(0, pList.size() - 1);
		//uniform_int_distribution<int> distroCarDriver(1, 10);

		int ran_num = distro(generator);
		vector<car*> cList;
		cout << "Total number of cars: " << ran_num << "\n\n";
		for (int i = 0; i < ran_num; i++) {
			//generate random participant
			//int ranP = distroP(generator);

			int seat = 0;
			int seatProb = distroSeat(generator);

			if (seatProb > 5) seat = 6;
			else seat = 4;

			cList.emplace_back(new car(NULL, to_string(i), seat));



		}
		return cList;
	}

	//generate node
	vector<node*> genNode(vector<node*> nList) {
		random_device rd;
		mt19937 gen(rd());
		//create random number for node
		uniform_int_distribution<int> distro_node(30, 30);
		//adjacency list size
		uniform_int_distribution<int> distro_adj(4, 8);
		//distance with each node
		uniform_real_distribution<float> dist_prob(0, 10);

		int ran_n = distro_node(gen);
		//add a random number generator for random adjacent node
		uniform_int_distribution<int> dist_n(0, ran_n - 1);
		float dist = 0;
		float proba = 0;
		//generate random node
		for (int i = 0; i < ran_n; i++) {
			nList.emplace_back(new node(to_string(i)));
			// generate structure for avaiList
		}
		//generate random adjacency list for a node
		int s = nList.size();
		for (int i = 0; i < s; i++) {
			int ran_adj = distro_adj(gen);
			//add in adjacency nodes
			//cout << nList[i]->avaiList.size() << "\n";
			for (int j = 0; j < ran_adj; j++) {
				int ran_node = dist_n(gen);
				//avoid that the adjacency list of a node includes itself
				while (ran_node == i || ran_node >= s) {
					ran_node = dist_n(gen);
				}
				//generate random distance between node
				proba = dist_prob(gen);
				//control the distribution of the random generated distances
				//proba > 8 && proba < 9.6
				if (proba > 8.5) {
					uniform_real_distribution<float> distro_dist(2, 3);
					dist = distro_dist(gen);
				}
				else {
					uniform_real_distribution<float> distro_dist(0.35, 2);
					dist = distro_dist(gen);
				}
				//dist = distro_dist(gen);
				nList[i]->addAdjList(nList[ran_node], dist);
			}
		}
		return nList;
	}

	Time roundDown(Time t) {

		Time a;
		a.minute = t.minute;
		a.hour = t.hour;

		if (a.minute < 30) a.minute = 0;
		else a.minute = 30;

		return a;
	}


};