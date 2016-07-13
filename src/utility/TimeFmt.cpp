#include "TimeFmt.h"
using namespace demeter_uti;
#include <stdio.h>
#include <sys/time.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

// 构造函数 
TimeFmt::TimeFmt()
{
}


// 析构函数
TimeFmt::~TimeFmt()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations 

// 获取当前时间戳
uint32_t TimeFmt::CurTimeStamp()
{
	time_t lt = time(NULL);
	localtime(&lt); 
	return (uint32_t)lt;
}


// 获取当前时间（精确到秒），并且以字符串形式返回
string TimeFmt::CurTime_asStr(const ETimeFmtType eOutFmt)
{
	time_t lt = time(NULL);
	struct tm *ptr = localtime(&lt);
	char sTime[128];
	sTime[0] = '\0';
	if(eOutFmt == _TIME_FMT_STD)	// 标准格式："YYYY-MM-DD hh:mm:ss"
		strftime(sTime, 128, "%F %T",ptr);
	else if(eOutFmt == _TIME_FMT_NOBLANK)	// 无空格格式："YYYY-MM-DD_hh:mm:ss"
		strftime(sTime, 128, "%F_%T",ptr);
	else if(eOutFmt == _TIME_FMT_COMPACT)	// 紧凑格式："YYYYMMDDhhmmss"
	{
		sprintf(sTime, "%04d%02d%02d%02d%02d%02d", ptr->tm_year+1900, ptr->tm_mon+1,
			ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec);
	}
	else
		sTime[0] = '\0'; 
	return string(sTime);
}


// 将32位的时间戳转换成字符串
string TimeFmt::TimeConv_Uint32ToStr(const uint32_t unTimeStamp, const ETimeFmtType eOutFmt)
{
	time_t rawtime = (time_t)unTimeStamp;
	struct tm* stTime = localtime(&rawtime);
	char buf[256];
	if(eOutFmt == _TIME_FMT_STD)
	{ // 标准格式："YYYY-MM-DD hh:mm:ss"
		sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else if(eOutFmt == _TIME_FMT_NOBLANK)
	{ // 无空格格式："YYYY-MM-DD_hh:mm:ss"
		sprintf(buf, "%04d-%02d-%02d_%02d:%02d:%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else if(eOutFmt == _TIME_FMT_COMPACT)	
	{ // 紧凑格式："YYYYMMDDhhmmss"
		sprintf(buf, "%04d%02d%02d%02d%02d%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else
	{
		buf[0] = '\0';
	}
	return string(buf);
}


// 将字符串格式的时间转换成32位的时间戳
uint32_t TimeFmt::TimeConv_StrToUint32(const char* sTime, const ETimeFmtType eInFmt)
{
	time_t rawtime; 
	time(&rawtime);         
	struct tm* stTime = localtime(&rawtime);
	if(eInFmt == _TIME_FMT_STD)
	{ // 标准格式："YYYY-MM-DD hh:mm:ss"
		sscanf(sTime, "%d-%d-%d %d:%d:%d", &stTime->tm_year, &stTime->tm_mon,
			&stTime->tm_mday, &stTime->tm_hour, &stTime->tm_min, &stTime->tm_sec);
	}
	else if(eInFmt == _TIME_FMT_NOBLANK)
	{ // 无空格格式："YYYY-MM-DD_hh:mm:ss"
		sscanf(sTime, "%d-%d-%d_%d:%d:%d", &stTime->tm_year, &stTime->tm_mon,
			&stTime->tm_mday, &stTime->tm_hour, &stTime->tm_min, &stTime->tm_sec);
	}
	else
	{
		return 0;
	}

	stTime->tm_year -= 1900;        
	stTime->tm_mon -= 1;

	time_t ttTime = mktime(stTime); 
	return (uint32_t)ttTime;
}


// 将32位日期转换成字符串
string TimeFmt::DateConv_Uint32ToStr(const uint32_t unTimeStamp, const EDateFmtType eOutFmt)
{
	time_t rawtime = (time_t)unTimeStamp;
	struct tm* stTime = localtime(&rawtime);  
	char buf[256];
	if(eOutFmt == _DATE_FMT_STD) // 标准格式："YYYY-MM-DD"
		sprintf(buf, "%04d-%02d-%02d", stTime->tm_year+1900, stTime->tm_mon+1, stTime->tm_mday);
	else if(eOutFmt == _DATE_FMT_COMPACT)	// 紧凑格式："YYYYMMDD"
		sprintf(buf, "%04d%02d%02d", stTime->tm_year+1900, stTime->tm_mon+1, stTime->tm_mday);
	else
		buf[0] = '\0'; 
	return string(buf);
}	


// 将字符串格式的日期转换成32位的时间戳
uint32_t TimeFmt::DateConv_StrToUint32(const char* sDate)
{
	time_t rawtime;
	time(&rawtime);
	struct tm* stTime = localtime(&rawtime);
	sscanf(sDate, "%d-%d-%d", &stTime->tm_year, &stTime->tm_mon, &stTime->tm_mday);

	stTime->tm_hour = 0; 
	stTime->tm_min = 0;
	stTime->tm_sec = 0; 

	stTime->tm_year -= 1900;
	stTime->tm_mon -= 1;     

	time_t ttTime = mktime(stTime); 
	return (uint32_t)ttTime;  
}



