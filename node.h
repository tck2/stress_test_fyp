#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <atomic>
#include "participant.h"
#include "car.h"
#include "Time.h"

using namespace std;

class node;

// custom comparator for priority queue
struct compareDist {
	bool operator()(pair<node*, float>& a, pair<node*, float>& b) {
		return a.second > b.second;
	}
};

class participant;
class car;
class Time;

class node {
public:
	//vector <driver> dr;
	//name of the point
	string name;
	//adjacency list, float is for distance
	vector <pair<node*, float>> nextList;
	//node* next;
	//the car that is currently at the node
	vector <car*> currCar;
	// - record the request of participant
	//record the participants currently on the node
	vector <participant*> currPar;
	//record the available driver list
	//unordered_map<Time, vector<participant*>> avaiList;
	//vector <pair<Time, vector<participant*>>> avaiList;

public:

	node() {
		name = "";
	}

	node(string s) {
		name = s;
	}

	string getName() {
		return name;
	}

	vector <pair<node*, float>> getAdjList() {
		return nextList;
	}

	//for adding other nodes into the adjacency list of a node
	void addAdjList(node* n, float dist) {
		nextList.emplace_back(n, dist);
	}

	//print out the adjacency list of the node
	void printNode() {
		ofstream out("node.txt", ios::app);

		if (!out.is_open()) {
			cout << "Failed to open file\n";
		}
		out << "Node " << name << ": ";

		for (int i = 0; i < nextList.size(); i++) {
			out << "{Node " << nextList[i].first->getName() << ", " << nextList[i].second << "km } --> ";
		}

		out << "\n\n";

		out.close();

	}

	//for finding the shortest distance to a node, using uniform cost search
	float UCS(node* source, node* dest) {
		priority_queue<pair<node*, float>, vector<pair<node*, float>>, compareDist> frontier;
		priority_queue<pair<node*, float>, vector<pair<node*, float>>, compareDist> frontier2;
		frontier.push({ source, (float)0.0 });
		frontier2.push({ source, (float)0.0 });
		vector<pair<node*, float>> selected;
		int counter = 0;
		//int slsize = selected.size();

		selected.push_back(frontier.top());
		frontier.pop();
		vector <pair<node*, float>> curr = selected[counter].first->getAdjList();
		int s = curr.size();
		for (int i = 0; i < s; i++) {
			if (curr[i].first != selected[counter].first) {
				frontier.push({ curr[i].first, curr[i].second + selected[counter].second });
			}
		}

		//counter++;
		//selected[counter].first != dest
		//printQ

		while (selected[counter].first != dest) {

			//if (selected[counter].first == dest) break;

			if (frontier.top().first == source) frontier.pop();
			selected.push_back(frontier.top());
			frontier.pop();
			counter++;
			//adjacency list of the top element
			vector <pair<node*, float>> curr = selected[counter].first->getAdjList();
			int s = curr.size();
			for (int i = 0; i < s; i++) {
				if (curr[i].first != selected[counter].first) {
					frontier.push({ curr[i].first, curr[i].second + selected[counter].second });
				}
			}
			//for debugging purposes
			//if (counter == selected.size() - 1) cout << "\nIm finding...\n";
			//counter++;
		}

		//cout << "We reached the end: " << selected[counter].first->name << ", distance is " << selected[counter].second << "\n\n";

		return selected[counter].second;
	}

	/*
	void genAvailist() {
		Time t1 = Time(0, 8);
		Time t_comp = Time(0, 19);

		while (t1.compareTime(t_comp)) {
			vector <participant*> curr;
			pair <Time, vector<participant*>> temp = { t1, curr };

			avaiList.emplace_back(temp);
			t1.addTime(30);
		}
	}*/

	Time roundDown(Time t) {

		Time a;
		a.minute = t.minute;
		a.hour = t.hour;

		if (a.minute < 30) a.minute = 0;
		else a.minute = 30;

		return a;
	}
};

#include "participant.h"