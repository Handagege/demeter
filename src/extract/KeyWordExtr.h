#ifndef _DEMETER_EXTRACT_KEYWORD_EXTR_H
#define _DEMETER_EXTRACT_KEYWORD_EXTR_H


#include <map>
#include <vector>
#include <string>
using namespace std; 
#include <stdint.h>
#include "WsPair.h"


namespace demeter_extract
{

class KeyWordExtr
{
public: 
	KeyWordExtr(); 
	virtual ~KeyWordExtr(); 

	void Clear(); 
	bool LoadResource(const char* sResFile); 

	int32_t Extract(vector<WsPair>& vtrKeyWords, vector<string>& vtrInfoWords, const int32_t nTopN = 0); 

private:
	map<string,float> m_mapTerm2Idf; 
};

}


#endif /* _DEMETER_EXTRACT_KEYWORD_EXTR_H */

