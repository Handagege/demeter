// 文件名：JsonTransf.h
// 说明：json格式转换器JsonTransf的声明
// 最后更新：
//	v1.0, 2015-01-07 by fengyoung (fengyang3@staff.weibo.com)
//

#ifndef _DEMETER_SERVICE_JSON_TRANSF_H 
#define _DEMETER_SERVICE_JSON_TRANSF_H 

#include <string>
using namespace std; 
#include "json/json.h"

namespace demeter_service
{

// 类名：JsonTransf
// 说明：Json格式转换器
class JsonTransf
{
private:
	// 构造函数 & 析构函数
	JsonTransf(); 
	virtual ~JsonTransf(); 

public:
	// 函数名：JsonCppToString
	// 说明：将一个Json::Value对象转换成字符串
	// 参数：
	//	[IN] jsonValue - Json::Value对象
	//	[IN] bStyled - style标志
	// 返回值：Json串
	static string JsonCppToString(Json::Value& jsonValue, const bool bStyled = false); 
	
	// 函数名：StringToJsonCpp 
	// 说明：将一个json字符串转换成Json::Value对象
	// 参数：
	//	[OUT] jsonValue - Json::Value对象
	//	[IN] sJsonStr - Json字符串
	// 返回值：成功返回true，否则返回false
	static bool StringToJsonCpp(Json::Value& jsonValue, const char* sJsonStr); 
}; 

}

#endif /* _DEMETER_SERVICE_JSON_TRANSF_H */ 


