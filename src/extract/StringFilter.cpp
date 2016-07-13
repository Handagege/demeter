#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<stdlib.h>
#include<cctype>
#include <algorithm>
#include<stdint.h>
#include"StringFilter.h"
using namespace std;

//构造函数
CStringFilter::CStringFilter()
{
    str_seq = "+";
    ch_seq = '+';
    str_link1 = "url";
    str_link2 = "http";
	punct_value.push_back(0xE3808A);
	punct_value.push_back(0xE3808B);
	map_symbols.insert(make_pair("[","]"));
	map_symbols.insert(make_pair("(",")"));

}

//析构函数
CStringFilter::~CStringFilter()
{
}


//短链过滤，去除http和URL链接
//str_link：短链标志
//str_line：原始文本
void CStringFilter::RemovLinkTags(string& str_link, string &str_line)
{
	//去除url开头的链接
	int pos_b,pos_e;
	pos_b = str_line.find(str_link,0);
	while (pos_b != -1)
	{
		pos_e = pos_b+str_link.length();
		while(pos_e<str_line.length())
		{
			if(str_line[pos_e]>=0)
				pos_e++;
			else
			{
                if(pos_e+3>str_line.length())
                    break;
				if(str_line.substr(pos_e,3) == "：")
				{
					pos_e += 3;
				}
				else
					break;
			}
		}
		if(pos_e-pos_b>str_link.length()+1)
		{
			str_line.erase(pos_b,pos_e-pos_b);
			str_line.insert(pos_b,str_seq);
			pos_b = str_line.find(str_link,0);
		}
		else
			pos_b = str_line.find(str_link,pos_e);
	}
}

//过滤句子中包含[],##,短链字符串
//str_line：文本
//返回过滤噪声字符串后的文本
string CStringFilter::RemoveSpecialSymbol(string& str_line)
{
    int pos_b,pos_e;
    map<string,string>::iterator ite_symbols = map_symbols.begin();
    
    for(; ite_symbols!=map_symbols.end();ite_symbols++)
    {
        int symbol_len = (*ite_symbols).first.length();
        pos_b = str_line.find((*ite_symbols).first, 0);
        while(pos_b!=-1)
        {
            pos_e = str_line.find((*ite_symbols).second, pos_b+1);
            if(pos_e != -1)
            {
                str_line.erase(pos_b, pos_e-pos_b+symbol_len);
				str_line.insert(pos_b,str_seq);
                pos_b = str_line.find((*ite_symbols).first, 0);
            }
            else
                break;
        }
        
    }
	//去除url和http开头的链接
	RemovLinkTags(str_link1,str_line);
	RemovLinkTags(str_link2,str_line);
	return str_line;
}

//删除非中文和英文字符串
//str_line：原文本
//返回中英文文本
string CStringFilter::SwapNoise(string str_line)
{
	int pos_b = str_line.find(str_link2,0);
	int pos_e = str_line.find("com",pos_b+1);
	while(pos_b!=-1 && pos_e!=-1)
	{
		str_line.erase(pos_b,pos_e+3-pos_b);
		pos_b = str_line.find(str_link2,0);
		pos_e = str_line.find("com",pos_b+1);
	}

	//保留一个“|”
	int len = str_line.length();
	int pos = 0;
	string stand_weibo = "";
	while(pos<len)
	{
		if(str_line[pos]>=0)
		{
			int count = 0;
			int pos_b = pos;
			while(pos_b<len && str_line[pos_b]>=0)
			{
				if(str_line[pos_b]==ch_seq)
				{
					count++;
					pos_b++;
				}
				else
					break;
			}
			if(count==1 && pos_b==0)
				break;
			if (count>=2)
			{
				stand_weibo += str_seq;
				pos = pos_b;
			}
			else
			{
				stand_weibo += str_line[pos];
				pos++;
			}
		}
		else
		{
			stand_weibo += str_line.substr(pos,3);
			pos = pos + 3;
		}
	}
	//去除末尾的http，url标记
	pos_b = stand_weibo.rfind(str_link2);
	if(pos_b!=-1)
	{
		if(pos_b+str_link2.length()==stand_weibo.length())
		{
			if(pos_b-1>=0 && stand_weibo[pos_b-1]==ch_seq)
			{
				stand_weibo.erase(pos_b-1,str_link2.length()+1);
			}
			else
				stand_weibo.erase(pos_b,str_link2.length());
		}
	}
	else
	{
		pos_e = stand_weibo.rfind(str_link1);
		if((pos_e!=-1) && (pos_e+str_link1.length()==stand_weibo.length()))
		{
			if(pos_e-1>=0 && stand_weibo[pos_e-1]==ch_seq)
			{
				stand_weibo.erase(pos_e-1,str_link1.length()+1);
			}
			else
				stand_weibo.erase(pos_e,str_link1.length());
		}
	}

    return stand_weibo; 
}

