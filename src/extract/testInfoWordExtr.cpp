#include <iostream>
#include "InfoWordExtr.h"
#include "stdio.h"

using namespace std;
using namespace demeter_extract;


void testSingleText(InfoWordExtr &ins)
{
        string text = "#微博时评团# 英国脱欧公投通过，首相卡梅隆宣布辞职！英国为何急于脱离欧盟？英镑暴跌日元暴涨、多国股市哀鸿遍野，全球市场为何反应如此剧烈？对中国外交/经济有何影响？留学、海淘…它会如何影响你的生活？微博时评团专家全方位解读↓↓ 点击进入O英国脱欧解读专题";
        printf("INPUT TEXT : \n%s\n",text.c_str()); 
        vector<string> vtrInfoWords;
        ins.Extract(vtrInfoWords,text.c_str());
        printf("OUPUT WORDS : "); 
        for(int i=0; i<vtrInfoWords.size(); ++i){
                cout << vtrInfoWords[i] << " ";
        }
        cout << endl;
}


int main(int argc, char* argv[])
{
        const char* sResFile = "../../resource/demeter_resource_20160621/infoword_vocabulary.txt";
        const char* sTradSimpFile = "../../resource/demeter_resource_20160621/tradition_simple_character.txt";
        InfoWordExtr ins;
        ins.LoadResource(sResFile,sTradSimpFile);
        printf("load resource %s \nand %s success",sResFile,sTradSimpFile); 
        testSingleText(ins);
}
