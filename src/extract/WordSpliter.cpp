#include<string>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<map>
#include"WordSpliter.h"
//#include "woo/log.h"

//构造函数
CSplitWords::CSplitWords()
{
    str_seq = "+";
    ch_seq = '+';
}

//析构函数
CSplitWords::~CSplitWords()
{
}


void CSplitWords::clear()
{
    m_mapWordsVec.clear();
    cStringFilter.clear();
}


//字符串切分
//sStr：字符串
//sDilm：分隔符
void CSplitWords::StringSegment(vector<string>& vtrString, string sStr, string sDilm) 
{ 
    string str(sStr);
    //cout<<str<<endl;
    str += string(sDilm) + string("EOF");   
    int32_t nLen = str.length();    
    int32_t nOffset = 0, nPos; 
    vtrString.clear();
    while(nOffset < nLen)
    { 
        nPos = str.find(sDilm, nOffset); 
        if(nPos == string::npos)  
            nPos = str.length();
        if(nPos == nOffset)
        {
            vtrString.push_back(string("")); 
        }
        else
        {              
            vtrString.push_back(str.substr(nOffset, nPos - nOffset)); 
            //cout<<str.substr(nOffset, nPos - nOffset)<<endl;
        }
        nOffset = nPos + sDilm.length(); 
    }
    vtrString.pop_back(); 
}  


//判断在词表中是否存在
//sWords：要查找的词语
//返回值true表示能找到，false表示找不到
bool CSplitWords::IsWords(string sWords)
{
    map<string, vector<float> >::iterator iter;
    iter = m_mapWordsVec.find(sWords);
    if(iter != m_mapWordsVec.end())
        return true;
    else
        return false;

}

//将词向量表存入map容器中，词为key值
bool CSplitWords::ReadWordsVec(const char* sResFile)
{
    ifstream fin(sResFile, ios::in);
	if(!fin.is_open())
		return false; 
    string str_line = "";
    string sWords = "";
    vector<string> vtrString;
    while (!fin.eof())
    {
        getline(fin, str_line);
        StringSegment(vtrString,str_line, "\t");
        vector<float> m_vtrVec;
        int vSize = vtrString.size();
        if(vSize == 2)
        {
            sWords = vtrString[0];
            for(int i=1; i<vSize; ++i)
            {
                m_vtrVec.push_back(atof(vtrString[i].c_str()));
            }
            m_mapWordsVec[sWords] = m_vtrVec;
        }
    }
    cout << m_mapWordsVec.size() << " words vector has read over!"<<endl;
    fin.close();
	return true; 
}

//对字符串进行拆分，拆成单个的汉字和英文词并存在容器中
//sSubStr：要拆分的字符串
//返回拆分后存储的容器
vector<string> CSplitWords::SplitSingleWord(string& sSubStr)
{
    int sLen = sSubStr.length();
    vector<string> vecWord;
    string sWord = "";

    string candWords = "";
    for(int i=0; i<sLen; )
    {
        if(sSubStr[i]>='a' && sSubStr[i]<='z')
        {
            sWord += sSubStr[i];
            ++i;
        }
        else
        {
            if(sWord != "")
            {
                vecWord.push_back(sWord);
                sWord = "";
            }
            sWord =  sSubStr.substr(i,3);
            vecWord.push_back(sWord);
            sWord = "";
            i += 3;
        }
    }
    if(sWord != "")
        vecWord.push_back(sWord);
    return vecWord;
}

