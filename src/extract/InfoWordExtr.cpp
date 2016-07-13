#include "InfoWordExtr.h"
using namespace demeter_extract;


InfoWordExtr::InfoWordExtr()
{
}


InfoWordExtr::~InfoWordExtr()
{
}


void InfoWordExtr::Clear()
{
	m_wordSpliter.clear();
}


bool InfoWordExtr::LoadResource(const char* sResFile, const char* sTradSimpFile)
{
	if(!sResFile)
		return false; 
	if(!m_wordSpliter.ReadWordsVec(sResFile))
		return false; 
	m_wordSpliter.ReadTradSimpMap(sTradSimpFile);
	return true; 
}


int32_t InfoWordExtr::Extract(vector<string>& vtrInfoWords, const char* sText)
{
	if(!sText)
		return -1; 
	vtrInfoWords.clear(); 

	vtrInfoWords = m_wordSpliter.SplitWords(sText);

	return (int32_t)vtrInfoWords.size(); 
}


