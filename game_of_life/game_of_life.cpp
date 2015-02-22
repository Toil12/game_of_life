// Copyright (C) 2013 Matej Groma; licensed under GPL3 (see LICENSE)
#include "stdafx.h"
#include "game_of_life.h"
#include "graphics.h"
#include "fps.h"
#include "config.h"
#include "rules.h"
#include "map_manager.h"
goa* goa::instance=nullptr;

int main(int argc,char* argv[])
{
	if (argc != 2)
	{
		MessageBox(NULL,L"Drag and drop config file to the program to load it.",L"No config provided",MB_ICONASTERISK| MB_TASKMODAL | MB_SETFOREGROUND);
		return 0;
	}
	conf=new config(argv[1]);
	if (!goa::init())return 1;
	atexit(goa::deinit);
	while (true)
	{
		if (!goa::get()->run())break;
		goa::destroy_instance();
	}
	delete conf;
	return 0;
}

const int MOUSE_MODES=3;
goa::goa() : lives1(new bool[conf->x*conf->y]),lives2(new bool[conf->x*conf->y]),alt_buff(false),g(graphics::get_surface(),"font.bin"),mx(0),my(0),mouse_mode(0)
{
	for (size_t i=0;i!=conf->x*conf->y;i++)lives1[i]=((conf->def == 2) ? rand_bool(conf->init_rand_threshold) : (conf->def != 0));
	SDL_PumpEvents();
	SDL_GetMouseState(&mx,&my);
}
bool goa::init()
{
	if (!graphics::init())return false;
	srand(unsigned int (time(NULL)));
	return true;
}
void goa::deinit()
{
	graphics::deinit();
	goa::destroy_instance();
}

