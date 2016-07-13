#include "ExtendWordExtr.h"
using namespace demeter_extract; 
#include "StringArray.h"
using namespace demeter_uti; 
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std; 
#include <stdio.h>


ExtendWordExtr::ExtendWordExtr()
{
}


ExtendWordExtr::~ExtendWordExtr()
{
}


void ExtendWordExtr::Clear()
{
	m_mapTerm2Extends.clear(); 
}


bool ExtendWordExtr::LoadResource(const char* sResFile)
{
	if(!sResFile)
		return false;
	ifstream ifs(sResFile); 
	if(!ifs.is_open())
		return false; 

	Clear(); 	
	string str; 
	vector<WsPair> vtr_ws; 
	WsPair ws; 

	while(!ifs.eof())
	{
		std::getline(ifs, str); 
		if(str.empty())
			continue; 
		StringArray array(str.c_str(), "\t"); 
		if(array.Count() < 2) 
			continue; 
		vtr_ws.clear(); 
		for(int32_t i = 1; i < array.Count(); i++) 
		{
			StringArray ar(array.GetString(i).c_str(), ":"); 
			if(ar.Count() != 2)
				continue; 
			ws.m_strWord = ar.GetString(0); 
			sscanf(ar.GetString(1).c_str(), "%f", &ws.m_fScore);
			vtr_ws.push_back(ws); 
		}
		m_mapTerm2Extends.insert(pair<string,vector<WsPair> >(array.GetString(0), vtr_ws)); 
	}

	ifs.close(); 
	
	printf("%d word-extend mapping has been read!\n", (int32_t)m_mapTerm2Extends.size()); 
	return true; 
}



int32_t ExtendWordExtr::Extract(vector<WsPair>& vtrExtend, vector<string>& vtrTerms, const int32_t nTopN) 
{
	vtrExtend.clear(); 

	map<string, float> map_ext; 
	map<string, vector<WsPair> >::iterator iter_map0; 
	map<string, float>::iterator iter_map1; 

	for(size_t i = 0; i < vtrTerms.size(); i++) 
	{
		iter_map0 = m_mapTerm2Extends.find(vtrTerms[i]);
		if(iter_map0 != m_mapTerm2Extends.end())
		{
			for(size_t j = 0; j < iter_map0->second.size(); j++)
			{
				iter_map1 = map_ext.find(iter_map0->second[j].m_strWord); 
				if(iter_map1 != map_ext.end())
					iter_map1->second += iter_map0->second[j].m_fScore; 
				else
					map_ext.insert(pair<string,float>(iter_map0->second[j].m_strWord, iter_map0->second[j].m_fScore)); 
			}
		}
	}

	if(!map_ext.empty())	
	{
		for(iter_map1 = map_ext.begin(); iter_map1 != map_ext.end(); iter_map1++) 
			vtrExtend.push_back(WsPair(iter_map1->first.c_str(), iter_map1->second)); 
		sort(vtrExtend.begin(), vtrExtend.end(), WsPair::CompScore); 
		if(nTopN > 0 && nTopN < (int32_t)vtrExtend.size())
		{
			vector<WsPair>::iterator iter_vtr = vtrExtend.begin();
			iter_vtr += nTopN; 
			vtrExtend.erase(iter_vtr, vtrExtend.end()); 	
		}	
		float max = vtrExtend[0].m_fScore; 
		for(size_t k = 0; k < vtrExtend.size(); k++) 
		{
			vtrExtend[k].m_fScore /= max; 
		}
	}

	return (int32_t)vtrExtend.size(); 
}


int32_t ExtendWordExtr::Extract(vector<WsPair>& vtrExtend, vector<WsPair>& vtrTerms, const int32_t nTopN)
{
	vtrExtend.clear(); 

	map<string, float> map_ext; 
	map<string, vector<WsPair> >::iterator iter_map0; 
	map<string, float>::iterator iter_map1; 

	for(size_t i = 0; i < vtrTerms.size(); i++) 
	{
		iter_map0 = m_mapTerm2Extends.find(vtrTerms[i].m_strWord);
		if(iter_map0 != m_mapTerm2Extends.end())
		{
			for(size_t j = 0; j < iter_map0->second.size(); j++)
			{
				iter_map1 = map_ext.find(iter_map0->second[j].m_strWord); 
				if(iter_map1 != map_ext.end())
					iter_map1->second += iter_map0->second[j].m_fScore * vtrTerms[i].m_fScore; 
				else
					map_ext.insert(pair<string,float>(iter_map0->second[j].m_strWord, iter_map0->second[j].m_fScore * vtrTerms[i].m_fScore)); 
			}
		}
	}

	if(!map_ext.empty())	
	{
		for(iter_map1 = map_ext.begin(); iter_map1 != map_ext.end(); iter_map1++) 
			vtrExtend.push_back(WsPair(iter_map1->first.c_str(), iter_map1->second)); 
		sort(vtrExtend.begin(), vtrExtend.end(), WsPair::CompScore); 
		if(nTopN > 0 && nTopN < (int32_t)vtrExtend.size())
		{
			vector<WsPair>::iterator iter_vtr = vtrExtend.begin();
			iter_vtr += nTopN; 
			vtrExtend.erase(iter_vtr, vtrExtend.end()); 	
		}	
		float max = vtrExtend[0].m_fScore; 
		for(size_t k = 0; k < vtrExtend.size(); k++) 
			vtrExtend[k].m_fScore /= max; 
	}

	return (int32_t)vtrExtend.size(); 
}




