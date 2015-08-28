#ifndef _WEBTHREAD_
#define _WEBTHREAD_

#include "Common.h"

class CWebThread
{
	SOCKET	m_socket;

	DWORD RecvDataFromWebServer(DWORD &refdwValue);
	DWORD ParseData(std::string &refstrMessage);
public:
	CWebThread();
	~CWebThread();

	DWORD StartWebThread(SOCKET socket);
};


#endif