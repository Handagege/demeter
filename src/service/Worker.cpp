#include "Worker.h"
using namespace demeter_service;
#include <stdio.h>
#include "woo/log.h"


ThreadRWLock Worker::m_rwLock; 


Worker::Worker()
{
	m_pTextAnalyze = NULL;
}


Worker::~Worker()
{
	if(m_pTextAnalyze)
	{
		delete m_pTextAnalyze; 
		m_pTextAnalyze = NULL; 
	}
}


void Worker::Init(ServConf& servConf)
{
	m_servConf = servConf; 
}


void Worker::WorkCore(Json::Value& jsonReq, Json::Value& jsonResp)
{
	jsonResp.clear();
	try 
	{
		string str_cmd = jsonReq["cmd"].asString();
		if(str_cmd == "load_resource")
			Work_LoadResource(jsonReq["body"], jsonResp);
		else if(str_cmd == "text_analyze")
			Work_TextAnalyze(jsonReq["body"], jsonResp);
		else
		{
			jsonResp["ret"] = _DEMETER_SERV_UNSUPPORT_CMD; 
			jsonResp["msg"] = "unsupported cmd";
			LOG_ERROR("CMD [%s] is Unsupported!", str_cmd.c_str()); 
		}
	}
	catch(...)
	{
		jsonResp["ret"] = _DEMETER_SERV_EXCEPTION_OCCUR;
		jsonResp["msg"] = "exception occur";
		LOG_ERROR("Exception occur");
	}
}


void Worker::Work_LoadResource(Json::Value& jsonReqBody, Json::Value& jsonResp)
{
	char s_res_path[1024]; 
	sprintf(s_res_path, "%s/%s", m_servConf.m_strResPath.c_str(), jsonReqBody["resource_name"].asString().c_str()); 
	TextAnalyze* p_text_analyze = new TextAnalyze(); 
	TextAnalyze* p_old_text_analyze = m_pTextAnalyze;  	
	
	LOG_DEBUG("TEXT_ANAYZE | req_body: %s", JsonTransf::JsonCppToString(jsonReqBody).c_str()); 
	
	if(p_text_analyze->LoadResource(s_res_path))
	{
		m_rwLock.WrLock(); 
		m_pTextAnalyze = p_text_analyze; 
		m_rwLock.Unlock(); 

		if(p_old_text_analyze)
		{
			usleep(1000*100);
			delete p_old_text_analyze; 
		}

		jsonResp["ret"] = _DEMETER_SERV_SUCCESS; 
		jsonResp["msg"] = "ok";
		LOG_INFO("RESOURCE_LOAD | success, resource name is [%s]", jsonReqBody["resource_name"].asString().c_str()); 
	}
	else
	{
		delete p_text_analyze; 
		jsonResp["ret"] = _DEMETER_SERV_LOADRESOURCE_FAIL; 
		jsonResp["msg"] = string("failed to load resource from ") + jsonReqBody["resource_name"].asString();
		LOG_ERROR("RESOURCE_LOAD | failed, resource name is [%s]", jsonReqBody["resource_name"].asString().c_str()); 
	}
}


void Worker::Work_TextAnalyze(Json::Value& jsonReqBody, Json::Value& jsonResp)
{
	int32_t option = _TEXT_ANALYZE_OPTS_INFOWORD; 
	TextContent text_cont; 

	LOG_DEBUG("TEXT_ANAYZE | req_body: %s", JsonTransf::JsonCppToString(jsonReqBody).c_str()); 

	TextAnalyze* p_text_analyze = m_pTextAnalyze; 
	if(!p_text_analyze)
	{
		jsonResp["ret"] = _DEMETER_SERV_TEXTANALYZE_FAIL; 
		jsonResp["msg"] = "the analyzer is null"; 
		LOG_ERROR("TEXT_ANAYZE | failed, the analyzer is null"); 
	}
	else
	{
		option |= _TEXT_ANALYZE_OPTS_KEYWORD; 
		if(!jsonReqBody["keyword_topn"].isNull())
			text_cont.m_nTopN_Keyword = jsonReqBody["keyword_topn"].asInt(); 

		if(!jsonReqBody["extend_topn"].isNull())  
		{
			option |= _TEXT_ANALYZE_OPTS_EXTENDWORD; 
			text_cont.m_nTopN_Extend = jsonReqBody["extend_topn"].asInt(); 
			if(text_cont.m_nTopN_Extend > _MAX_TOP_N || text_cont.m_nTopN_Extend <= 0)
				text_cont.m_nTopN_Extend = _MAX_TOP_N; 
		}

		if(!jsonReqBody["topic_topn"].isNull())
		{
			option |= _TEXT_ANALYZE_OPTS_TOPIC; 
			text_cont.m_nTopN_Topic = jsonReqBody["topic_topn"].asInt(); 
			if(text_cont.m_nTopN_Topic > _MAX_TOP_N || text_cont.m_nTopN_Topic < 0)
				text_cont.m_nTopN_Topic = _MAX_TOP_N; 
		}

		p_text_analyze->Analyze(text_cont, jsonReqBody["text"].asString().c_str(), option); 
		char stmp[32]; 

		if(!text_cont.m_vtrInfoWords.empty())	
		{
			for(size_t i = 0; i < text_cont.m_vtrInfoWords.size(); i++)  
				jsonResp["info_words"].append(text_cont.m_vtrInfoWords[i]); 	
			if(!text_cont.m_vtrKeyWords.empty())	
			{
				Json::Value keyword_json;
				for(size_t i = 0; i < text_cont.m_vtrKeyWords.size(); i++)  
				{
					keyword_json["w"] = text_cont.m_vtrKeyWords[i].m_strWord;
					sprintf(stmp, "%.6g", text_cont.m_vtrKeyWords[i].m_fScore); 
					keyword_json["s"] = stmp; 
					jsonResp["keywords"].append(keyword_json); 
				}
			}
			if(!text_cont.m_vtrExtendWords.empty())	
			{
				Json::Value extend_json; 
				for(size_t i = 0; i < text_cont.m_vtrExtendWords.size(); i++)  
				{
					extend_json["w"] = text_cont.m_vtrExtendWords[i].m_strWord; 
					sprintf(stmp, "%.6g", text_cont.m_vtrExtendWords[i].m_fScore); 
					extend_json["s"] = stmp; 
					jsonResp["extend_words"].append(extend_json); 
				}
			}	
			if(!text_cont.m_vtrTopics.empty())	
			{
				Json::Value topic_json; 
				for(size_t i = 0; i < text_cont.m_vtrTopics.size(); i++)  
				{
					sprintf(stmp, "%d", text_cont.m_vtrTopics[i].first); 
					topic_json["tid"] = stmp; 
					sprintf(stmp, "%.6g", text_cont.m_vtrTopics[i].second); 
					topic_json["s"] = stmp; 
					jsonResp["topics"].append(topic_json); 
				}	
			}

			jsonResp["ret"] = _DEMETER_SERV_SUCCESS; 
			jsonResp["msg"] = "ok"; 
			LOG_INFO("TEXT_ANAYZE | success | %s", text_cont.TimeCostAsString().c_str()); 	
		}
		else
		{
			jsonResp["ret"] = _DEMETER_SERV_TEXTANALYZE_FAIL; 
			jsonResp["msg"] = "no info-word be extracted"; 
			LOG_INFO("TEXT_ANAYZE | failed, no info-word be extracted | %s", text_cont.TimeCostAsString().c_str()); 	
		}
	}
}



