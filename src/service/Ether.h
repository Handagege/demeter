#ifndef _DEMETER_SERVICE_ETHER_H 
#define _DEMETER_SERVICE_ETHER_H 


#include <string>
using namespace std; 
#include <stdint.h>


namespace demeter_service
{

class Ether
{
private:
	// 构造函数 & 析构函数
	Ether(); 
	virtual ~Ether(); 

public:
	static bool StringToHostPort(string& strHost, int32_t& nPort, const char* sHostPort); 
	static string HostPortToString(const char* sHost, const int32_t nPort); 
	static string LocalIp(const char* ether = "eth0");
}; 

}


#endif /* _DEMETER_SERVICE_ETHER_H */

