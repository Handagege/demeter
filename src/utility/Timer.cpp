#include "Timer.h"
using namespace demeter_uti;
#include <stdio.h>

Timer::Timer()
{
	Clear(); 
}


Timer::Timer(const Timer& timer)
{
	m_laststart = timer.m_laststart;
	m_bPingPang = timer.m_bPingPang;
	m_vtrSeg = timer.m_vtrSeg;
}


Timer::~Timer()
{
}


Timer& Timer::operator = (const Timer& timer)
{
	m_laststart = timer.m_laststart;
	m_bPingPang = timer.m_bPingPang;
	m_vtrSeg = timer.m_vtrSeg;
	return *this; 
}


void Timer::Clear()
{
	m_bPingPang = false;
	m_vtrSeg.clear(); 
}


void Timer::Start()
{
	gettimeofday(&m_laststart, NULL);
	m_bPingPang = true;
}


void Timer::Stop()
{
	if(m_bPingPang)
	{
		timeval end;
		gettimeofday(&end, NULL);
		pair<uint32_t,uint32_t> cost; 
		if(end.tv_usec >= m_laststart.tv_usec)
		{
			cost.first = end.tv_sec - m_laststart.tv_sec; 
			cost.second = end.tv_usec - m_laststart.tv_usec; 
		}
		else
		{
			cost.first = end.tv_sec - m_laststart.tv_sec - 1; 
			cost.second = end.tv_usec + 1000000 - m_laststart.tv_usec; 
		}
		m_vtrSeg.push_back(cost); 
		m_bPingPang = false;
	}
}


uint32_t Timer::SegCnt()
{
	return m_vtrSeg.size(); 
}


int32_t Timer::GetLast_asUSec()
{
	if(m_vtrSeg.empty())
		return -1; 
	return (int32_t)(m_vtrSeg.back().first * 1000 * 1000 + m_vtrSeg.back().second);
}

float Timer::GetLast_asMSec()
{
	return (float)(m_vtrSeg.back().first) * 1000.0 + (float)m_vtrSeg.back().second / 1000.0;
}


float Timer::GetLast_asSec()
{
	return (float)(m_vtrSeg.back().first) + (float)m_vtrSeg.back().second / 1000.0 / 1000.0;
}


int32_t Timer::Get_asUSec(const uint32_t seg)
{
	if(seg >= m_vtrSeg.size())
		return -1;
	return (int32_t)(m_vtrSeg[seg].first * 1000 * 1000 + m_vtrSeg[seg].second);
}


float Timer::Get_asMSec(const uint32_t seg)
{
	if(seg >= m_vtrSeg.size())
		return -1.0;
	return (float)(m_vtrSeg[seg].first) * 1000.0 + (float)m_vtrSeg[seg].second / 1000.0;
}


float Timer::Get_asSec(const uint32_t seg)
{
	if(seg >= m_vtrSeg.size())
		return -1.0;
	return (float)(m_vtrSeg[seg].first) + (float)m_vtrSeg[seg].second / 1000.0 / 1000.0;
}

int32_t Timer::GetTotal_asUSec()
{
	int32_t time_us = 0;
	for(uint32_t i = 0; i < SegCnt(); i++)
		time_us += Get_asUSec(i); 
	return time_us; 
}


float Timer::GetTotal_asMSec()
{
	float time_ms = 0.0;
	for(uint32_t i = 0; i < SegCnt(); i++)
		time_ms += Get_asMSec(i); 
	return time_ms; 
}


float Timer::GetTotal_asSec()
{
	float time_s = 0;
	for(uint32_t i = 0; i < SegCnt(); i++)
	{
		time_s += Get_asSec(i); 
	}
	return time_s; 
}


string Timer::AsStringMSec()
{
	string str; 
	char stmp[64]; 

	sprintf(stmp, "%.3f(", GetTotal_asMSec()); 
	str = stmp; 

	for(uint32_t i = 0; i < SegCnt(); i++)
	{
		if(i == 0)
			sprintf(stmp, "%.3f", Get_asMSec(i)); 
		else 
			sprintf(stmp, "-%.3f", Get_asMSec(i)); 
		str += stmp; 	
	}
	str += ")"; 
	return str; 
}

