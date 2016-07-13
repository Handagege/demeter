#ifndef _DEMETER_EXTRACT_INFOWORD_EXTR_H
#define _DEMETER_EXTRACT_INFOWORD_EXTR_H


#include <map>
#include <vector>
#include <string>
using namespace std; 
#include <stdint.h>
#include "WordSpliter.h"


namespace demeter_extract
{

class InfoWordExtr
{
public: 
	InfoWordExtr(); 
	virtual ~InfoWordExtr(); 

	void Clear(); 
	bool LoadResource(const char* sResFile, const char* sTradSimpFile); 

	int32_t Extract(vector<string>& vtrInfoWords, const char* sText); 

private:
	CSplitWords m_wordSpliter;

};

}


#endif /* _DEMETER_EXTRACT_INFOWORD_EXTR_H */

