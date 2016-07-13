#ifndef _DEMETER_EXTRACT_TEXT_ANALYZE_H 
#define _DEMETER_EXTRACT_TEXT_ANALYZE_H 

#include "Timer.h"
using namespace demeter_uti; 
#include "WsPair.h"
#include "InfoWordExtr.h"
#include "KeyWordExtr.h"
#include "ExtendWordExtr.h"
#include "TopicReco.h"

#define _TEXT_ANALYZE_OPTS_INFOWORD		0x00
#define _TEXT_ANALYZE_OPTS_KEYWORD		0x01
#define _TEXT_ANALYZE_OPTS_EXTENDWORD	0x02
#define _TEXT_ANALYZE_OPTS_TOPIC		0x04
#define _TEXT_ANALYZE_OPTS_ALL			0xEFFFFFFF


namespace demeter_extract
{


// TextContent
// 文本内容，存储文本分析结果
class TextContent
{
public: 
	friend class TextAnalyze;	
	
	TextContent(); 
	TextContent(const TextContent& textCont); 
	virtual ~TextContent();

	TextContent& operator = (const TextContent& textCont); 

	void Clear(); 

	string TimeCostAsString(); 

public:
	int32_t m_nTopN_Keyword; 	
	int32_t m_nTopN_Extend; 	
	int32_t m_nTopN_Topic; 	
	
	vector<string> m_vtrInfoWords;				// info words of the text 
	vector<WsPair> m_vtrKeyWords;				// keywords of the text
	vector<WsPair> m_vtrExtendWords;			// extend words of the text
	vector<pair<int32_t, float> > m_vtrTopics; // hit topics of the text

private:
	Timer m_timer;	
}; 


// TextAnalyze
// 文本分析器
class TextAnalyze
{
public: 
	TextAnalyze(); 
	virtual ~TextAnalyze(); 

	void Release(); 
	bool LoadResource(const char* sResPath); 
	
	int32_t Analyze(TextContent& textCont, const char* sText, const int32_t nOptions = _TEXT_ANALYZE_OPTS_ALL); 

private: 
	InfoWordExtr* m_pInfoWordExtr; 
	KeyWordExtr* m_pKeyWordExtr; 
	ExtendWordExtr* m_pExtendWordExtr; 
	TopicReco* m_pTopicReco; 
}; 

}


#endif /* _DEMETER_EXTRACT_TEXT_ANALYZE_H */ 

