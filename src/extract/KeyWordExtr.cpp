#include "KeyWordExtr.h"
using namespace demeter_extract; 
#include "StringArray.h"
using namespace demeter_uti; 
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
using namespace std; 
#include <stdio.h>


KeyWordExtr::KeyWordExtr()
{
}


KeyWordExtr::~KeyWordExtr()
{
}


void KeyWordExtr::Clear()
{
	m_mapTerm2Idf.clear(); 
}


bool KeyWordExtr::LoadResource(const char* sResFile)
{
	if(!sResFile)
		return false;
	ifstream ifs(sResFile); 
	if(!ifs.is_open())
		return false; 
	Clear(); 	
	string str; 
	float idf; 

	while(!ifs.eof())
	{
		std::getline(ifs, str); 
		if(str.empty())
			continue; 
		StringArray array(str.c_str(), "\t"); 
		if(array.Count() < 2) 
			continue;
		sscanf(array.GetString(1).c_str(), "%f", &idf); 
		m_mapTerm2Idf.insert(pair<string,float>(array.GetString(0), idf)); 
	}

	ifs.close();

	printf("%d words idf has been read!\n", (int32_t)m_mapTerm2Idf.size()); 
	return true; 
}


int32_t KeyWordExtr::Extract(vector<WsPair>& vtrKeyWords, vector<string>& vtrInfoWords, const int32_t nTopN)
{
	vtrKeyWords.clear(); 
	WsPair ws; 
	map<string,float>::iterator iter_map; 
	set<string> set_filter; 
	for(size_t i = 0; i < vtrInfoWords.size(); i++) 
	{
		if(set_filter.find(vtrInfoWords[i]) != set_filter.end())
			continue; 
		set_filter.insert(vtrInfoWords[i]); 
		ws.m_strWord = vtrInfoWords[i]; 
		iter_map = m_mapTerm2Idf.find(ws.m_strWord); 
		if(iter_map != m_mapTerm2Idf.end())
			ws.m_fScore = iter_map->second; 
		else
			ws.m_fScore = 1.0; 
		vtrKeyWords.push_back(ws); 
	}

	sort(vtrKeyWords.begin(), vtrKeyWords.end(), WsPair::CompScore); 
	if(nTopN > 0 && nTopN < (int32_t)vtrKeyWords.size())
	{
		vector<WsPair>::iterator iter_vtr = vtrKeyWords.begin();
		iter_vtr += nTopN; 
		vtrKeyWords.erase(iter_vtr, vtrKeyWords.end()); 	
	}
        if(!vtrKeyWords.empty()){
                float max = vtrKeyWords[0].m_fScore; 
                for(size_t k = 0; k < vtrKeyWords.size(); k++) 
                {
                        vtrKeyWords[k].m_fScore /= max; 
                }
        }
	return (int32_t)vtrKeyWords.size(); 
}


