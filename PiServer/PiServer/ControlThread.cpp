#include "ControlThread.h"
#include "Log.h"

////////////////////////////////////////////////////////////////////////////////////////
CControlThread::CControlThread()
{

}

////////////////////////////////////////////////////////////////////////////////////////
CControlThread::~CControlThread()
{

}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CControlThread::SendHeader()
{
	ST_PROTO_HEADER stProtoHeader;
	stProtoHeader.eProtoType		= E_PROTO_ACTION_REQ;
	stProtoHeader.dwSizeOfStruct	= sizeof(ST_PROTO_ACTION_REQ);

	int iLen = sizeof(ST_PROTO_HEADER);
	int iSent = 0;
	int iTotal = 0;
	while (1)
	{
		iSent = ::send(m_socket, (char *)&stProtoHeader, iLen - iSent, 0);
		iTotal += iSent;
		if (iTotal != iLen) {
			continue;
		}
		break;
	}
	
	iTotal = 0;
	/*
		C Language not use "bool" so substitude bool for int
		bool size is 1
		int size is 4
		As a result, use plus 3
	*/
	/*
		iLen = sizeof(ST_PROTO_HEADER_RES) + 3;
		int iRecv = 0;
		char *szBuf = new char[iLen];
		while (1)
		{
			iRecv = ::recv(m_socket, szBuf, iLen - iRecv, 0);
			iTotal += iRecv;
			if (iTotal != iLen) {
				szBuf += iRecv;
				continue;
			}
			break;
		}

		delete[] szBuf;
	*/
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CControlThread::SendControlData()
{
	ST_PROTO_ACTION_REQ stProtoActionReq;
	::memset(&stProtoActionReq, 0x00, sizeof(ST_PROTO_ACTION_REQ));

	DWORD dwRet;
	dwRet = FillPacket(stProtoActionReq);
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Fail to fill Data");
		return E_RET_FAIL;
	}

	int iLen = sizeof(ST_PROTO_ACTION_REQ);
	int iSent = 0;
	int iTotal = 0;
	while (1)
	{
		iSent = ::send(m_socket, (char *)&stProtoActionReq, iLen, 0);
		iTotal += iSent;
		if (iTotal != iLen) {
			continue;
		}
		break;
	}

	iTotal = 0;
	/*
		C Language not use "bool" so substitude bool for int
		bool size is 1
		int size is 4
		As a result, use plus 3
	*/
	iLen = sizeof(ST_PROTO_ACTION_RES) + 3;
	int iRecv = 0;
	char *szBuf = new char[iLen];
	while (1)
	{
		iRecv = ::recv(m_socket, szBuf, iLen - iRecv, 0);
		iTotal += iRecv;
		if (iTotal != iLen) {
			szBuf += iRecv;
			continue;
		}
		break;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CControlThread::FillPacket(ST_PROTO_ACTION_REQ &refstProtoActionReq)
{
	DWORD dwValue;
	DWORD dwRet = 0;

	while (1) 
	{ 
		dwRet = CDataQueue::CreateDataQueue()->PopDataToQueue(dwValue);
		if (dwRet != E_RET_SUCCESS) {
			WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_CONTROL_PORT] Not Queue");
			::Sleep(2000);
			continue;
		}
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_CONTROL_PORT] Queue Data [%d]", dwValue);
		break;
	}
	
	switch (dwValue)
	{
		case E_PROTO_CONTROL_UP:
			refstProtoActionReq.eProtoControl = E_PROTO_CONTROL_UP;
			break;
		case E_PROTO_CONTROL_DOWN:
			refstProtoActionReq.eProtoControl = E_PROTO_CONTROL_DOWN;
			break;
		case E_PROTO_CONTROL_LEFT:
			refstProtoActionReq.eProtoControl = E_PROTO_CONTROL_LEFT;
			break;
		case E_PROTO_CONTROL_RIGHT:
			refstProtoActionReq.eProtoControl = E_PROTO_CONTROL_RIGHT;
			break;
		default:
			WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Unknown data queue");
			return E_RET_FAIL;
	}
	return E_RET_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////
DWORD CControlThread::CommunicateWithClient(SOCKET socket)
{
	m_socket = socket;
	DWORD dwRet;
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_CONTROL_PORT] Start to send Header");
	dwRet = SendHeader();
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Fail to send header");
		return E_RET_FAIL;
	}

	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_CONTROL_PORT] Start to send Control Data");
	dwRet = SendControlData();
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Fail to send control data");
		return E_RET_FAIL;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CControlThread::StartControlThread(SOCKET socket)
{
	m_stControlThreadArg.socket = socket;
	m_stControlThreadArg.pstControlThreadArg = this;
	HANDLE hThread = NULL;
	hThread = (HANDLE)_beginthreadex(NULL, 0, ControlThread, (VOID *)&m_stControlThreadArg, 0, NULL);
	if (hThread == NULL) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Fail to create thread");
		return E_RET_FAIL;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI ControlThread(VOID *pParam)
{
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_CONTROL_PORT] Start Control Thread");
	ST_CONTROL_THREAD_ARG *pstControlThreadArg = NULL;
	pstControlThreadArg = (ST_CONTROL_THREAD_ARG *)pParam;

	CControlThread *pControlThread = (CControlThread *)pstControlThreadArg->pstControlThreadArg;
	DWORD dwRet;
	BOOL bContinued = TRUE;
	while (bContinued)
	{
		dwRet = pControlThread->CommunicateWithClient(pstControlThreadArg->socket);
		if (dwRet != E_RET_SUCCESS) {
			WriteLog(E_LOG_LEVEL_ERROR, "[Thread_CONTROL_PORT] Fail to commnuicate with client");
			continue;
		}

		::Sleep(1000);
	}
	
	return 0;
}