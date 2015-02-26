// Copyright (C) 2013 Matej Groma; licensed under GPL3 (see LICENSE)
#include "stdafx.h"
#include "fps.h"

void fps::start()
{
	if (running)throw exception("FPS clock already running");
	clock=GetTickCount64();
	running=true;
}
bool fps::stop(bool do_delay)
{
	if (!running)throw exception("FPS clock not running");
	running=false;
	diff=GetTickCount64()-clock;
	do_delay=do_delay && (desired_fps != 0);
	bool stiham=true;
	ULONGLONG desired_diff=(desired_fps ? 1000/desired_fps : 0);
	if (do_delay)
	{
		if (desired_diff > diff)SDL_Delay(Uint32(desired_diff-diff));
		else if (desired_diff != diff)stiham=false;
	}
	if (measure_fps)
	{
		ULONGLONG real_diff=max(diff,desired_diff);
		real_diff=max(real_diff,1);// nemame taku presnost aby tam nebola 0
		long double ac_fps=1000/long double(real_diff);
		measured_fps=(measure_count*measured_fps+ac_fps)/(measure_count+1);
		measure_count++;
	}
	return stiham;
}

void fps::enable_measure(bool enable)
{
	if (!measure_fps && !enable)throw exception("FPS measure already disabled");
	if (enable)
	{
		measured_fps=0;
		measure_count=0;
	}
	measure_fps=enable;
}