//过滤非中文英文及特殊符号的字符串
//pBuffer：原始文本
//size：文本大小
string CStringFilter::IsUTF8(const char* pBuffer, long size)
{  
    char word[4]={"\0"};

    string str_word = "";
	int last_pos = 0;
	int temp_pos = 0;
	//汉字的编码范围
	uint32_t min = 0xE4B880;
	uint32_t max = 0xE9BEA0;

	uint32_t word_code = 0;
    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;
    while (start < end)
    {
        if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符
        {

			bool flag = false;
			//英文标点符号范围
			if((*start>=0x41 && *start<=0x5A) || (*start>=0x61 && *start <= 0x7A))
			{

				if(last_pos != temp_pos)
					str_word += str_seq;
				if(*start>=0x41 && *start<=0x5A)
					str_word += tolower(*start);
				else
					str_word += (*start);
				flag = true;
			}
			else if((*start>=0x30 && *start<=0x39) || (*start == '-') || (*start == '_') || (*start == '@'))	
			{
				if(last_pos != temp_pos)
					str_word += str_seq;
				str_word += (*start);
				flag = true;
			}
            start++;
			temp_pos++;
			if(flag == true)
				last_pos = temp_pos;
		}
        else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
        {
			start++;
			temp_pos++;
        }
        else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符
        {
            if (start >= end - 1)
                break;
            start += 2;
			temp_pos += 2;
        }
        else if (*start < (0xF0)) //(11110000): 此范围内为3字节UTF-8字符
        {
            
            if (start >= end - 2)
                break;
			
			
			bool flag = false;
			if((*start >= (0xE3)) && (*start <= (0xE9))||(*start == 0xEF)) 
			{
				if((start[1] & (0xC0)) == 0x80 || (start[2] & (0xC0)) == 0x80)
				{

					word[0]=start[0];
                	word[1]=start[1];
                	word[2]=start[2];

					word_code |= ((uint32_t)start[0])<<16;
					word_code |= ((uint32_t)start[1])<<8;
					word_code |= (uint32_t)start[2];

					if((word_code >= min) && (word_code <= max))
					{
						if((temp_pos != last_pos) && (last_pos>0))
						{
							str_word += str_seq;
						}
						str_word += word;
						flag = true;
					}
					else
					{
						if(find(punct_value.begin(),punct_value.end(),word_code)!=punct_value.end())
						{
							if((temp_pos != last_pos) && (last_pos>0))
							{
								str_word += str_seq;
							}
							str_word += word;
							flag = true;
						}
					}
					word_code = 0;
				}

			}
			
            start += 3;
			temp_pos += 3;
			if(flag == true)
				last_pos = temp_pos;
        }
        else
        {
			start += 4;
			temp_pos += 4;
        }
    }
    return str_word;
}

