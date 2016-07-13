#include "StringArray.h"
using namespace demeter_uti;  
#include "Ether.h"
using namespace demeter_service; 
#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <arpa/inet.h>   
#include <sys/socket.h>   
#include <sys/ioctl.h>   
#include <net/if.h>   


Ether::Ether()
{
}


Ether::~Ether()
{
}


bool Ether::StringToHostPort(string& strHost, int32_t& nPort, const char* sHostPort)
{
	StringArray ar(sHostPort, ":"); 
	if(ar.Count() != 2)
		return false; 
	sscanf(ar.GetString(1).c_str(), "%d", &nPort); 
	strHost = ar.GetString(0); 
	return true; 
}


string Ether::HostPortToString(const char* sHost, const int32_t nPort)
{
	char stmp[128]; 
	sprintf(stmp, "%s:%d", sHost, nPort);  
	return string(stmp); 
}


string Ether::LocalIp(const char* ether)
{
	int inet_sock;  
	struct ifreq ifr;  
	char ip[64];  

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);  
	strcpy(ifr.ifr_name, ether); 
	ioctl(inet_sock, SIOCGIFADDR, &ifr);  
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));  

	return string(ip); 
}



