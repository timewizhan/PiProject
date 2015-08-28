#include "HealthThread.h"
#include "Log.h"
#include "SocketError.h"

CHealthThread::CHealthThread()
{
}
CHealthThread::~CHealthThread()
{
}

DWORD CHealthThread::StartHealthThread(SOCKET socket)
{
	m_socket = socket;
	HANDLE hThread = NULL;
	hThread = (HANDLE)_beginthreadex(NULL, 0, HealthThread, (VOID *)&m_socket, 0, NULL);
	if (hThread == NULL) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_HEALTH_PORT] Fail to create thread");
		return E_RET_FAIL;
	}

	return E_RET_SUCCESS;
}

unsigned int WINAPI HealthThread(VOID *pParam)
{
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_HEALTH_PORT] Start Health Thread");
	SOCKET ClientSock = *((SOCKET *)pParam);

	/*
		C Language not use "bool" so substitude bool for int
		bool size is 1
		int size is 4
		As a result, use plus 3
	*/
	int iLen = sizeof(ST_PROTO_HEALTH_REQ) + 3;
	int iRecv = 0;
	int iTotal = 0;
	char *pRecvBuf = NULL;

	BOOL bContinued = TRUE;
	while (bContinued)
	{
		if (pRecvBuf == NULL) {
			pRecvBuf = new char[iLen];
			::memset(pRecvBuf, 0x00, iLen);
		}
		while (1)
		{
			if (pRecvBuf == NULL) {
				WriteLog(E_LOG_LEVEL_ERROR, "[Thread_HEALTH_PORT] Fail to allocate virtual memory");
				Sleep(1000);
				continue;
			}
		
			WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_HEALTH_PORT] Start to receive Health Data from client");
			iRecv = ::recv(ClientSock, pRecvBuf, iLen - iRecv, 0);
			DWORD dwError = WSAGetLastError();
			DWORD dwRet;
			dwRet = CheckSocketError(dwError, E_THREAD_HEALTH);
			if (dwRet != E_RET_SUCCESS) {
				bContinued = false;
				break;
			}

			iTotal += iRecv;
			if (iTotal != iLen) {
				WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_HEALTH_PORT] Not yet to receive Total Data [%d]", iRecv);
				pRecvBuf += iRecv;
				continue;
			}

			WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_HEALTH_PORT] Succeed to receive Health Data from client");
			::memset(pRecvBuf, 0x00, iLen);
			iRecv = iTotal = 0;
		}
	}
	
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_HEALTH_PORT] Succeed to close health thread");
	closesocket(ClientSock);

	delete[] pRecvBuf;
	return 0;
}