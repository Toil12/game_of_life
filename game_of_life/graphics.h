// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once
#include "game_of_life.h"
#include "config.h"
#include "resource.h"

static const int FONT_SIZE=20;
static const Uint16 CURSOR_LENGTH=3;
struct render_fps_t
{
	bool doit;
	bool stihal;
	long double mfps;
	bool zmaz_fps;
};
struct cursor_info
{
	bool enabled;
	SDL_Color c;
};
inline int avg(int a,int b) {return (a+b)/2;}
inline SDL_Color get_creation_color()
{
	SDL_Color ret={avg(conf->fr,conf->br),avg(conf->fg,conf->bg),avg(conf->fb,conf->bb)};
	return ret;
}
inline SDL_Color rev_color(SDL_Color c)
{
	SDL_Color ret={255-c.r,255-c.g,255-c.b};
	return ret;
}
inline SDL_Color make_color(Uint8 r,Uint8 g,Uint8 b)
{
	SDL_Color ret={r,g,b};
	return ret;
}

// universal resource loader by Matej
template<typename T>
struct l_res
{
	size_t size;
	T* ptr;
};
template<typename T>
l_res<T> resource_loader(PTCHAR res_name,PTCHAR res_type)
{
	l_res<T> ret;
	ret.size=0;
	ret.ptr=nullptr;
	HRSRC hRes=FindResource(NULL,res_name,res_type);
	if(!hRes)return ret;
	ret.size=SizeofResource(NULL,hRes);
	HGLOBAL hBytes=LoadResource(NULL,hRes);
	if(!hBytes)return ret;
	void* v_ptr=LockResource(hBytes);
	if(!v_ptr)return ret;
	ret.ptr=(T*)v_ptr;
	return ret;
}

class graphics
{
	friend class goa;// kvoli flipovaniu surfacu
private:
	SDL_Surface* screen;
	static SDL_Surface* win;
	TTF_Font* font;
	const size_t rect_w,rect_h;
	const SDL_Color color_insert,color_erase;
	SDL_Surface* ls;
	Uint32 ls_ck;
	HWND window;
public:
	graphics(SDL_Surface* surf,string font_file) : screen(surf),font(nullptr),rect_w(conf->width/conf->x),rect_h(conf->height/conf->y),color_insert(get_creation_color()),color_erase(rev_color(color_insert)),ls(SDL_CreateRGBSurface(0,win->w,win->h,32,0,0,0,0)),ls_ck(SDL_MapRGB(ls->format,conf->br,conf->bg,conf->bb)),window(NULL)
	{
		do
		{
			auto font_res=resource_loader<BYTE>(MAKEINTRESOURCE(IDR_GOA_RES1),L"GOA_RES");
			if (!font_res.ptr)break;
			auto rwops=SDL_RWFromConstMem(font_res.ptr,font_res.size);
			if (!rwops)break;
			font=TTF_OpenFontRW(rwops,1,FONT_SIZE);
		} while (false);
		if (!font)
		{
			MessageBox(NULL,L"Could not load font.",L"File not found",MB_ICONERROR| MB_TASKMODAL | MB_SETFOREGROUND);
			exit(1);
		}
		SDL_SetColorKey(ls,SDL_SRCCOLORKEY,ls_ck);
		SDL_SysWMinfo wminfo;
		SDL_VERSION(&wminfo.version);
		if (SDL_GetWMInfo(&wminfo) != 1)wminfo.window=0;
		window=wminfo.window;
	}
	~graphics() {TTF_CloseFont(font);SDL_FreeSurface(screen);}
	static bool init();
	static SDL_Surface* get_surface();
	static void deinit();
	void draw();
	void make(bool* map,SDL_Surface* dst,int w,int h,int adv_x=0,int adv_y=0);
	void draw_all(render_fps_t fi,cursor_info c);
	void render_fps(long double fps,bool stihal);
	void redraw_ls(bool* map,size_t w,size_t h,size_t x,size_t y);
};

SDL_Surface* SDL_CreateRGBASurface(Uint32 flags,int width,int height);