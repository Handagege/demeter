#ifndef _DEMETER_UTILITY_TIME_FMT_H 
#define _DEMETER_UTILITY_TIME_FMT_H 

#include <string>
using namespace std;
#include <stdint.h>

namespace demeter_uti
{

// 时间的字符串格式类型
enum ETimeFmtType
{
	_TIME_FMT_STD,		// 标准格式："YYYY-MM-DD hh:mm:ss"
	_TIME_FMT_NOBLANK,	// 无空格格式："YYYY-MM-DD_hh:mm:ss"
	_TIME_FMT_COMPACT	// 紧凑格式："YYYYMMDDhhmmss"
};


// 日期的字符串格式类型
enum EDateFmtType
{
	_DATE_FMT_STD,		// 标准格式："YYYY-MM-DD"
	_DATE_FMT_COMPACT	// 紧凑格式："YYYYMMDD"
};


class TimeFmt
{
private:
	// 构造函数 & 析构函数
	TimeFmt();
	virtual ~TimeFmt();

public:
	// 函数名：CurTimeStamp
	// 说明：获取当前时间戳
	// 返回值：32位的时间戳
	static uint32_t CurTimeStamp();

	// 函数名：CurTime_asStr
	// 说明：获取当前时间（精确到秒），并且以字符串形式返回
	// 参数：
	//	[IN] eOutFmt - 输出的时间字符串的格式
	// 返回值：表示当前时间的字符串
	static string CurTime_asStr(const ETimeFmtType eOutFmt = _TIME_FMT_STD);

	// 函数名：TimeConv_Uint32ToStr
	// 说明：将32位的时间戳转换成字符串
	// 参数：
	//	[IN] unTimeStamp - 输入的时间戳
	//	[IN] eOutFmt - 输出的时间字符串的格式
	// 返回值：表示时间的字符串
	static string TimeConv_Uint32ToStr(const uint32_t unTimeStamp, const ETimeFmtType eOutFmt = _TIME_FMT_STD);
	
	// 函数名：TimeConv_StrToUint32
	// 说明：将字符串格式的时间转换成32位的时间戳
	// 参数：
	//	[IN] sTime - 输入的时间字符串
	//	[IN] eInFmt - 输入的时间字符串的格式
	// 返回值：32位时间戳
	static uint32_t TimeConv_StrToUint32(const char* sTime, const ETimeFmtType eInFmt = _TIME_FMT_STD);

	// 函数名：DateConv_Uint32ToStr
	// 说明：将32位日期转换成字符串
	// 参数：
	//	[IN] unTimeStamp - 输入的时间戳
	//	[IN] eOutFmt - 输出的日期字符串的格式
	// 返回值：字符串格式的日期，输出格式：YYYY-MM-DD
	static string DateConv_Uint32ToStr(const uint32_t unTimeStamp, const EDateFmtType eOutFmt = _DATE_FMT_STD);
	
	// 函数名：DateConv_StrToUint32
	// 说明：将字符串格式的日期转换成32位的时间戳
	// 参数：
	//	[IN] sDate - 输入的日期字符串，格式：YYYY-MM-DD
	// 返回值：32位时间戳
	static uint32_t DateConv_StrToUint32(const char* sDate);
};

}


#endif /* _DEMETER_UTILITY_TIME_FMT_H */

