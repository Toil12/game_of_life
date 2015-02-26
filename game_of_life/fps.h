// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once

class fps
{
private:
	bool running;
	ULONGLONG clock,diff;
	unsigned long long desired_fps;
	bool measure_fps;
	long double measured_fps;
	unsigned long long measure_count;
public:
	fps() : running(false),measure_fps(false),clock(0),diff(0),desired_fps(0),measured_fps(0),measure_count(0) {}
	inline void set_fps(int f) {desired_fps=f;}
	void start();
	bool stop(bool do_delay=true);
	void enable_measure(bool enable);
	inline long double get_measured_fps() {return measured_fps;}
	inline ULONGLONG get_diff() {return diff;}
};