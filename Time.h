#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>

using namespace std;

class Time {
	//self define a time class to use as time during optimization
public:
	int minute;
	int hour;

	Time() {
		hour = 0;
		minute = 0;
	}

	Time(int m, int h) {
		hour = h;
		minute = m;
		if (minute >= 60) {
			minute -= 60;
			hour++;
		}
	}

	string getTime() {
		return to_string(hour) + ":" + to_string(minute);
	}

	bool calcTimeDiff(int min, Time t) {
		int min_a = this->hour * 60 + this->minute;
		int min_b = t.hour * 60 + t.minute;

		if (min_b - min_a <= min) return true;

		return false;
	}

	int calcDiff(Time t) {
		int t1 = hour * 60 + minute;
		int t2 = t.hour * 60 + t.minute;

		int tt = t1 - t2;

		if (tt > 0) return tt;

		return 0;

	}

	void subtractTime(int m) {
		int tq = hour * 60 + minute;
		tq -= m;
		hour = tq / 60;
		minute = tq % 60;
	}

	void addTime(int min) {
		minute += min;

		if (minute >= 60) {
			minute -= 60;
			hour++;
		}
	}

	bool compareTime(Time t) {
		int t1 = hour * 60 + minute;
		int t2 = t.hour * 60 + t.minute;

		return t1 < t2;
	}

	bool compSameTime(Time t) {
		int t1 = hour * 60 + minute;
		int t2 = t.hour * 60 + t.minute;

		return t1 == t2;
	}
	
};
