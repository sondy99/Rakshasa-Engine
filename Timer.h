#ifndef __TIMER_H_
#define __TIMER_H_

#include "SDL.h"

class Timer
{
public:
	Timer()
	{
	};
	~Timer()
	{
	};

	void Start()
	{
		ticksStart = SDL_GetTicks();
	};

	float Stop()
	{
		ticksStop = SDL_GetTicks();

		return (ticksStop - ticksStart) / 1000.0f;
	};
private:
	__int32 ticksStart;
	__int32 ticksStop;
};

class TimerMicro
{
public:
	TimerMicro() {};

	~TimerMicro() {};

	void Start()
	{
		ticksStart = SDL_GetPerformanceCounter();
	};

	float Stop()
	{
		ticksStop = SDL_GetPerformanceCounter();

		return (ticksStop - ticksStart);
	};
private:
	__int64 ticksStart;
	__int64 ticksStop;
};

#endif __TIMER_H_

