#ifndef _DEMETER_EXTRACT_WS_PAIR_H 
#define _DEMETER_EXTRACT_WS_PAIR_H 

#include <string>
using namespace std; 
#include <stdint.h>


namespace demeter_extract
{

class WsPair
{
public: 	
	WsPair(); 
	WsPair(const char* sWord, const float fScore); 
	WsPair(const WsPair& wsPair); 
	virtual ~WsPair(); 
	
	WsPair& operator = (const WsPair& wsPair); 

	static bool CompScore(WsPair a, WsPair b); 

public: 
	string m_strWord; 
	float m_fScore; 
}; 

}

#endif /* _DEMETER_EXTRACT_WS_PAIR_H */


