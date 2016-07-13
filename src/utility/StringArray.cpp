#include "StringArray.h" 
using namespace demeter_uti;
#include <string.h>


////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

StringArray::StringArray(const char* sStr, const char* sDilm) 
{ 
	Decompose(sStr, sDilm);  
} 


StringArray::~StringArray() 
{ 
}  


////////////////////////////////////////////////////////////////////////////
// Operations  

string StringArray::GetString(const int32_t nIdx) const 
{ 
	if(nIdx < 0 || nIdx >= (int32_t)m_vtrString.size()) 
		return string(""); 
	return m_vtrString[nIdx];  
}  


int32_t StringArray::Count() const 
{ 
	return (int32_t)m_vtrString.size();  
}  


////////////////////////////////////////////////////////////////////////////
// Internal Operations 

void StringArray::Decompose(const char* sStr, const char* sDilm) 
{ 
	string str(sStr);
       	str += string(sDilm) + string("EOF");	
	int32_t len = str.length(); 	
	int32_t offset = 0, pos; 
	while(offset < len)
	{ 
		pos = str.find(sDilm, offset); 
		if(pos == (int32_t)string::npos)  
			pos = str.length();
		if(pos == offset)
		{
			m_vtrString.push_back(string("")); 
		}
		else
		{		       
			m_vtrString.push_back(str.substr(offset, pos - offset)); 
		}
		offset = pos + strlen(sDilm); 
	}
	m_vtrString.pop_back(); 
}  


