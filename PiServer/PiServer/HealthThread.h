#ifndef _HEALTH_THREAD_
#define _HEALTH_THREAD_

#include "Common.h"

class CHealthThread
{
	SOCKET	m_socket;
public:
	CHealthThread();
	~CHealthThread();

	DWORD StartHealthThread(SOCKET socket);
};

unsigned int WINAPI HealthThread(VOID *pParam);

#endif