//采用前向遍历进行分词：原则上选取尽量长的词语作为词语
//vecWord：拆分后的顺序字词容器
//vSize：容器大小
//bPos：字的起始位置
//len：最大的词语长度
//返回最合适的词语
string CSplitWords::FwwWords(vector<string>& vecWord, int vSize, int& bPos, int len)
{
    string strWords = "";
    map<int, string> mapWords;

    for(int i=bPos; i<bPos+len; ++i)
    {
        strWords += vecWord[i];

        if(IsWords(strWords))
        {
            mapWords[i] = strWords;
        }
    }

    string correctWords = "";
    map<int,vector<int> > mapPos;
    map<int,string>::reverse_iterator iter = mapWords.rbegin();
    if(mapWords.size()>=2)
    {
        int count = 0;
        for(; iter!=mapWords.rend(); iter--)
        {
            int bPos = (*iter).first+1;
            string sWords = (*iter).second;

            int wLen = min(vSize-bPos, 4);
            string strWords = "";
            string tempWords = "";
            int tempPos = -1;
            for(int j=bPos; j<bPos+wLen; ++j)
            {
                strWords += vecWord[j];
                if(IsWords(strWords))
                {
                    tempWords = strWords;
                    tempPos = j;
                }
            }
            if(tempWords != "")
            {
                mapPos[count].push_back((*iter).first);
                mapPos[count].push_back(tempPos);
                if(m_mapWordsVec[tempWords][0] >= m_mapWordsVec[sWords][0])
                    mapPos[count].push_back(m_mapWordsVec[sWords][0]);
                else
                    mapPos[count].push_back(m_mapWordsVec[tempWords][0]);

            }
            else
            {
                mapPos[count].push_back((*iter).first);
                mapPos[count].push_back(0);
                mapPos[count].push_back(m_mapWordsVec[sWords][0]);
            }
            ++count;
            if(count==2)
                break;
        }
        if(mapPos[1][1] > mapPos[0][0])
        {
            if(mapPos[1][2] >= mapPos[0][2])
            {
                bPos = mapPos[1][0]+1;
                correctWords = mapWords[mapPos[1][0]];
            }
            else
            {
                bPos = mapPos[0][0]+1;
                correctWords = mapWords[mapPos[0][0]];
            }

        }
        else
        {
            bPos = mapPos[0][0]+1;
            correctWords = mapWords[mapPos[0][0]];
        }

    }
    if(mapWords.size() == 1)
    {
        bPos = (*iter).first+1;
        correctWords = (*iter).second;
    }
    if(mapWords.size() == 0)
        bPos = bPos+1;
    return correctWords; 
}


//采用后向遍历进行分词：原则上选取尽量长的词语作为词语
//vecWord：拆分后的顺序字词容器
//vSize：容器大小
//bPos：字的起始位置
//len：最大的词语长度
//返回最合适的词语
string CSplitWords::BwwWords(vector<string>& vecWord, int vSize, int& bPos, int len)
{
    string strWords = "";
    map<int, string> mapWords;

    //cout<<bPos<<" "<<len<<endl;
    for(int i=bPos; i>bPos-len; i--)
    {
        strWords = vecWord[i]+strWords;

        //cout<<strWords<<endl;
        if(IsWords(strWords))
        {
            mapWords[i] = strWords;
        }
    }

    string correctWords = "";
    vector<int> vecPos;
    map<int, vector<int> > mapPos;
    map<int,string>::iterator iter = mapWords.begin();
    if(mapWords.size()>=2)
    {
        int count = 0;
        for(; iter!=mapWords.end(); ++iter)
        {
            //cout<<"map:"<<(*iter).second<<endl;
            int bPos = (*iter).first-1;
            string sWords = (*iter).second;

            int wLen = min(bPos+1, 4);
            string strWords = "";
            string tempWords = "";
            int tempPos = -1;
            if(bPos>=0)
            {
                for(int j=bPos; j>bPos-wLen; j--)
                {
                    strWords = vecWord[j]+strWords;
                    if(IsWords(strWords))
                    {
                        tempWords = strWords;
                        tempPos = j;
                    }
                }
            }
            if(tempWords != "")
            {
                mapPos[count].push_back((*iter).first);
                mapPos[count].push_back(tempPos);
                if(m_mapWordsVec[tempWords][0] >= m_mapWordsVec[sWords][0])
                    mapPos[count].push_back(m_mapWordsVec[sWords][0]);
                else
                    mapPos[count].push_back(m_mapWordsVec[tempWords][0]);

            }
            else
            {
                mapPos[count].push_back((*iter).first);
                mapPos[count].push_back(0);
                mapPos[count].push_back(m_mapWordsVec[sWords][0]);
            }
            ++count;
            if(count==2)
                break;
        }
        if(mapPos[1][1] > mapPos[0][0])
        {
            if(mapPos[1][2] >= mapPos[0][2])
            {
                bPos = mapPos[1][0]-1;
                correctWords = mapWords[mapPos[1][0]];
            }
            else
            {
                bPos = mapPos[0][0]-1;
                correctWords = mapWords[mapPos[0][0]];
            }

        }
        else
        {
            bPos = mapPos[0][0]-1;
            correctWords = mapWords[mapPos[0][0]];
        }
    }
    if(mapWords.size() == 1)
    {
        bPos = (*iter).first-1;
        correctWords = (*iter).second;
    }
    if(mapWords.size() == 0)
        bPos = bPos-1;
    //cout<<"correct:"<<correctWords<<" "<<bPos<<endl;
    return correctWords; 
}

