// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once

struct map_segment
{
	Uint16 xfrom,yfrom;
	Uint16 xto,yto;
	bool operator==(const map_segment& b) const
	{
		if (xfrom != b.xfrom)return false;
		if (yfrom != b.yfrom)return false;
		if (xto != b.xto)return false;
		if (yto != b.yto)return false;
		return true;
	}
};
typedef pair<size_t,size_t> seg_size;
class map_manager
{
public:
	static const map_segment MAP_SEGMENT_BLANK;
	static const char char_dead,char_live;
	static map_segment get_smallest_seg(bool* map,size_t w,size_t h);
	static wstring file_chooser(bool savedialog,HWND window);
	static bool save(wstring path,const bool* map,size_t w,size_t h,map_segment seg);
	static tuple<bool*,size_t,size_t> load(wstring path,size_t max_width);
	static void blit(const bool* src,seg_size srcsiz,bool* dst,seg_size dstsiz,int ax,int ay);
};