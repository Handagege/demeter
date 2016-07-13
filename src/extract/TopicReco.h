#ifndef _DEMETER_TOPIC_RECO_H 
#define _DEMETER_TOPIC_RECO_H


#include <map>
#include <vector>
#include <string>
using namespace std; 
#include <stdint.h>
#include "WsPair.h"


namespace demeter_extract
{

// TopicReco
// 话题识别 
class TopicReco
{
public: 
	TopicReco(); 
	virtual ~TopicReco(); 
	
	void Clear(); 
	bool LoadResource(const char* sResFile); 
	
	int32_t Recognize(vector<pair<int32_t,float> >& vtrHitTopics, vector<string>& vtrTerms, const int32_t nTopN = 0); 
	int32_t Recognize(vector<pair<int32_t,float> >& vtrHitTopics, vector<WsPair>& vtrTerms, const int32_t nTopN = 0); 

private: 
	static bool CompScore(pair<int32_t,float> a, pair<int32_t,float> b);

private: 
	map<string, int32_t> m_mapTerm2Topic;	// "term->topic"
};

}

#endif /* _DEMETER_TOPIC_RECO_H */


