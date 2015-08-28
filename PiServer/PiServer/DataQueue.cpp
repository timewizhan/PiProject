#include "DataQueue.h"
#include "Log.h"

////////////////////////////////////////////////////////////////////////////////////////
DWORD CDataQueue::PushDataToQueue(DWORD dwPushValue)
{
	::EnterCriticalSection(&m_stCriticalSection);
	m_dequestProto.push_back(dwPushValue);
	::LeaveCriticalSection(&m_stCriticalSection);

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CDataQueue::PopDataToQueue(DWORD &refdwPopValue)
{
	::EnterCriticalSection(&m_stCriticalSection);

	DWORD dwRet = E_RET_SUCCESS;
	if (m_dequestProto.size() != 0) {
		refdwPopValue = m_dequestProto.front();
		m_dequestProto.pop_front();
		dwRet = E_RET_SUCCESS;
	}
	else {
		refdwPopValue = 0;
		dwRet = E_RET_FAIL;
	}
	::LeaveCriticalSection(&m_stCriticalSection);

	return dwRet;
}
