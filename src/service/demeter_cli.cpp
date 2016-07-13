#include <iostream>
#include <fstream>
using namespace std; 
#include "JsonTransf.h"
using namespace demeter_service; 
#include "StringArray.h"
#include "Timer.h"
using namespace demeter_uti; 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "woo/binaryclient.h"

#define _RESP_BUF_SIZE  (1024*1024*8)


bool Send(const char* sHost, const int32_t nPort, const char* sCmd, Json::Value& jsonReqBody, Json::Value& jsonResp, 
			const int32_t nLogId = 2016, const int32_t nSendTimeoutMs = 2000, const int32_t nRecvTimeoutMs = 2000) 
{
	Json::Value json_req; 
	json_req["cmd"] = sCmd; 
	json_req["body"] = jsonReqBody; 
	string str_req = JsonTransf::JsonCppToString(json_req); 
	uint32_t send_len = str_req.length(); 
	char s_resp[_RESP_BUF_SIZE];
	uint32_t recv_len; 	

	woo::binary_client_t *cli = woo::binary_client_create(sHost, nPort, nSendTimeoutMs * 1000, nRecvTimeoutMs * 1000);  
	ssize_t ret = woo::binary_client_talk(cli, str_req.c_str(), send_len, s_resp, &recv_len, _RESP_BUF_SIZE, nLogId); 
	binary_client_destroy(cli); 
	
	if(ret) 
        {
                cout << "1 send jsonReqBody " << str_req << " False" << endl;
		return false; 
        }
	s_resp[recv_len] = '\0'; 

	if(!JsonTransf::StringToJsonCpp(jsonResp, s_resp))
        {
                cout << "2 send jsonReqBody " << str_req << " False" << endl;
		return false; 
        }

        cout << "3 send jsonReqBody " << str_req << " True" << endl;
	return true; 
}


void Load(const char* sHost, const int32_t nPort, const char* sResName)
{
	Timer timer; 
	Json::Value json_req_body, json_resp; 
	json_req_body["resource_name"] = sResName;
	
	timer.Start(); 
	bool flag = Send(sHost, nPort, "load_resource", json_req_body, json_resp, 2016, 1000*120, 1000*120); 
	timer.Stop(); 

	if(flag) 
	{
		printf("time_cost(ms): %.3f\n", timer.GetLast_asMSec()); 
		cout<<JsonTransf::JsonCppToString(json_resp)<<endl; 
	}
	else
	{
		cout<<"Fail"<<endl; 
	}
}


void Analyze(const char* sHost, const int32_t nPort, const char* sText)
{
	Timer timer; 	
	Json::Value json_req_body, json_resp; 
	json_req_body["keyword_topn"] = 5;
	json_req_body["extend_topn"] = 10;
	json_req_body["topic_topn"] = 3;
	json_req_body["text"] = sText; 

	timer.Start(); 	
	bool flag = Send(sHost, nPort, "text_analyze", json_req_body, json_resp); 
	timer.Stop(); 	

	if(flag)
	{
		printf("time_cost(ms): %.3f\n", timer.GetLast_asMSec()); 
		cout<<JsonTransf::JsonCppToString(json_resp)<<endl; 
	}
	else
	{
		cout<<"Fail"<<endl; 
	}
}


void AnalyzeFile(const char* sHost, const int32_t nPort, const char* sTextFile)
{
	string str; 
	Json::Value json_req_body, json_resp; 
	json_req_body["keyword_topn"] = 5;
	json_req_body["extend_topn"] = 10;
	json_req_body["topic_topn"] = 3;

	ifstream ifs(sTextFile); 
	if(!ifs.is_open())
	{
		cout<<"failed to open "<<sTextFile<<endl; 
		return; 
	}

	int32_t num = 0; 
	while(!ifs.eof())
	{
		std::getline(ifs, str); 
		if(str.empty())
			continue; 
		json_req_body["text"] = str; 

		Timer timer; 	
		timer.Start(); 	
		bool flag = Send(sHost, nPort, "text_analyze", json_req_body, json_resp); 
		timer.Stop(); 	
		
		cout<<"("<<num+1<<")\t"<<str<<endl; 
		printf("time_cost(ms): %.3f\n", timer.GetLast_asMSec());
		if(flag)
		{
			cout<<"RESP:\t"<<JsonTransf::JsonCppToString(json_resp)<<endl;
		}
		else
		{
			cout<<"FAIL"<<endl; 
		}
		cout<<"--"<<endl; 
		num++; 	
	}

	ifs.close(); 
}

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cout<<"usage: detemer_cli <host:port> [--load <resource_name>]"<<endl; 
		cout<<"                               [--analyze <text>]"<<endl; 
		cout<<"                               [--analyze-file <text_file>]"<<endl; 
		return -1; 	
	}

	string host; 
	int32_t port; 
	StringArray ar(argv[1], ":"); 
	if(ar.Count() != 2)
	{
		cout<<"error serv host:port"<<endl; 
		return -2; 
	}
	host = ar.GetString(0); 
	sscanf(ar.GetString(1).c_str(), "%d", &port); 

	if(strcmp(argv[2], "--load") == 0)
	{
		Load(host.c_str(), port, argv[3]); 
	}
	else if(strcmp(argv[2], "--analyze") == 0)
	{
		Analyze(host.c_str(), port, argv[3]); 
	}
	else if(strcmp(argv[2], "--analyze-file") == 0)
	{
		AnalyzeFile(host.c_str(), port, argv[3]); 
	}
	else
	{
		cout<<"unsupported cmd"<<endl; 
		return -3; 
	}

	return 0; 
}



