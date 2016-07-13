#include "WsPair.h"
using namespace demeter_extract; 


WsPair::WsPair()
{
}


WsPair::WsPair(const char* sWord, const float fScore) : m_strWord(sWord), m_fScore(fScore) 
{
}


WsPair::WsPair(const WsPair& wsPair)
{
	m_strWord = wsPair.m_strWord; 
	m_fScore = wsPair.m_fScore; 
}


WsPair::~WsPair()
{
}


WsPair& WsPair::operator = (const WsPair& wsPair)
{
	m_strWord = wsPair.m_strWord; 
	m_fScore = wsPair.m_fScore; 
	return *this; 
}


bool WsPair::CompScore(WsPair a, WsPair b)
{
	return a.m_fScore > b.m_fScore; 
}


