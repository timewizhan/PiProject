#ifndef _CONTROL_THREAD_
#define _CONTROL_THREAD_

#include "Common.h"

struct ST_CONTROL_THREAD_ARG
{
	SOCKET socket;
	void *pstControlThreadArg;
};

class CControlThread
{
	ST_CONTROL_THREAD_ARG	m_stControlThreadArg;
	SOCKET					m_socket;

	DWORD SendHeader();
	DWORD SendControlData();
	DWORD FillPacket(ST_PROTO_ACTION_REQ &refstProtoActionReq);
public:
	CControlThread();
	~CControlThread();

	DWORD StartControlThread(SOCKET socket);
	DWORD CommunicateWithClient(SOCKET socket);
};

unsigned int WINAPI ControlThread(VOID *pParam);

#endif