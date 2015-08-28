#ifndef _QUEUE_
#define _QUEUE_

#include "Common.h"

/*
	Concurrently, This Class must be used 
*/

class CDataQueue
{
	CRITICAL_SECTION		m_stCriticalSection;
	std::deque<DWORD>		m_dequestProto;
public:
	DWORD		PushDataToQueue(DWORD dwPushValue);
	DWORD		PopDataToQueue(DWORD &refdwPopValue);
	CDataQueue() {
		::InitializeCriticalSection(&m_stCriticalSection);
	}
	~CDataQueue() {
		::DeleteCriticalSection(&m_stCriticalSection);
	}
	
	static CDataQueue* CreateDataQueue() {
		static CDataQueue		m_pQueue;
		return &m_pQueue;
	}
};


#endif 