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
		running = true;
	};

	void Stop()
	{
		running = false;
	};

	inline float Read() {
		if (running)
			time = (SDL_GetTicks() - ticksStart + timeSkipped);
		return time;
	}

	inline float ReadSeconds() {
		if (running)
			time = (SDL_GetTicks() - ticksStart + timeSkipped) / 1000.0f;
		return time;
	}

	inline void Pause() {
		timeSkipped += (SDL_GetTicks() - ticksStart);
		running = false;
	}

	inline void Reset() {
		ticksStart = SDL_GetTicks();
		timeSkipped = 0;
	}
public:
	bool running = false;
private:
	float time = 0.0f;
	__int32 ticksStart = 0;
	__int32 timeSkipped = 0;
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
	__int64 ticksStart = 0;
	__int64 ticksStop = 0;
};

#endif __TIMER_H_

