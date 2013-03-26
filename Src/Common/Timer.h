/**************************************************************************
 *
 * 	Timer.h by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__ 1

class Timer{

	public:
		Timer();

		float TotalTime() const;
		float DeltaTime() const;

		void Start();
		void Stop();
		void Reset();
		void Tick();

	private:
		double m_secsPerCount;
		double m_deltaTime;

		__int64 m_baseTime;
		__int64 m_stopTime;
		__int64 m_pausedTime;
		__int64 m_currTime;
		__int64 m_prevTime;

		bool m_stopped;
};

#endif  //__TIMER_H__
