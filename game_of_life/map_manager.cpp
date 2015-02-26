// Copyright (C) 2013 Matej Groma; licensed under GPL3 (see LICENSE)
#include "stdafx.h"
#include "map_manager.h"
#include "misc.h"

const map_segment map_manager::MAP_SEGMENT_BLANK={0,0,0,0};
const char map_manager::char_dead=' ';
const char map_manager::char_live='X';

map_segment map_manager::get_smallest_seg(bool* map,size_t w,size_t h)
{
	map_segment ret={w,h,0,0};
	bool any=false;
	for (int x=0;x!=w;x++)for (int y=0;y!=h;y++)if (map[y*w+x])
	{
		any=true;
		ret.xfrom=min(ret.xfrom,x);
		ret.yfrom=min(ret.yfrom,y);
		ret.xto=max(ret.xto,x+1);
		ret.yto=max(ret.yto,y+1);
	}
	if (any)return ret;
	return MAP_SEGMENT_BLANK;
}

wstring map_manager::file_chooser(bool savedialog,HWND window)
{
	wchar_t buf[MAX_PATH]={};
	DWORD flags=OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST;
	if (!savedialog)flags|=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	else flags|=OFN_OVERWRITEPROMPT;
	wchar_t cdb[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,cdb);
	OPENFILENAME ofn={sizeof(OPENFILENAME),window,NULL,L"GOA saves\0*.gos\0All files\0*.*\0\0",NULL,NULL,0,buf,MAX_PATH,
		NULL,0,cdb,NULL,flags,0,0,L"txt",NULL,NULL,NULL,NULL,0,0};
	BOOL dret=(!savedialog ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn));
	if (window)SetForegroundWindow(window);
	if (dret)return ofn.lpstrFile;
	return wstring();
}

bool map_manager::save(wstring path,const bool* map,size_t w,size_t h,map_segment seg)
{
	ofstream fout(path);
	if (!fout.is_open())return false;
	for (int y=seg.yfrom;y!=seg.yto;y++)
	{
		for (int x=seg.xfrom;x!=seg.xto;x++)
		{
			size_t pos=y*w+x;
			fout<<(map[pos] ? char_live : char_dead);
		}
		fout<<endl;
	}
	fout.close();
	return true;
}

tuple<bool*,size_t,size_t> map_manager::load(wstring path,size_t max_width)
{
	ifstream fin(path);
	if (!fin.is_open())return make_tuple(nullptr,0,0);
	char* buf=new char[max_width+1];
	vector<string> vmap;
	while (fin.getline(buf,max_width+1))vmap.push_back(buf);
	delete buf;
	fin.close();
	if (vmap.empty())return make_tuple(nullptr,0,0);
	size_t width=vmap[0].size();
	bool* map=new bool[width*vmap.size()];
	for (size_t y=0;y!=vmap.size();y++)
	{
		string row=vmap[y];
		if (width != row.size())
		{
			delete map;
			return make_tuple(nullptr,0,0);
		}
		for (size_t x=0;x!=width;x++)
		{
			bool lives=(row[x]!=char_dead);
			map[y*width+x]=lives;
		}
	}
	return make_tuple(map,width,vmap.size());
}

void map_manager::blit(const bool* src,seg_size srcsiz,bool* dst,seg_size dstsiz,int ax,int ay)
{
	for (Uint16 x=0;x!=srcsiz.first;x++)for (Uint16 y=0;y!=srcsiz.second;y++)
	{
		Uint16 dstx,dsty;
		tie(dstx,dsty)=wrap_surs(x+ax,y+ay,dstsiz.first,dstsiz.second);
		if (src[y*srcsiz.first+x])dst[dsty*dstsiz.first+dstx]=true;
	}
}