/**************************************************************************
 *
 * 	Timer.cpp by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#include <windows.h>

#include "Timer.h"

Timer::Timer():
	m_secsPerCount(0.0),
	m_deltaTime(0.0),
	m_baseTime(0),
	m_stopTime(0),
	m_pausedTime(0),
	m_currTime(0),
	m_prevTime(0),
	m_stopped(false){

	__int64 frequency;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
	m_scesPerCount = 1.0 / (double)frequency;
}

float Timer::TotalTime() const {

	if(m_stopped){
		return (float)(m_stopTime - m_pausedTime - m_baseTime)*m_secsPerCount;
	}
	else{
		return (float)(m_currTime - m_pausedTime - m_baseTime)*m_secsPerCount;
	}
}

float Timer::DeltaTime() const {
	
	return (float)m_deltaTime;
}


void Timer::Start(){

	if(m_stopped){
		__int64 startTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

		m_pausedTime += (startTime - m_stopTime);
		m_prevTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void Timer::Stop(){

	if(!m_stopped){
		__int64 currTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&cuurTime));
		m_stopTime = currTime;
		m_stopped = true;
	}
}

void Timer::Reset(){

	__int64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

	m_baseTime = currTime;
	m_currTime = currTime;
	m_stopTime = 0;
	m_stopped = false;
}

void Timer::Tick(){

	if(m_stopped){
		m_deltaTime = 0.0;
		return;
	}

	__int64 curr;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&curr));
	m_currTime = curr;

	m_deltaTime = (m_currTime - m_prevTime) * m_secsPerCount;
	m_prevTime = m_currTime;

	m_deltaTime = m_deltaTime < 0.0 ? 0.0 : m_deltaTime;
}
