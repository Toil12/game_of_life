// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once

inline pair<int,int> wrap_surs(int x,int y,Uint16 w,Uint16 h)
{
	while (x < 0)x=w+x;
	while (y < 0)y=h+y;
	x%=w;
	y%=h;
	return make_pair(x,y);
}

inline long long rand_n(long long from,long long to)
{
	if(from==to)return from;
	return int((rand()%(to-from+1)+from));
}
const unsigned long long FAP_MAX=10000;
inline bool rand_bool(unsigned long long fParam)
{
	unsigned long long f=FAP_MAX-fParam;
	unsigned long long r=rand_n(0,FAP_MAX-1);
	if (r>=f)return true;
	return false;
}

inline vector<string> explode(char separator,const string& str,size_t limit=0)
{
	vector<string> ret;
	string t;
	for (auto it=str.begin();it!=str.end();it++)
	{
		char x=*it;
		if (limit && (ret.size()+1 >= limit))t+=x;
		else
		{
			if (x!=separator)t+=x;
			else if (t!="")
			{
				ret.push_back(t);
				t=string();
			}
		}
	}
	if (t!="")ret.push_back(t);
	return ret;
}
template<typename T>
inline T strto(const string& str)
{
	stringstream ss(str);
	T ret;
	ss>>ret;
	return ret;
}

inline bool enabled_state(const string& param)
{
	return param == "1" || param == "en" || param == "enabled" || param == "true" || param == "yes" || param == "on";
}