//删除@标记
//str_words：文本
string CStringFilter::RemoveAtCall(string str_words)
{
	int pos_b = str_words.find('@',0);
	while(pos_b != -1)
	{
		int pos_e = str_words.find(ch_seq,pos_b);
		if(pos_e != -1)
			str_words.erase(pos_b,pos_e-pos_b);
		else
			str_words.erase(pos_b);
		pos_b = str_words.find('@',0);
	}
	
	int len = str_words.length();
	string str_w = "";
	for(int i=0; i<len; i++)
	{
		if(str_words[i]=='@'|| (str_words[i]=='-')||(str_words[i]=='_')||(str_words[i]>='0' && str_words[i]<='9'))
		{
			str_w += str_seq;
		}
		else
			str_w += str_words[i];
	}
	return str_w;
}

//读入繁简词表
bool CStringFilter::LoadTraditionSimpCh(const char* sTradSimpFile)
{
	ifstream fin(sTradSimpFile);
	if(!fin.is_open())
		return false; 
	string str_line;
	string str_trad="";
	string str_simp="";
	int n = 0;
	while(!fin.eof())
	{
		getline(fin,str_line);
		n++;
		if(str_line != "")
		{
			if(n%3 == 1)
				str_trad = str_line;
			else if(n%3 == 2)
			{
				str_simp = str_line;
            	int trad_len = str_trad.length()/3;
            	int simp_len = str_simp.length()/3;
            	string trad_word = "";
            	string simp_word = "";
            	if(trad_len == simp_len)
            	{
            		for(int i=0; i<trad_len; i++)
            		{
            			trad_word = str_trad.substr(i*3,3);
            			simp_word = str_simp.substr(i*3,3);
            			if(trad_word != simp_word)
            			{
            				if(map_trad_simp_ch.find(trad_word)==map_trad_simp_ch.end())
            					map_trad_simp_ch.insert(make_pair(trad_word,simp_word));
            				else
            				{
            					if(simp_word != map_trad_simp_ch[trad_word])
            						cout<<"the diff simp word!"<<endl;
            					else
            						continue;
            				}
            
            			}
            			else
                            continue;
                    }
                }
	            else
		            cout<<"not the same length!"<<trad_len<<"\t"<<simp_len<<endl;
            }
        }
    }
	fin.close();
	return true; 
}

//对句子进行繁简转化
//str_weibo：文本内容
//返回转换后的简体
string CStringFilter::TradToSimp(string str_weibo)
{
	int len = str_weibo.length();
	string str_w = "";
	string word = "";
	int count = 0;
	for(int i=0; i<len; )
	{
		if(str_weibo[i]>=0)
		{
			str_w += str_weibo[i];
			i++;
		}
		else
		{
			count++;
			word = str_weibo.substr(i,3);

			if(map_trad_simp_ch.find(word) != map_trad_simp_ch.end())
				str_w += map_trad_simp_ch[word];
			else
				str_w += word;
			i += 3;
		}
	}
	if(count>=3 or str_w.length()>=9)
		return str_w;
	else
		return "";
}

//对文本进行去噪
//str_weibo：文本内容
//返回去噪后的文本
string CStringFilter::FilterStringNoise(string str_weibo)
{
    //读取词向量
    //str_weibo = "生活里的抽象画】街头摄影有许多不同的表现形式，在摄影师Klaus von Frieling眼中，明亮的色彩和错落有致的几何线条就足以将生活的点滴展现出来。";
	string str = RemoveSpecialSymbol(str_weibo);
	//去除非中文标点符号
	int size=str.length();
   	const char * pBuffer = str.c_str();
   	string str_words=IsUTF8(pBuffer, size);
	string str_no_at = RemoveAtCall(str_words);
    //去除末尾的http,url等标记
    string str_res = SwapNoise(str_no_at);
	//将繁体字转换成简体字
	string str_result = TradToSimp(str_res);
    //cout<< str_result<<endl;
    return str_result;
}
