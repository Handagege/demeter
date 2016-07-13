#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<stdlib.h>
#include<cctype>
#include <algorithm>
#include<stdint.h>
using namespace std;

class CStringFilter
{
    private:
        map<string,string> map_trad_simp_ch;
        map<string, string> map_symbols;
        vector<uint32_t> punct_value;
        
        string str_link1;
        string str_link2;
        char ch_seq;
        string str_seq;

    public:

         //构造函数
        CStringFilter();

        //析构函数
        ~CStringFilter();
        
        void clear(){ map_trad_simp_ch.clear(); }

        //短链过滤，去除http和URL链接
        //str_link：短链标志
        //str_line：原始文本 
        void RemovLinkTags(string& str_link, string& str_line);
        
        //过滤句子中包含[],##,短链字符串
        //str_line：文本
        //返回过滤噪声字符串后的文本
        string RemoveSpecialSymbol(string& str_line);
        
        //删除中文和英文字符串
        //str_line：原文本
        //返回中英文文本
        string SwapNoise(string str_line);
        
        //过滤非中文英文及特殊符号的字符串
        //pBuffer：原始文本
        //size：文本大小   
        string IsUTF8(const char* pBuffer, long size);
        
        //删除@标记
        //str_words：文本
        string RemoveAtCall(string str_words);
       
        void GetTradSimpCh(string str_trad,string str_simp);
        
        //读入繁简词表 
        bool LoadTraditionSimpCh(const char* sTradSimpFile);
        
        //对句子进行繁简转化
        //str_weibo：文本内容
        //返回转换后的简体
        string TradToSimp(string str_weibo);
        
        //对文本进行去噪
        //str_weibo：文本内容
        //返回去噪后的文本
        string FilterStringNoise(string str_weibo);
};
