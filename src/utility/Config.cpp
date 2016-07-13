#include "Config.h"
#include "StringArray.h"
using namespace demeter_uti;
#include <fstream>
using namespace std; 
#include <stdio.h>


//////////////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

Config::Config()
{
}


Config::~Config()
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Operations 

bool Config::Read(const char* sConfigFile, const char* sSep)
{
	ifstream ifs(sConfigFile); 
	if(!ifs.is_open())
		return false; 

	m_mapKeyToValues.clear(); 

	string str, strkey, strvals, strval; 
	map<string,vector<string> >::iterator iter_map; 
	bool flag = true; 

	while(!ifs.eof())
	{
		std::getline(ifs, str); 
		if(str.empty())
			continue;
		if(str.at(0) == '#')
			continue; 

		StringArray ar1(str.c_str(), "="); 
		StringArray ar2(str.c_str(), "+="); 
		if(ar1.Count() == 2)
		{
			strkey = ar1.GetString(0);	
			strvals = ar1.GetString(1); 	
			flag = true;
		}	
		else if(ar2.Count() == 2)
		{
			strkey = ar2.GetString(0);	
			strvals = ar2.GetString(1); 	
			flag = false;	
		}
		else
			continue; 

		IgnoreSpace(strkey); 
		IgnoreSpace(strvals); 
		StringArray ar_vals(strvals.c_str(), sSep); 

		iter_map = m_mapKeyToValues.find(strkey); 
		if(iter_map == m_mapKeyToValues.end())
		{
			vector<string> vtr;
			for(int32_t i = 0; i < ar_vals.Count(); i++) 
			{
				strval = ar_vals.GetString(i);
				IgnoreSpace(strval); 
				vtr.push_back(strval); 
			}
			m_mapKeyToValues.insert(pair<string, vector<string> >(strkey, vtr));
		}
		else
		{
			if(flag)
				iter_map->second.clear(); 
			for(int32_t i = 0; i < ar_vals.Count(); i++) 
			{
				strval = ar_vals.GetString(i);
				IgnoreSpace(strval); 
				iter_map->second.push_back(strval);
			}
		}
	}

	ifs.close(); 
	return true; 
}


int32_t Config::ValCnt(const char* sKey)
{
	map<string,vector<string> >::iterator iter_map = m_mapKeyToValues.find(sKey); 
	if(iter_map == m_mapKeyToValues.end())
		return 0; 
	return (int32_t)iter_map->second.size(); 
}


string Config::GetVal_asString(const char* sKey, const int32_t nIdx, const char* sDefault)
{
	map<string,vector<string> >::iterator iter_map = m_mapKeyToValues.find(sKey); 
	if(iter_map == m_mapKeyToValues.end())
		return string(sDefault); 
	if(nIdx < 0 || nIdx >= (int32_t)iter_map->second.size())
		return string(sDefault); 
	return string(iter_map->second[nIdx]); 
}


int32_t Config::GetVal_asInt(const char* sKey, const int32_t nIdx, const int32_t nDefault)
{
	map<string,vector<string> >::iterator iter_map = m_mapKeyToValues.find(sKey); 
	if(iter_map == m_mapKeyToValues.end())
		return nDefault; 
	if(nIdx < 0 || nIdx >= (int32_t)iter_map->second.size())
		return nDefault; 
	int32_t val; 
	sscanf(iter_map->second[nIdx].c_str(), "%d", &val); 
	return val; 
}


double Config::GetVal_asFloat(const char* sKey, const int32_t nIdx, const double dDefault)
{
	map<string,vector<string> >::iterator iter_map = m_mapKeyToValues.find(sKey); 
	if(iter_map == m_mapKeyToValues.end())
		return dDefault; 
	if(nIdx < 0 || nIdx >= (int32_t)iter_map->second.size())
		return dDefault; 
	double val; 
	sscanf(iter_map->second[nIdx].c_str(), "%lf", &val); 
	return val; 
}


bool Config::GetVal_asBool(const char* sKey, const int32_t nIdx, const bool bDefault)
{
	map<string,vector<string> >::iterator iter_map = m_mapKeyToValues.find(sKey); 
	if(iter_map == m_mapKeyToValues.end())
		return bDefault; 
	if(nIdx < 0 || nIdx >= (int32_t)iter_map->second.size())
		return bDefault; 
	if(iter_map->second[nIdx] == "true")
		return true;
	else
		return false;
}


string Config::ToString(const char* sSep) 
{
	map<string,vector<string> >::iterator iter_map; 
	string str; 	
	char stmp[64]; 	
	for(iter_map = m_mapKeyToValues.begin(); iter_map != m_mapKeyToValues.end(); iter_map++) 
	{
		if(iter_map == m_mapKeyToValues.begin())
			sprintf(stmp, "%s=", iter_map->first.c_str()); 
		else	
			sprintf(stmp, "|%s=", iter_map->first.c_str()); 
		str += stmp; 

		for(size_t i = 0; i < iter_map->second.size(); i++) 
		{
			if(i == 0)
				sprintf(stmp, "%s", iter_map->second[i].c_str());
			else
				sprintf(stmp, "%s%s", sSep, iter_map->second[i].c_str());
			str += stmp; 	
		}
	}
	return str; 	
}


bool Config::FromString(const char* sStr, const char* sSep)
{
	m_mapKeyToValues.clear(); 
	StringArray array(sStr, "|"); 
	for(int32_t i = 0; i < array.Count(); i++) 
	{
		StringArray ar(array.GetString(i).c_str(), "="); 
		if(ar.Count() != 2)
			return false; 
		StringArray ar_val(ar.GetString(1).c_str(), sSep); 
		vector<string> vtr; 	
		for(int32_t j = 0; j < ar_val.Count(); j++) 
			vtr.push_back(ar_val.GetString(j)); 	
		m_mapKeyToValues.insert(pair<string,vector<string> >(ar.GetString(0), vtr));	
	}
	return true; 
}


//////////////////////////////////////////////////////////////////////////////////////////
// Internal Operations 

void Config::IgnoreSpace(string& str)
{
	int32_t nOffset = 0; 
	while(nOffset < (int32_t)str.length()) 
	{
		if(str.at(nOffset) != ' ' && str.at(nOffset) != '\t' && str.at(nOffset) != '\r')
			break;
		nOffset++;
	}
	str = str.substr(nOffset, str.length() - nOffset);

	nOffset = str.length() - 1;
	while(nOffset >= 0)
	{
		if(str.at(nOffset) != ' ' && str.at(nOffset) != '\t' && str.at(nOffset) != '\r')
			break;
		nOffset--;
	}
	str = str.substr(0, nOffset+1);
}


