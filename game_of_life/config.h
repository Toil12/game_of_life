// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once

class config
{
private:
	config(const config&);
	config& operator=(const config&);
public:
	config(string file);
	~config();
	Uint16 width,height;
	Uint16 x,y;
	int def;
	int rd;
	bool enable_counter;
	int ar_depth;
	Uint8 fr,fg,fb;
	Uint8 br,bg,bb;
	int* rules_for_alive;
	int* rules_for_dead;
	int fps;
	int init_rand_threshold,rules_rand_threshold;
	bool wrap_around;
};
extern config* conf;