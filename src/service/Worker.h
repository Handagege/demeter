#ifndef _DEMETER_SERVICE_WORKER_H 
#define _DEMETER_SERVICE_WORKER_H 

#include <string>
using namespace std;
#include "demeter_extract.h"
using namespace demeter_extract; 
#include <stdint.h>
#include "ServConf.h"
#include "ThreadRWLock.h"
#include "JsonTransf.h"
#include <unistd.h>

#define _MAX_TOP_N	20

#define _DEMETER_SERV_SUCCESS			0
#define _DEMETER_SERV_PARSEREQ_FAIL		-1

#define _DEMETER_SERV_EXCEPTION_OCCUR	-11
#define _DEMETER_SERV_UNSUPPORT_CMD		-12

#define _DEMETER_SERV_LOADRESOURCE_FAIL	-101
#define _DEMETER_SERV_TEXTANALYZE_FAIL	-102



namespace demeter_service
{

class Worker 
{
public: 
	Worker(); 
	virtual ~Worker(); 

	void Init(ServConf& servConf); 
	
	void WorkCore(Json::Value& jsonReq, Json::Value& jsonResp); 

private:
	void Work_LoadResource(Json::Value& jsonReqBody, Json::Value& jsonResp); 
	void Work_TextAnalyze(Json::Value& jsonReqBody, Json::Value& jsonResp); 

private:
	ServConf m_servConf; 
	TextAnalyze* m_pTextAnalyze;
	
	static ThreadRWLock m_rwLock; 
};

}

#endif /* _DEMETER_SERVICE_WORKER_H */

