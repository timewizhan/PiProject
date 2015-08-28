#include "Client.h"
#include "Log.h"

BOOL g_bThreadExit = TRUE;

////////////////////////////////////////////////////////////////////////////////////////
CClient::CClient()
{
}

////////////////////////////////////////////////////////////////////////////////////////
CClient::~CClient()
{
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CClient::MakeThread()
{
	/*
		Thread have to make only 2 
		1 Thread : Action Req/Res
		2 Thread : Client Health Check
	*/
	
	m_hArray[0] = (HANDLE)_beginthreadex(NULL, 0, WorkerActionThread, (void *)&m_stClientSock, 0, NULL);
	m_hArray[1] = (HANDLE)_beginthreadex(NULL, 0, WorkerHealthThread, (void *)&m_stClientSock, 0, NULL);

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CClient::InitClient(std::string strAddress, DWORD dwPort)
{
	DWORD dwRet;
	try
	{
		dwRet = WSAStartup(MAKEWORD(2, 2), &m_stClientSock.wsaData);
		if (dwRet != 0)
			throw std::exception("Fail to WSAStartup");

		dwRet = m_stClientSock.sockServer = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (dwRet == INVALID_SOCKET)
			throw std::exception("Fail to make socket");

		m_stClientSock.stSockAddr_in.sin_family			= AF_INET;
		m_stClientSock.stSockAddr_in.sin_port			= ::htons((unsigned short)dwPort);
		inet_pton(AF_INET, strAddress.c_str(), &m_stClientSock.stSockAddr_in.sin_addr.s_addr);
	}
	catch (std::exception &e)
	{
		WriteLog(E_LOG_LEVEL_ERROR, e.what());
		return E_RET_FAIL;
	}
	return E_RET_SUCCESS;
}

DWORD CClient::ConnectServer()
{
	/*
		if count has value, it was failed past
		so, wait for count * 2
	*/
	DWORD dwCount = 0;
	while (1)
	{
		if (dwCount != 0) {
			// 1000 is one second
			DWORD dwSleepTime = 1000;
			::Sleep(dwSleepTime * dwCount * 2);
		}

		int iRet;
		iRet = ::connect(m_stClientSock.sockServer, (SOCKADDR *)&m_stClientSock.stSockAddr_in, sizeof(m_stClientSock.stSockAddr_in));
		if (iRet == SOCKET_ERROR) {
			WriteLog(E_LOG_LEVEL_DEBUG, "Fail to connect to Server [%d]", dwCount);
			dwCount++;
			continue;
		}
		break;
	}
	
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CClient::StartClient()
{
	DWORD dwRet;
	DWORD dwCount = 0;
		
	while (1)
	{
		dwRet = ConnectServer();
		if (dwRet != E_RET_SUCCESS) {
			WriteLog(E_LOG_LEVEL_ERROR, "Fail to connect to Server");
		}

		dwRet = MakeThread();
		if (dwRet != E_RET_SUCCESS) {
			WriteLog(E_LOG_LEVEL_ERROR, "Fail to make thread");
		}

		WaitForMultipleObjects(MAX_THREAD, m_hArray, TRUE, INFINITE);
		
		/*
			normally, code have to be not executed
			if it was done, first or second thread is exited
		*/
		g_bThreadExit = FALSE;

		/*
			Wait for 1 second to exit all thread ordinarily	
		*/
		::Sleep(1000);
		int i;
		for (i = 0; i < MAX_THREAD; i++)
			m_hArray[i] = NULL;

		g_bThreadExit = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI WorkerActionThread(VOID *pParam)
{
	ST_CLIENT_SOCK *pstClientSock = NULL;
	pstClientSock = (ST_CLIENT_SOCK *)pParam;
	SOCKET sockServer;
	sockServer = pstClientSock->sockServer;
	DWORD dwRet;
	WriteLog(E_LOG_LEVEL_DEBUG, "Start Worker Action Thread");
	while (g_bThreadExit)
	{
		::Sleep(1000);
		///*
		//	First, send Header
		//*/
		//WriteLog(E_LOG_LEVEL_DEBUG, "Recv Action Data");
		//dwRet = RecvActionData(sockServer);
		//if (dwRet != E_RET_SUCCESS) {
		//	continue;
		//}

		///*
		//	Second, send Body
		//*/
		//WriteLog(E_LOG_LEVEL_DEBUG, "Send Action Data for ack");
		//dwRet = SendActionData(sockServer);
		//if (dwRet != E_RET_SUCCESS) {
		//	continue;
		//}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI WorkerHealthThread(VOID *pParam)
{
	ST_CLIENT_SOCK *pstClientSock = NULL;
	pstClientSock = (ST_CLIENT_SOCK *)pParam;
	SOCKET sockServer;
	sockServer = pstClientSock->sockServer;
	DWORD dwRet;
	WriteLog(E_LOG_LEVEL_DEBUG, "Start Worker Health Thread");
	while (g_bThreadExit)
	{
		WriteLog(E_LOG_LEVEL_DEBUG, "One cycle start, continue");
		/*
			each 1 minute, send HealthData to Server
		*/

		::Sleep(SLEEP_TIME);

		/*
			First, send Header
		*/
		dwRet = SendHeader(sockServer);
		if (dwRet != E_RET_SUCCESS) {
			continue;
		}

		/*
			Second, send Health Data
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "Send Health Data to Server");
		dwRet = SendHealthData(sockServer);
		if (dwRet != E_RET_SUCCESS) {
			continue;
		}

		/*
			Third, receive Health Data
			not yet!!
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "Recv Health Data from server");
		dwRet = RecvHealthData(sockServer);
		if (dwRet != E_RET_SUCCESS) {
			continue;
		}

		WriteLog(E_LOG_LEVEL_DEBUG, "One cycle end, continue");
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
int SendHeader(SOCKET sock)
{
	ST_PROTO_HEADER		stProtoHeader;
	int iTotalData = sizeof(ST_PROTO_HEADER);
	int iSent;

	stProtoHeader.eProtoType = E_PROTO_HEALTH_REQ;
	stProtoHeader.dwSizeOfStruct = sizeof(ST_PROTO_HEALTH_REQ);
	iSent = ::send(sock, (char *)&iSent, sizeof(int), 0);
	iSent = ::send(sock, (char *)&stProtoHeader, iTotalData, 0);
	if (iSent != iTotalData) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to send Header Data [%d/%d] [%d]", iSent, iTotalData, WSAGetLastError());
		return E_RET_SEND_FAIL;
	}

	iTotalData = sizeof(ST_PROTO_HEADER_RES);
	char *pRecvBuf = new char[iTotalData];
	int iRecv;
	iRecv = ::recv(sock, pRecvBuf, (int)iTotalData, 0);
	if (iRecv != iTotalData) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to receive Header Data [%d/%d] [%d]", iRecv, iTotalData, WSAGetLastError());
		delete[] pRecvBuf;
		return E_RET_SEND_FAIL;
	}

	delete[] pRecvBuf;
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
int SendHealthData(SOCKET sock)
{
	ST_PROTO_HEALTH_REQ	stProtoHealthReq;

	int iTotalData;
	int iSent;
	iTotalData = sizeof(ST_PROTO_HEALTH_REQ);
	iSent = ::send(sock, (char *)&stProtoHealthReq, iTotalData, 0);
	if (iSent != iTotalData) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to send Health Data [%d/%d] [%d]", iSent, iTotalData, WSAGetLastError());
		return E_RET_SEND_FAIL;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
int RecvHealthData(SOCKET sock)
{
	char *pRecvBuf = NULL;
	
	int iTotalData = sizeof(ST_PROTO_HEALTH_RES);
	pRecvBuf = new char[sizeof(char) * iTotalData];

	int iRecv = 0;
	while (1)
	{
		int iCurrentRecv = 0;
		iCurrentRecv = ::recv(sock, (char *)pRecvBuf, iTotalData, 0);

		iRecv += iCurrentRecv;
		if (iRecv != iTotalData) {
			WriteLog(E_LOG_LEVEL_ERROR, "Fail to receive Health Data [%d/%d]", iRecv, iTotalData);
			pRecvBuf = pRecvBuf + iRecv;
			continue;
		}
		break;
	}

	delete[] pRecvBuf;

	return E_RET_SUCCESS;
}

int SendActionData(SOCKET sock)
{
	ST_PROTO_HEADER		stProtoHeader;
	ST_PROTO_ACTION_RES	stProtoActionRes;
	stProtoHeader.eProtoType = E_PROTO_HEALTH_REQ;
	stProtoHeader.dwSizeOfStruct = sizeof(ST_PROTO_HEALTH_REQ);

	int iTotalData = sizeof(ST_PROTO_HEADER);
	int iSent;
	iSent = ::send(sock, (char *)&stProtoHeader, iTotalData, 0);
	if (iSent != iTotalData) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to send Header Data [%d/%d]", iSent, iTotalData);
		return E_RET_SEND_FAIL;
	}

	iTotalData = sizeof(ST_PROTO_HEALTH_REQ);
	iSent = ::send(sock, (char *)&stProtoActionRes, iTotalData, 0);
	if (iSent != iTotalData) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to send Action Data [%d/%d]", iSent, iTotalData);
		return E_RET_SEND_FAIL;
	}

	return E_RET_SUCCESS;
}
int RecvActionData(SOCKET sock)
{
	char *pRecvBuf = NULL;

	int iTotalData = sizeof(ST_PROTO_ACTION_REQ);
	pRecvBuf = new char[sizeof(char) * iTotalData];

	int iRecv = 0;
	while (1)
	{
		int iCurrentRecv = 0;
		iCurrentRecv = ::recv(sock, (char *)pRecvBuf, iTotalData, 0);
		if (iCurrentRecv == -1) {
			printf("[%d]\n", WSAGetLastError());
			::Sleep(1000);
		}


		iRecv += iCurrentRecv;
		if (iRecv != iTotalData) {
			WriteLog(E_LOG_LEVEL_ERROR, "Fail to receive Action Data [%d/%d]", iRecv, iTotalData);
			pRecvBuf = pRecvBuf + iRecv;
			continue;
		}
		break;
	}

	/*
		Save action to internal queue
	*/

	delete[] pRecvBuf;

	return E_RET_SUCCESS;
}