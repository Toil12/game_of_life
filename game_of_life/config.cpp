// Copyright (C) 2013 Matej Groma; licensed under GPL3 (see LICENSE)
#include "stdafx.h"
#include "config.h"
#include "game_of_life.h"

const string dead_prefix="rules_for_dead_";
const string alive_prefix="rules_for_alive_";
config* conf;
config::config(string file) : width(800),height(600),x(0),y(0),def(2),rd(0),enable_counter(true),ar_depth(1),fr(0),fg(0),fb(0),br(255),bg(255),bb(255),rules_for_alive(nullptr),rules_for_dead(nullptr),fps(60),init_rand_threshold(FAP_MAX/4),rules_rand_threshold(FAP_MAX/5),wrap_around(false)
{
	fstream fin(file);
	if (!fin.is_open())
	{
		MessageBox(NULL,L"Cannot open config file!",L"Error loading config",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
		exit(1);
	}
	auto create_ruleset=[this]()
	{
		size_t siz=ar_depth*(4*ar_depth+4)+1;
		rules_for_alive=new int[siz];
		rules_for_dead=new int[siz];
		for (size_t i=0;i!=siz;i++)rules_for_dead[i]=rules_for_alive[i]=rd;
	};
	auto check_bounds=[this](size_t i)
	{
		if (i > size_t(ar_depth*(4*ar_depth+4)))throw exception();
		return i;
	};
	auto is_for_dead=[](string name)
	{
		return (name.size() >= dead_prefix.size()) && (name.substr(0,dead_prefix.size()) == dead_prefix);
	};
	auto is_for_alive=[](string name)
	{
		return (name.size() >= alive_prefix.size()) && (name.substr(0,alive_prefix.size()) == alive_prefix);
	};
	char buf[256];
	while (fin.getline(buf,sizeof(buf)))
	{
		string opt=buf;
		if (opt.empty() || (opt[0]=='#'))continue;
		auto parts=explode(':',opt);
		string name=parts[0];
		auto vals=explode(' ',parts[1]);
		if (name == "screen")
		{
			if (vals[0] == "full")
			{
				width=GetSystemMetrics(SM_CXSCREEN);
				height=GetSystemMetrics(SM_CYSCREEN);
				continue;
			}
			width=strto<size_t>(vals[0]);
			height=strto<size_t>(vals[1]);
		}
		else if (name == "blocks")
		{
			x=strto<size_t>(vals[0]);
			y=strto<size_t>(vals[1]);
		}
		else if (name == "map_default")
		{
			def=strto<int>(vals[0]);
		}
		else if (name == "enable_counter")
		{
			enable_counter=enabled_state(vals[0]);
		}
		else if (name == "wrap_around")
		{
			wrap_around=enabled_state(vals[0]);
		}
		/*
		else if (name == "radius")
		{
		ar_depth=strto<int>(vals[0]);
		if (rules)
		{
		MessageBox(NULL,L"Double radius specification!",L"Error in config",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
		exit(1);
		}
		create_ruleset();
		}
		*/
		else if (name == "ruleset_default")
		{
			if (rules_for_alive)
			{
				MessageBox(NULL,L"Option \"ruleset_default\" must precede rules!",L"Error in config",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				exit(1);
			}
			rd=strto<int>(vals[0]);
		}
		else if (name == "fg")
		{
			fr=strto<size_t>(vals[0]);
			fg=strto<size_t>(vals[1]);
			fb=strto<size_t>(vals[2]);
		}
		else if (name == "bg")
		{
			br=strto<size_t>(vals[0]);
			bg=strto<size_t>(vals[1]);
			bb=strto<size_t>(vals[2]);
		}
		else if ((is_for_dead(name) || is_for_alive(name)) ||
			((name == "die") || (name == "dies") || (name == "live") || (name == "lives") || (name == "rand") || (name == "rands") ||
			(name == "keep") || (name == "keeps") || (name == "same")))
		{
			if (!rules_for_alive)create_ruleset();
			int ttype=2;
			if (is_for_dead(name))
			{
				ttype=0;
				name=name.substr(dead_prefix.size(),name.size()-dead_prefix.size());
			}
			if (is_for_alive(name))
			{
				ttype=1;
				name=name.substr(alive_prefix.size(),name.size()-alive_prefix.size());
			}
			int rtype=3;
			if ((name == "die") || (name == "dies"))rtype=0;
			if ((name == "live") || (name == "lives"))rtype=1;
			if ((name == "rand") || (name == "rands"))rtype=2;
			try
			{
				for (auto v : vals)
				{
					if ((ttype == 0) || (ttype == 2))rules_for_dead[check_bounds(strto<size_t>(v))]=rtype;
					if ((ttype == 1) || (ttype == 2))rules_for_alive[check_bounds(strto<size_t>(v))]=rtype;
				}
			}
			catch (...)
			{
				MessageBox(NULL,L"Rule value out of bounds!",L"Error in config",MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				exit(1);
			}
		}
		else if (name == "fps")
		{
			fps=strto<int>(vals[0]);
		}
		else if (name == "init_rand_threshold")
		{
			init_rand_threshold=strto<int>(vals[0]);
		}
		else if (name == "rules_rand_threshold")
		{
			rules_rand_threshold=strto<int>(vals[0]);
		}
		else
		{
			wchar_t widen_opt[256];
			locale loc;
			use_facet<ctype<wchar_t>>(loc).widen(name.c_str(),name.c_str()+name.size()+1,widen_opt);
			MessageBox(NULL,(wstring(L"Unknown config option: \"") + widen_opt + L"\"!").c_str(),L"Unknown config option",MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND);
		}
	}
	if (!rules_for_alive)create_ruleset();
	if (!x)x=width/2;
	if (!y)y=height/2;
	fin.close();
}
config::~config()
{
	if (rules_for_alive)delete[] rules_for_alive;
	if (rules_for_dead)delete[] rules_for_dead;
}