string CSplitWords::TransferFormat(string& candProper)
{
    if(candProper[0]==ch_seq)
        candProper.erase(0,1);
    int len = candProper.length();
    if(candProper[len-1]==ch_seq)
        candProper.erase(len-1,1);
    int pos = candProper.find(str_seq,0);

    if(pos==-1)
        return candProper+str_seq;
    else
    {
        return candProper;
    }
    
}

string CSplitWords::FindProperNoun(const string& sTxt, vector<string>& vtrProper)
{
    vtrProper.clear();
    int pos_b = 0;
    int pos_r = sTxt.find("》", pos_b);
    vector<string> vecSubStr;
    if(pos_r == -1)
        return sTxt;
   
    string strWeibo = "";
    while(pos_r!=-1)
    {
        
        int pos_l = sTxt.rfind("《", pos_r);
        if(pos_l!=-1 && pos_r>=pos_l && pos_l>=pos_b)
        {
            string candProper = sTxt.substr(pos_l+3,pos_r-pos_l-2);
            string strFormat = TransferFormat(candProper);
            if(IsWords(strFormat))
            {
                vtrProper.push_back(candProper);
                strWeibo += sTxt.substr(pos_b, pos_l-pos_b-1)+str_seq;
            }
            else
            {
                strWeibo += sTxt.substr(pos_b, pos_l-pos_b-1)+str_seq+candProper+str_seq;
            }
        }
        else
        {
            strWeibo += sTxt.substr(pos_b,pos_r-pos_b+1)+str_seq+sTxt.substr(pos_r+3);
        }
        pos_b = pos_r+3;
        pos_r = sTxt.find("》", pos_b);
        
    }
    return strWeibo;
}


//分词：根据前向遍历和后向遍历中未参与进来的字个数选取合适的分词结果
//sTxt:去掉特殊字符后的文本内容
//返回分词结果
vector<string> CSplitWords::SplitWords(const char* sText)
{
    //cout<<sTxt<<endl;
    string str_wb = cStringFilter.FilterStringNoise(string(sText));
    //专有名词列表
    vector<string> vtrProper;
    string sTempTxt = FindProperNoun(str_wb,vtrProper);

    vector<string> vtrString;
    StringSegment(vtrString, sTempTxt, str_seq);
    int sSize = vtrString.size();
    vector<string> vecFww;
    vector<string> vecBww;

    vector<string> vecFWord;
    vector<string> vecBWord;

    
    for(int i=0; i<sSize; ++i)
    {
        vector<string> vecWord;
        vecWord = SplitSingleWord(vtrString[i]);
        int wordNum = vecWord.size();

        //存放前向遍历和后向遍历词

        if(wordNum==1)
        {
            if(IsWords(vecWord[0]))
            {
                vecFww.push_back(vecWord[0]);
                vecBww.push_back(vecWord[0]);
            }
            continue;
        }

        if(wordNum==2)
        {
            int i=0;
            string sWords = vecWord[0]+vecWord[1];
            if(IsWords(sWords))
            {
                vecFww.push_back(sWords);
                vecBww.push_back(sWords);
            }
            else 
            {    
                if(IsWords(vecWord[0]))
                {
                    vecFww.push_back(vecWord[0]);
                    vecBww.push_back(vecWord[0]);
                }
                if(IsWords(vecWord[1]))
                {
                    vecFww.push_back(vecWord[1]);
                    vecBww.push_back(vecWord[1]);
                }
            }
            continue;
        }
        
        vector<string> vecTempBww;
        for(int i=0; i<wordNum;)
        {
            int wordLen = min(wordNum-i,4);
            string sWords = FwwWords(vecWord, wordNum, i, wordLen);
            //cout<<"fww_words:"<<sWords<<endl;
            if(sWords != "")
                vecFww.push_back(sWords);
            else
                vecFWord.push_back(vecWord[i-1]);
        }
        for(int i=wordNum-1; i>=0; )
        {
            int wordLen = min(i+1,4);
            string sWords = BwwWords(vecWord, wordNum, i, wordLen);
            //cout<<sWords<<endl;
            if(sWords != "")
                vecTempBww.push_back(sWords);
            else
                vecBWord.push_back(vecWord[i+1]);
        }
        for(int m=vecTempBww.size()-1;m>=0;m--)
            vecBww.push_back(vecTempBww[m]);
    }

    if(vecFWord.size() < vecBWord.size())
    {
        vecFww.insert(vecFww.end(),vtrProper.begin(),vtrProper.end());
        return vecFww;
    }
    else
    {
        vecBww.insert(vecBww.end(),vtrProper.begin(),vtrProper.end());
        return vecBww; 
    }
}


