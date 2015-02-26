// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once
#include "graphics.h"
#include "fps.h"
#include "misc.h"

class goa
{
private:
	goa();
	~goa()
	{
		delete[] lives1;
		delete[] lives2;
	}
	goa(const goa&);
	goa& operator=(const goa&);
	static goa* instance;
	graphics g;
	fps f;
public:
	static bool init();
	static void deinit();
	inline static goa* get()
	{
		if (!instance)instance=new goa;
		return instance;
	}
	inline static void destroy_instance()
	{
		if (instance)delete instance;
		instance=nullptr;
	}
	bool run();
	bool step();
	static inline size_t count_ar(bool* buf,size_t i);
	bool* lives1;
	bool* lives2;
	bool alt_buff;
	int mx,my;
	inline pair<SDL_Rect,SDL_Rect> get_cursor_rects(int x,int y)
	{
		int xfrom=max((x-CURSOR_LENGTH)*goa::get()->g.rect_w,0);
		int xto=min((x+CURSOR_LENGTH+1)*goa::get()->g.rect_w,conf->width);
		SDL_Rect rx={xfrom,y*goa::get()->g.rect_h,xto-xfrom,goa::get()->g.rect_h};
		int yfrom=max((y-CURSOR_LENGTH)*goa::get()->g.rect_h,0);
		int yto=min((y+CURSOR_LENGTH+1)*goa::get()->g.rect_h,conf->height);
		SDL_Rect ry={x*goa::get()->g.rect_w,yfrom,goa::get()->g.rect_w,yto-yfrom};
		return make_pair(rx,ry);
	}
	int mouse_mode;
};

inline pair<int,int> wrap_surs(int x,int y)
{
	return wrap_surs(x,y,conf->x,conf->y);
}