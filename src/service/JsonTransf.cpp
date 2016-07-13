// 文件名：JsonTransf.cpp
// 说明：json格式转换器JsonTransf的定义
// 最后更新：
//	v1.0, 2015-01-07 by fengyoung (fengyang3@staff.weibo.com)
//
#include "JsonTransf.h"
using namespace demeter_service; 


/////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

// 构造函数 
JsonTransf::JsonTransf()
{
}


// 析构函数
JsonTransf::~JsonTransf()
{
}


/////////////////////////////////////////////////////////////////////////////////
// Operations 

// 将一个Json::Value对象转换成字符串
string JsonTransf::JsonCppToString(Json::Value& jsonValue, const bool bStyled)
{
	string str; 
	if(bStyled)
	{
		Json::StyledWriter writer;
		str = writer.write(jsonValue);
	}
	else
	{
		Json::FastWriter writer;
		str = writer.write(jsonValue);
		str.erase(str.length()-1, 1);
	}
	return str;
}


// 将一个json字符串转换成Json::Value对象
bool JsonTransf::StringToJsonCpp(Json::Value& jsonValue, const char* sJsonStr)
{
	if(!sJsonStr)
		return false; 

	Json::Reader reader(Json::Features::strictMode()); 
	try
	{
		return reader.parse(sJsonStr, jsonValue); 
	}
	catch(...)
	{
		return false; 		
	}
}


