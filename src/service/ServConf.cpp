#include "ServConf.h"
#include "Ether.h"
using namespace demeter_service; 

#include "Config.h"
#include "StringArray.h"
using namespace demeter_uti;  
#include <iostream>
using namespace std; 
#include <stdio.h>


ServConf::ServConf()
{
	m_strHost = Ether::LocalIp(); 
	m_nPort = 19527; 
	m_bLongConn = false;
	m_nThreadNum = 3;
	m_nRecvBufSize = 1024 * 1024;
	m_nSendBufSize = 1024 * 1024;
	m_nRecvTimeout = 1000; 
	m_nSendTimeout = 1000; 
	m_strLogPath = "./log/"; 
	m_strLogLev = "ewidt";
	m_strResPath = "./resource/";
}


ServConf::ServConf(const ServConf& servConf)
{
	m_strHost = servConf.m_strHost; 
	m_nPort = servConf.m_nPort; 
	
	m_bLongConn = servConf.m_bLongConn;
	m_nThreadNum = servConf.m_nThreadNum;
	m_nRecvBufSize = servConf.m_nRecvBufSize;
	m_nSendBufSize = servConf.m_nSendBufSize;
	m_nRecvTimeout = servConf.m_nRecvTimeout; 
	m_nSendTimeout = servConf.m_nSendTimeout; 

	m_strLogPath = servConf.m_strLogPath; 
	m_strLogLev = servConf.m_strLogLev;
	
	m_strResPath = servConf.m_strResPath;
}


ServConf::~ServConf()
{
}


ServConf& ServConf::operator = (const ServConf& servConf)
{
	m_strHost = servConf.m_strHost; 
	m_nPort = servConf.m_nPort; 
	
	m_bLongConn = servConf.m_bLongConn;
	m_nThreadNum = servConf.m_nThreadNum;
	m_nRecvBufSize = servConf.m_nRecvBufSize;
	m_nSendBufSize = servConf.m_nSendBufSize;
	m_nRecvTimeout = servConf.m_nRecvTimeout; 
	m_nSendTimeout = servConf.m_nSendTimeout; 

	m_strLogPath = servConf.m_strLogPath; 
	m_strLogLev = servConf.m_strLogLev;
	
	m_strResPath = servConf.m_strResPath;
	
	return *this; 
}


bool ServConf::Read(const char* sConfigFile, const int32_t nPort)
{
	Config conf; 
	if(!conf.Read(sConfigFile))
		return false; 

	m_strHost = Ether::LocalIp(conf.GetVal_asString("ETHER", 0, "eth0").c_str()); 
	m_nPort = conf.GetVal_asInt("PORT", 0, m_nPort); 
	if(nPort > 0)
		m_nPort = nPort; 
	m_bLongConn = conf.GetVal_asBool("LONG_CONN", 0, m_bLongConn); 
	m_nThreadNum = conf.GetVal_asInt("THREAD_NUM", 0, m_nThreadNum); 
	m_nRecvBufSize = conf.GetVal_asInt("RECV_BUF_SIZE", 0, m_nRecvBufSize); 
	m_nSendBufSize = conf.GetVal_asInt("SEND_BUF_SIZE", 0, m_nSendBufSize); 
	m_nRecvTimeout = conf.GetVal_asInt("RECV_TIMEOUT", 0, m_nRecvTimeout);
	m_nSendTimeout = conf.GetVal_asInt("SEND_TIMEOUT", 0, m_nSendTimeout);
	m_strLogPath = conf.GetVal_asString("LOG_PATH", 0, m_strLogPath.c_str()); 
	m_strLogLev = conf.GetVal_asString("LOG_LEV", 0, m_strLogLev.c_str()); 
	m_strResPath = conf.GetVal_asString("RESOURCE_PATH", 0, m_strResPath.c_str()); 

	return true; 
}


string ServConf::LogFile()
{
	char stmp[1024];
	sprintf(stmp, "%s/demeter_serv_%s_%d.log", m_strLogPath.c_str(), m_strHost.c_str(), m_nPort); 
	return string(stmp); 
}


