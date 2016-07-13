#include<string>
#include<iostream>
#include<vector>
#include<map>
#include<math.h>
#include"StringFilter.h"

using namespace std;

class CSplitWords
{
    public:
        //构造函数
        CSplitWords();

        //析构函数
        ~CSplitWords();
        

        //释放资源
        //zhanghan add
        void clear();


        //字符串切分
        //sStr：字符串
        //sDilm：分隔符
        void StringSegment(vector<string>& vtrString, string sStr, string sDilm);

        //判断在词表中是否存在
        //sWords：要查找的词语
        //返回值true表示能找到，false表示找不到
        bool IsWords(string sWords);

        //将词向量表存入map容器中，词为key值
        bool ReadWordsVec(const char* sResFile);
        bool ReadTradSimpMap(const char* sTradSimpFile){
                return cStringFilter.LoadTraditionSimpCh(sTradSimpFile);
        }

        //对字符串进行拆分，拆成单个的汉字和英文词并存在容器中
        //sSubStr：要拆分的字符串
        //返回拆分后存储的容器
        vector<string> SplitSingleWord(string& sSubStr);

        //采用后向遍历进行分词：原则上选取尽量长的词语作为词语
        //vecWord：拆分后的顺序字词容器
        //vSize：容器大小
        //bPos：字的起始位置
        //len：最大的词语长度
        //返回最合适的词语
        string FwwWords(vector<string>& vecWord, int vSize, int& bPos, int len);

        //采用后向遍历进行分词：原则上选取尽量长的词语作为词语
        //vecWord：拆分后的顺序字词容器
        //vSize：容器大小
        //bPos：字的起始位置
        //len：最大的词语长度
        //返回最合适的词语
        string BwwWords(vector<string>& vecWord, int vSize, int& bPos, int len);

        //分词：根据前向遍历和后向遍历中未参与进来的字个数选取合适的分词结果
        //sTxt:去掉特殊字符后的文本内容
        //返回分词结果
        vector<string> SplitWords(const char* sText);

        
        //查找专有名词
        string FindProperNoun(const string& sTxt, vector<string>& vtrProper);

        //专有名词格式变换
        //candProper：原始专有名词
        string TransferFormat(string& candProper);


    private:
        //分割后的字符串
        vector<string> m_vtrString; 
        //词向量
        map<string, vector<float> > m_mapWordsVec;
        //第一个句子的词语集合

        string str_seq;
        char ch_seq;
        CStringFilter cStringFilter;

};
