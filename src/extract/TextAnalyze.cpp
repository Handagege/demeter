#include "TextAnalyze.h"
using namespace demeter_extract; 
#include "Config.h"
using namespace demeter_uti; 
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////////////////////
// class TextContent 

TextContent::TextContent() : m_nTopN_Keyword(0), m_nTopN_Extend(0), m_nTopN_Topic(0) 
{
}


TextContent::TextContent(const TextContent& textCont)
{
	m_nTopN_Keyword = textCont.m_nTopN_Keyword; 
	m_nTopN_Extend = textCont.m_nTopN_Extend; 	
	m_nTopN_Topic = textCont.m_nTopN_Topic; 	
	
	m_vtrInfoWords = textCont.m_vtrInfoWords; 
	m_vtrKeyWords= textCont.m_vtrKeyWords; 
	m_vtrExtendWords = textCont.m_vtrExtendWords;
	m_vtrTopics = textCont.m_vtrTopics;  
	m_timer = textCont.m_timer; 
}


TextContent::~TextContent()
{
}


TextContent& TextContent::operator = (const TextContent& textCont)
{
	m_nTopN_Keyword = textCont.m_nTopN_Keyword; 
	m_nTopN_Extend = textCont.m_nTopN_Extend; 	
	m_nTopN_Topic = textCont.m_nTopN_Topic; 	
	
	m_vtrInfoWords = textCont.m_vtrInfoWords; 
	m_vtrKeyWords= textCont.m_vtrKeyWords; 
	m_vtrExtendWords = textCont.m_vtrExtendWords;
	m_vtrTopics = textCont.m_vtrTopics;  
	m_timer = textCont.m_timer; 
	return *this; 
}


void TextContent::Clear()
{
	m_vtrInfoWords.clear(); 
	m_vtrKeyWords.clear(); 
	m_vtrExtendWords.clear(); 
	m_vtrTopics.clear(); 
	m_timer.Clear(); 
}


string TextContent::TimeCostAsString()
{
	return m_timer.AsStringMSec(); 
}


/////////////////////////////////////////////////////////////////////////////////////////////
// class TextAnalyze


TextAnalyze::TextAnalyze() : m_pInfoWordExtr(NULL), m_pKeyWordExtr(NULL), m_pExtendWordExtr(NULL), m_pTopicReco(NULL) 
{
}


TextAnalyze::~TextAnalyze()
{
	Release(); 
}


void TextAnalyze::Release()
{
	if(m_pInfoWordExtr)
	{
		delete m_pInfoWordExtr;
		m_pInfoWordExtr = NULL;
	}
	if(m_pKeyWordExtr)
	{
		delete m_pKeyWordExtr;
		m_pKeyWordExtr = NULL;
	}
	if(m_pExtendWordExtr)
	{
		delete m_pExtendWordExtr;
		m_pExtendWordExtr = NULL;
	}
	if(m_pTopicReco)
	{
		delete m_pTopicReco;
		m_pTopicReco = NULL;
	}
}


bool TextAnalyze::LoadResource(const char* sResPath)
{
	if(!sResPath)
		return false; 
	char sfile[1024], sfile2[1024];

	// read config file
	sprintf(sfile, "%s/demeter.conf", sResPath); 
	Config conf; 
	if(!conf.Read(sfile))
		return false; 

	// load info-words resource
	if(!m_pInfoWordExtr)
		m_pInfoWordExtr = new InfoWordExtr(); 
	sprintf(sfile, "%s/%s", sResPath, conf.GetVal_asString("INFOWORD_VOCAB").c_str()); 
	sprintf(sfile2, "%s/%s", sResPath, conf.GetVal_asString("INFOWORD_TRAD_SAMPLE_MAPING").c_str()); 
	if(!m_pInfoWordExtr->LoadResource(sfile, sfile2))
	{
		delete m_pInfoWordExtr; 
		m_pInfoWordExtr = NULL; 
		return false; 
	}	

	// load keywords(idf) resource
	if(!m_pKeyWordExtr)
		m_pKeyWordExtr = new KeyWordExtr; 
	sprintf(sfile, "%s/%s", sResPath, conf.GetVal_asString("TERM_IDF").c_str()); 
	if(!m_pKeyWordExtr->LoadResource(sfile))
	{
		delete m_pKeyWordExtr; 
		m_pKeyWordExtr = NULL; 
	}

	// load extend-words resource
	if(!m_pExtendWordExtr)
		m_pExtendWordExtr = new ExtendWordExtr(); 
	sprintf(sfile, "%s/%s", sResPath, conf.GetVal_asString("EXTEND_MAPPING").c_str()); 
	if(!m_pExtendWordExtr->LoadResource(sfile))
	{
		delete m_pExtendWordExtr; 
		m_pExtendWordExtr = NULL; 
	}	

	// load topics resource
	if(!m_pTopicReco)
		m_pTopicReco = new TopicReco(); 
	sprintf(sfile, "%s/%s", sResPath, conf.GetVal_asString("TOPIC_REVERSED_INDEX").c_str()); 
	if(!m_pTopicReco->LoadResource(sfile))
	{
		delete m_pTopicReco; 
		m_pTopicReco = NULL; 
	}

	return true; 
}


int32_t TextAnalyze::Analyze(TextContent& textCont, const char* sText, const int32_t nOptions)
{
	textCont.Clear(); 
	if(!sText)
		return -1; 

	// extract info-words from the text
	textCont.m_timer.Start();
	if(!m_pInfoWordExtr)
		return -1; 
	m_pInfoWordExtr->Extract(textCont.m_vtrInfoWords, sText); 
        //cout << "info words extract finish ..." << endl;
        //for(int i=0; i<textCont.m_vtrInfoWords.size(); ++i)
        //        cout << textCont.m_vtrInfoWords[i] << endl;
	textCont.m_timer.Stop(); 	

	if(nOptions & _TEXT_ANALYZE_OPTS_KEYWORD)
	{
		int32_t ret = _TEXT_ANALYZE_OPTS_INFOWORD; 
		// extract keywords from the text
		textCont.m_timer.Start(); 
		if(!m_pKeyWordExtr)
			return -1; 
		m_pKeyWordExtr->Extract(textCont.m_vtrKeyWords, textCont.m_vtrInfoWords, textCont.m_nTopN_Keyword); 
			ret |= _TEXT_ANALYZE_OPTS_KEYWORD; 	
		textCont.m_timer.Stop(); 

		// extract extend-words according to info-words
		textCont.m_timer.Start(); 
		if(nOptions & _TEXT_ANALYZE_OPTS_EXTENDWORD)
		{
			if(m_pExtendWordExtr)
			{
				m_pExtendWordExtr->Extract(textCont.m_vtrExtendWords, textCont.m_vtrKeyWords, textCont.m_nTopN_Extend); 
				ret |= _TEXT_ANALYZE_OPTS_EXTENDWORD; 	
			}
		}
		textCont.m_timer.Stop(); 
		
		// extract topics according to info-words
		textCont.m_timer.Start(); 
		if(nOptions & _TEXT_ANALYZE_OPTS_TOPIC)
		{
			if(m_pTopicReco)
			{
				m_pTopicReco->Recognize(textCont.m_vtrTopics, textCont.m_vtrKeyWords, textCont.m_nTopN_Topic); 
				ret |= _TEXT_ANALYZE_OPTS_TOPIC; 	
			}
		}
		textCont.m_timer.Stop(); 
	
		return ret; 
	}

	return -1; 
}