bool goa::run()
{
	bool paused=false;
	bool stopped=false;
	bool step_it=false;
	f.set_fps(conf->fps);
	bool stihal=true;
	ULONGLONG hsclock=0;
	render_fps_t render_fps={false,false,0,false};
	cursor_info cur={false,{0,0,0}};
	bool show_fps=
#ifndef _DEBUG
		false;
#else
		true;
#endif
	bool left_down=false;
	int oldmx=-1;
	int oldmy=-1;
	bool* lmap=nullptr;
	size_t lw=0;
	size_t lh=0;
	while (true)
	{
		f.start();
		if (((!paused) && (!stopped)) || step_it)
		{
			step_it=false;
			if (!step())stopped=true;
		}
		auto cur_map=(!alt_buff ? lives1 : lives2);
		SDL_Event ev;
		while (SDL_PollEvent(&ev))switch(ev.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_ESCAPE)return false;
			if (ev.key.keysym.sym == SDLK_r)return true;
			if (ev.key.keysym.sym == SDLK_SPACE)if (!stopped)paused=true;
			else step_it=true;
			if (ev.key.keysym.sym == SDLK_c)
			{
				for (size_t i=0;i!=conf->x*conf->y;i++)lives1[i]=lives2[i]=0;
			}
			if (ev.key.keysym.sym == SDLK_p)
			{
				stopped^=true;
			}
			if (ev.key.keysym.sym == SDLK_f)
			{
				render_fps.doit=(show_fps^=true);
				render_fps.zmaz_fps=!render_fps.doit;
			}
			if (stopped && ev.key.keysym.sym == SDLK_s)
			{
				auto seg=map_manager::get_smallest_seg(cur_map,conf->x,conf->y);
				if (seg == map_manager::MAP_SEGMENT_BLANK)continue;
				wstring op=map_manager::file_chooser(true,g.window);
				if (op.empty())continue;
				if (!map_manager::save(op,cur_map,conf->x,conf->y,seg))
					MessageBox(NULL,L"Cannot save file!",L"Error saving map",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
			}
			if (stopped && ev.key.keysym.sym == SDLK_l)
			{
				wstring ip=map_manager::file_chooser(false,g.window);
				if (ip.empty())continue;
				if (lmap)delete[] lmap;
				tie(lmap,lw,lh)=map_manager::load(ip,conf->x);
				if (!lmap)MessageBox(NULL,L"Cannot load file!",L"Error loading map",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				g.redraw_ls(lmap,lw,lh,mx-lw/2,my-lh/2);
				mouse_mode=2;
			}
			break;
		case SDL_KEYUP:
			if (!stopped && (ev.key.keysym.sym == SDLK_SPACE))paused=false;
			break;
		case SDL_MOUSEMOTION:
			mx=ev.motion.x/g.rect_w;
			my=ev.motion.y/g.rect_h;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (stopped && ev.button.button == SDL_BUTTON_LEFT)
			{
				oldmx=oldmy=-1;
				left_down=true;
			}
			if (stopped && ev.button.button == SDL_BUTTON_RIGHT)
			{
				left_down=false;
				mouse_mode++;
				mouse_mode%=MOUSE_MODES;
				if (mouse_mode == 2)g.redraw_ls(lmap,lw,lh,mx-lw/2,my-lh/2);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (ev.button.button == SDL_BUTTON_LEFT)left_down=false;
			break;
		default:
			break;
		}
#ifdef NEIGHBORHOOD_DEBUG
		SDL_FillRect(g.screen,NULL,SDL_MapRGB(g.screen->format,255,255,255));
		count_ar(lives1,my*conf->x + mx);
		SDL_Flip(g.screen);
		SDL_Delay(10);
		continue;
#endif
		if ((GetTickCount64()-hsclock) >= 500)
		{
			hsclock=GetTickCount64();
			render_fps.mfps=f.get_measured_fps();
			if (render_fps.mfps)
			{
				render_fps.doit=show_fps;
				render_fps.stihal=stihal;
			}
			stihal=true;
			f.enable_measure(true);
		}
		cur.enabled=(stopped && !step_it);
		cur.c=(!mouse_mode ? g.color_insert : g.color_erase);
		if ((mouse_mode != 2) && cur.enabled && left_down && ((oldmx != mx) || (oldmy != my)))
		{
			auto mbuf=(!alt_buff ? lives1 : lives2);
			size_t pos=my*conf->x + mx;
			mbuf[pos]=!mouse_mode;
		}
		if ((mouse_mode == 2) && cur.enabled && ((oldmx != mx) || (oldmy != my)))
		{
			g.redraw_ls(lmap,lw,lh,mx-lw/2,my-lh/2);
			if (left_down)map_manager::blit(lmap,make_pair(lw,lh),cur_map,make_pair(conf->x,conf->y),mx-lw/2,my-lh/2);
		}
		if ((oldmx != mx) || (oldmy != my))
		{
			oldmx=mx;
			oldmy=my;
		}
		g.draw_all(render_fps,cur);
		stihal=(f.stop() ? stihal : false);
	}
	return false;
}

inline size_t goa::count_ar(bool* buf,size_t i)
{
	size_t count=0;
	int x=i % conf->x;
	int y=i / conf->x;
	int cxfrom=x-conf->ar_depth;
	if (!conf->wrap_around && cxfrom < 0)cxfrom=0;
	int cxto=x+conf->ar_depth;
	if (!conf->wrap_around && cxto > conf->x-1)cxto=conf->x-1;
	int cyfrom=y-conf->ar_depth;
	if (!conf->wrap_around && cyfrom < 0)cyfrom=0;
	int cyto=y+conf->ar_depth;
	if (!conf->wrap_around && cyto > conf->y-1)cyto=conf->y-1;
	for (int cx=cxfrom;cx<=cxto;cx++)
	{
		for (int cy=cyfrom;cy<=cyto;cy++)
		{
			int wx,wy;
			tie(wx,wy)=wrap_surs(cx,cy);
			if ((wx == x) && (wy == y))continue;
			size_t pos=wy*conf->x + wx;
			if (buf[pos])count++;
#ifdef NEIGHBORHOOD_DEBUG
			SDL_Rect r={wx*goa::get()->g.rect_w,wy*goa::get()->g.rect_h,goa::get()->g.rect_w,goa::get()->g.rect_h};
			SDL_FillRect(goa::get()->g.screen,&r,SDL_MapRGB(goa::get()->g.screen->format,0,0,0));
#endif
		}
	}
	return count;
}
bool goa::step()
{
	auto oldbuf=(!alt_buff ? lives1 : lives2);
	auto newbuf=(alt_buff ? lives1 : lives2);
	bool changed=false;
	parallel_for(0,conf->x*conf->y,[oldbuf,newbuf,&changed](int i) {
		auto oldl=oldbuf+i;
		auto newl=newbuf+i;
		bool oldval=*oldl;
		*newl=rules_provider((conf->enable_counter ? count_ar(oldbuf,i) : 0),oldval);
		if (oldval != *newl)changed=true;
	});
	alt_buff^=true;
	return changed;
}