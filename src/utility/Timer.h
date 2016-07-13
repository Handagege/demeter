#ifndef _DEMETER_UTILITY_TIMER_H 
#define _DEMETER_UTILITY_TIMER_H 

#include <string>
#include <vector>
using namespace std;
#include <stdint.h>
#include <sys/time.h>



namespace demeter_uti
{

class Timer
{
public: 
	Timer();
	Timer(const Timer& timer);
	virtual ~Timer();

	Timer& operator = (const Timer& timer);

	void Clear(); 
	
	void Start();
	void Stop();
	uint32_t SegCnt(); 

	int32_t GetLast_asUSec();
	float GetLast_asMSec();
	float GetLast_asSec();

	int32_t Get_asUSec(const uint32_t seg);
	float Get_asMSec(const uint32_t seg);
	float Get_asSec(const uint32_t seg);

	int32_t GetTotal_asUSec(); 
	float GetTotal_asMSec(); 
	float GetTotal_asSec(); 

	string AsStringMSec(); 
private: 
	timeval m_laststart;
	bool m_bPingPang;
	vector<pair<uint32_t, uint32_t> > m_vtrSeg;
};


}


#endif /* _DEMETER_UTILITY_TIMER_H */

