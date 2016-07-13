#include "TopicReco.h"
using namespace demeter_extract;
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std; 
#include "StringArray.h"
using namespace demeter_uti; 
#include <stdio.h>


TopicReco::TopicReco()
{
}


TopicReco::~TopicReco()
{
}


void TopicReco::Clear()
{
	m_mapTerm2Topic.clear(); 
}


bool TopicReco::LoadResource(const char* sResFile)
{
	if(!sResFile)
		return false; 
	ifstream ifs(sResFile); 
	if(!ifs.is_open())
		return false; 

	Clear(); 
	string str; 	
	int32_t topic_id; 

	while(!ifs.eof())
	{
		std::getline(ifs, str); 
		if(str.empty())
			continue; 
		StringArray ar(str.c_str(), "\t"); 
		if(ar.Count() != 2)
			continue;
		sscanf(ar.GetString(1).c_str(), "%d", &topic_id); 

		m_mapTerm2Topic.insert(pair<string,int32_t>(ar.GetString(0), topic_id)); 
	}

	ifs.close(); 
	printf("%d word-topic mapping has been read!\n", (int32_t)m_mapTerm2Topic.size()); 
	return true; 
}


int32_t TopicReco::Recognize(vector<pair<int32_t,float> >& vtrHitTopics, vector<string>& vtrTerms, const int32_t nTopN)
{
	vtrHitTopics.clear();

	map<int32_t,float> map_hits; 
	map<int32_t,float>::iterator iter_map_hits; 
	map<string, int32_t>::iterator iter_map_topic; 

	for(size_t i = 0; i < vtrTerms.size(); i++) 
	{
		iter_map_topic = m_mapTerm2Topic.find(vtrTerms[i]); 
		if(iter_map_topic != m_mapTerm2Topic.end())
		{
			iter_map_hits = map_hits.find(iter_map_topic->second); 
			if(iter_map_hits != map_hits.end())
				iter_map_hits->second += 1.0; 
			else
				map_hits.insert(pair<int32_t,float>(iter_map_topic->second, 1.0)); 
		}
	}

	if(!map_hits.empty())
	{
		for(iter_map_hits = map_hits.begin(); iter_map_hits != map_hits.end(); iter_map_hits++) 
			vtrHitTopics.push_back(pair<int32_t,float>(iter_map_hits->first, iter_map_hits->second));

		sort(vtrHitTopics.begin(), vtrHitTopics.end(), CompScore); 
		if(nTopN > 0 && nTopN < (int32_t)vtrHitTopics.size())
		{
			vector<pair<int32_t,float> >::iterator iter_vtr = vtrHitTopics.begin(); 
			iter_vtr += nTopN;
			vtrHitTopics.erase(iter_vtr, vtrHitTopics.end()); 
		}
		float max = vtrHitTopics[0].second; 
		for(size_t k = 0; k < vtrHitTopics.size(); k++) 
			vtrHitTopics[k].second /= max; 
	}

	return (int32_t)vtrHitTopics.size(); 
}


int32_t TopicReco::Recognize(vector<pair<int32_t,float> >& vtrHitTopics, vector<WsPair>& vtrTerms, const int32_t nTopN)
{
	vtrHitTopics.clear();

	map<int32_t,float> map_hits; 
	map<int32_t,float>::iterator iter_map_hits; 
	map<string, int32_t>::iterator iter_map_topic; 

	for(size_t i = 0; i < vtrTerms.size(); i++) 
	{
		iter_map_topic = m_mapTerm2Topic.find(vtrTerms[i].m_strWord); 
		if(iter_map_topic != m_mapTerm2Topic.end())
		{
			iter_map_hits = map_hits.find(iter_map_topic->second); 
			if(iter_map_hits != map_hits.end())
				iter_map_hits->second += vtrTerms[i].m_fScore; 
			else
				map_hits.insert(pair<int32_t,float>(iter_map_topic->second, vtrTerms[i].m_fScore)); 
		}
	}

	if(!map_hits.empty())
	{
		for(iter_map_hits = map_hits.begin(); iter_map_hits != map_hits.end(); iter_map_hits++) 
			vtrHitTopics.push_back(pair<int32_t,float>(iter_map_hits->first, iter_map_hits->second));

		sort(vtrHitTopics.begin(), vtrHitTopics.end(), CompScore); 
		if(nTopN > 0 && nTopN < (int32_t)vtrHitTopics.size())
		{
			vector<pair<int32_t,float> >::iterator iter_vtr = vtrHitTopics.begin(); 
			iter_vtr += nTopN; 
			vtrHitTopics.erase(iter_vtr, vtrHitTopics.end()); 
		}
		float max = vtrHitTopics[0].second; 
		for(size_t k = 0; k < vtrHitTopics.size(); k++) 
			vtrHitTopics[k].second /= max; 
	}

	return (int32_t)vtrHitTopics.size(); 
}


bool TopicReco::CompScore(pair<int32_t,float> a, pair<int32_t,float> b)
{
	return a.second > b.second; 
}


