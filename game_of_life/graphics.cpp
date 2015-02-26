// Copyright (C) 2013 Matej Groma; licensed under GPL3 (see LICENSE)
#include "stdafx.h"
#include "game_of_life.h"
#include "graphics.h"
#include "config.h"

SDL_Surface* graphics::win=nullptr;
bool graphics::init()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);
	SDL_WM_SetCaption("Game of life","GOA");
	TTF_Init();
	int sw=GetSystemMetrics(SM_CXSCREEN);
	int sh=GetSystemMetrics(SM_CYSCREEN);
	Uint32 flags=0;
	if ((conf->width == sw) && (conf->height == sh))flags|=SDL_FULLSCREEN;
	SDL_ShowCursor(SDL_DISABLE);
	win=SDL_SetVideoMode(conf->width,conf->height,0,flags);
	return true;
}
SDL_Surface* graphics::get_surface()
{
	return SDL_CreateRGBSurface(0,conf->width,conf->height,win->format->BitsPerPixel,0,0,0,0);
}
void graphics::deinit()
{
	//TTF_Quit();
	SDL_Quit();
}

void graphics::draw()
{
	auto buf=(!goa::get()->alt_buff ? goa::get()->lives1 : goa::get()->lives2);
	make(buf,screen,conf->x,conf->y);
}
void graphics::make(bool* map,SDL_Surface* dst,int w,int h,int adv_x,int adv_y)
{
	if ((rect_w == 1) && (rect_h == 1))
	{
		if (SDL_MUSTLOCK(dst))SDL_LockSurface(dst);
		Uint32* pixels=static_cast<Uint32*>(dst->pixels);
		for (size_t rx=0;rx!=w;rx++)for (size_t ry=0;ry!=h;ry++)
		{
			size_t x,y;
			tie(x,y)=wrap_surs(rx+adv_x,ry+adv_y);
			auto pixel=pixels+(y*dst->w + x);
			if (map[ry*w + rx])*pixel=SDL_MapRGB(dst->format,conf->fr,conf->fg,conf->fb);
			else *pixel=SDL_MapRGB(dst->format,conf->br,conf->bg,conf->bb);
		}
		if (SDL_MUSTLOCK(dst))SDL_UnlockSurface(dst);
	}
	else
	{
		//SDL_FillRect(dst,NULL,SDL_MapRGB(dst->format,conf->br,conf->bg,conf->bb));
		for (size_t rx=0;rx!=w;rx++)for (size_t ry=0;ry!=h;ry++)
		{
			size_t x,y;
			tie(x,y)=wrap_surs(rx+adv_x,ry+adv_y);
			Uint32 color=SDL_MapRGB(dst->format,conf->br,conf->bg,conf->bb);
			if (map[ry*w + rx])color=SDL_MapRGB(dst->format,conf->fr,conf->fg,conf->fb);
			SDL_Rect r={x*rect_w,y*rect_h,rect_w,rect_h};
			SDL_FillRect(dst,&r,color);
		}
	}
}
void graphics::render_fps(long double fps,bool stihal)
{
	SDL_Color c={255,0,0};
	if (stihal)c.g=255;
	stringstream ss;
	ss<<"FPS: ";
	ss<<setprecision(2)<<fixed<<setfill(' ')<<setw(6);
	if (fps)ss<<fps;
	else ss<<"???";
	string str(ss.str());
	SDL_Surface* st=TTF_RenderText_Blended(font,str.c_str(),c);
	SDL_Surface* sp=SDL_CreateRGBASurface(0,st->w,st->h);
	SDL_FillRect(sp,NULL,SDL_MapRGBA(sp->format,0,0,0,128+64));
	SDL_Rect r={st->h/2,st->h/2,st->w,st->h};
	SDL_BlitSurface(sp,NULL,win,&r);
	SDL_BlitSurface(st,NULL,win,&r);
	SDL_FreeSurface(st);
}
void graphics::redraw_ls(bool* map,size_t w,size_t h,size_t x,size_t y)
{
	SDL_FillRect(ls,NULL,ls_ck);
	make(map,ls,w,h,x,y);
}

void graphics::draw_all(render_fps_t fi,cursor_info c)
{
	draw();
	SDL_BlitSurface(screen,NULL,win,NULL);
	if (c.enabled && (goa::get()->mouse_mode != 2))
	{
		auto crs=goa::get()->get_cursor_rects(goa::get()->mx,goa::get()->my);
		SDL_FillRect(win,&crs.first,SDL_MapRGB(win->format,c.c.r,c.c.g,c.c.b));
		SDL_FillRect(win,&crs.second,SDL_MapRGB(win->format,c.c.r,c.c.g,c.c.b));
		SDL_Rect rs={goa::get()->mx*rect_w,goa::get()->my*rect_h,rect_w,rect_h};
		SDL_BlitSurface(screen,&rs,win,&rs);
		/*
		SDL_Color cs=(!goa::get()->mouse_erase ? make_color(conf->fr,conf->fg,conf->fb) : make_color(conf->br,conf->bg,conf->bb));
		SDL_FillRect(win,&rs,SDL_MapRGB(win->format,cs.r,cs.g,cs.b));
		*/
	}
	if (c.enabled && (goa::get()->mouse_mode == 2))SDL_BlitSurface(ls,NULL,win,NULL);
	if (fi.doit)render_fps(fi.mfps,fi.stihal);
	SDL_Flip(win);
}

SDL_Surface* SDL_CreateRGBASurface(Uint32 flags,int width,int height)
{
	SDL_Surface* temp=SDL_CreateRGBSurface(flags,width,height,32,0,0,0,0);
	SDL_Surface* ret=SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
	return ret;
}