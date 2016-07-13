#ifndef _DEMETER_EXTRACT_EXTENDWORD_EXTR_H
#define _DEMETER_EXTRACT_EXTENDWORD_EXTR_H


#include <map>
#include <vector>
#include <string>
using namespace std; 
#include <stdint.h>
#include "WsPair.h"


namespace demeter_extract
{

// ExtendWordExtr
// 扩展词提取器
class ExtendWordExtr
{
public: 
	ExtendWordExtr(); 
	virtual ~ExtendWordExtr(); 
	
	void Clear(); 
	bool LoadResource(const char* sResFile); 

	int32_t Extract(vector<WsPair>& vtrExtend, vector<string>& vtrTerms, const int32_t nTopN = 0); 
	int32_t Extract(vector<WsPair>& vtrExtend, vector<WsPair>& vtrTerms, const int32_t nTopN = 0); 

private:
	map<string, vector<WsPair> > m_mapTerm2Extends;
};

}


#endif /* _DEMETER_EXTRACT_EXTENDWORD_EXTR_H */

