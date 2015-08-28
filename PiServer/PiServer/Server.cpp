#include "Server.h"
#include "Log.h"
#include "WebThread.h"
#include "ControlThread.h"
#include "HealthThread.h"

////////////////////////////////////////////////////////////////////////////////////////
CServer::CServer()
{
	::memset(&m_stServerSock, 0x00, sizeof(ST_SERVER_SOCK));
}

////////////////////////////////////////////////////////////////////////////////////////
CServer::~CServer()
{
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CServer::InitThread()
{
	DWORD i;
	DWORD dwPort = CLIENT_CONTROL_PORT;
	for (i = 0; i < MAX_OPEN_PORT; i++)
	{
		m_stMainThreadArg.dwPort = dwPort;
		m_stMainThreadArg.Server = (VOID *)this;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (VOID *)&m_stMainThreadArg, 0, NULL);
		m_pTArrayHandle[i] = hThread;
		dwPort++;
		::Sleep(1000);
	}

	if (i != MAX_OPEN_PORT) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Main] Fail to create Thread [%d]", i);
		return E_RET_FAIL;
	}
	return E_RET_SUCCESS;
}

DWORD CServer::SetSocket(SOCKET socket)
{
	BOOL bOptVal = TRUE;
	int iOptVal = 1;
	int iResult;

	iResult = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&iOptVal, sizeof(int));
	if (iResult == SOCKET_ERROR) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to setsockopt [%d]", WSAGetLastError());
		return E_RET_FAIL;
	}

	struct timeval  stTimeVal;
	stTimeVal.tv_sec = 5;
	stTimeVal.tv_usec = 0;

	iResult = ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&stTimeVal, sizeof(stTimeVal));
	if (iResult == SOCKET_ERROR) {
		WriteLog(E_LOG_LEVEL_ERROR, "Fail to setsockopt [%d]", WSAGetLastError());
		return E_RET_FAIL;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CServer::InitServer(DWORD dwPort, DWORD dwBackLog)
{
	int iRet;
	try {
		iRet = WSAStartup(MAKEWORD(2, 2), &m_stServerSock.wsaData);
		if (iRet != 0)
			throw std::exception("Fail to WSAStartup");

		//iRet = (m_stServerSock.socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED));
		iRet = (m_stServerSock.socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0));
		if (iRet == INVALID_SOCKET)
			throw std::exception("Fail to make socket");
		
		iRet = SetSocket(m_stServerSock.socket);
		if (iRet != E_RET_SUCCESS)
			throw std::exception("Fail to set socket");

		iRet = m_cHelpTool.GetAddressInfo(m_stServerSock.vecstrAddress);
		if (iRet != E_RET_SUCCESS)
			throw std::exception("Fail to get address info");

		int iSize = sizeof(SOCKADDR);
		std::string strAddress = m_stServerSock.vecstrAddress[0].c_str();
		m_stServerSock.stSockAddr_in.sin_family	= AF_INET;
		m_stServerSock.stSockAddr_in.sin_port	= ::htons((unsigned short)dwPort);
		inet_pton(AF_INET, strAddress.c_str(), &m_stServerSock.stSockAddr_in.sin_addr.s_addr);

		iRet = ::bind(m_stServerSock.socket, (SOCKADDR *)&m_stServerSock.stSockAddr_in, sizeof(m_stServerSock.stSockAddr_in));
		if (iRet == SOCKET_ERROR) 
			throw std::exception("Fail to bind [%d]", WSAGetLastError());

		iRet = ::listen(m_stServerSock.socket, dwBackLog);
		if (iRet == SOCKET_ERROR)
			throw std::exception("Fail to listen");
	}
	catch (std::exception &e) {
		std::string straaa = e.what();
		WriteLog(E_LOG_LEVEL_ERROR, e.what());
		if (m_stServerSock.socket != 0)
			closesocket(m_stServerSock.socket);
		
		WSACleanup();
		return E_RET_FAIL;
	}
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CServer::AcceptServer(DWORD dwPort)
{
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Start to Accept for client connection");
	int iLength = sizeof(SOCKADDR);
	ST_CLIENT_SOCK stClientSock;

	BOOL bContinued = TRUE;
	DWORD dwRet;

	CWebThread WebThread;
	CControlThread ControlThread;
	CHealthThread HealthThread;
	while (bContinued)
	{
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Wait for Client Connection [%d]", dwPort);
		stClientSock.socket = ::accept(m_stServerSock.socket, (SOCKADDR *)&stClientSock.stSockAddr_in, &iLength);
		if (stClientSock.socket == INVALID_SOCKET) {
			WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to get Client socket");
			continue;
		}
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Get Client Connection [%d]", dwPort);

		SOCKET ClientSock = stClientSock.socket;
		switch (dwPort)
		{
			case CLIENT_CONTROL_PORT:
				WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Effective Port [CLIENT_CONTROL_PORT]");
				dwRet = ControlThread.StartControlThread(ClientSock);
				if (dwRet != E_RET_SUCCESS) {
					WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to Start Control Thread");
					continue;
				}
				break;
			case CLIENT_HEALTH_PORT:
				WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Effective Port [CLIENT_HEALTH_PORT]");
				dwRet = HealthThread.StartHealthThread(ClientSock);
				if (dwRet != E_RET_SUCCESS) {
					WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to Start Health Thread");
					continue;
				}
				break;
			case WEBSERVER_PORT:
				WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Effective Port [WEBSERVER_PORT]");
				dwRet = WebThread.StartWebThread(ClientSock);
				if (dwRet != E_RET_SUCCESS) {
					WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to Start Web Thread");
					continue;
				}
				break;
			default:
				WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Invalid Socket Port [%d]", dwPort);
				bContinued = FALSE;
				break;
		}
	}
	
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CServer::StartServer()
{
	BOOL bContinued = TRUE;
	BOOL bThread = TRUE;
	DWORD dwRet;
	while (bContinued)
	{
		if (bThread) {
			WriteLog(E_LOG_LEVEL_DEBUG, "[Main] Ready to create Thread and start Server");
			dwRet = InitThread();
			if (dwRet == E_RET_FAIL) {
				WriteLog(E_LOG_LEVEL_ERROR, "[Main] Fail to init Thread");
				::Sleep(1000);
				continue;
			}
			bThread = FALSE;
		}
		/*
			Wait for initializing thread during 1 second
		*/
		::Sleep(1000);
		WriteLog(E_LOG_LEVEL_DEBUG, "[Main] Start Server operation");

		dwRet = WaitForMultipleObjects(MAX_OPEN_PORT, m_pTArrayHandle, TRUE, INFINITE);
		if (dwRet == WAIT_FAILED) {
			bContinued = FALSE;
			WriteLog(E_LOG_LEVEL_ERROR, "[Main] Fail to wait for main threads [%d]", GetLastError());
		}
		else if (dwRet == WAIT_TIMEOUT) {
			WriteLog(E_LOG_LEVEL_ERROR, "[Main] Fail for timeout");
		}
	}
	
	WriteLog(E_LOG_LEVEL_DEBUG, "[Main] End Server");
	return E_RET_SUCCESS;
}

/*
	From below, Thread;
*/

////////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI MainThread(VOID *pParam)
{
	ST_MAIN_THREAD_ARG *stMainThreadArg = NULL;
	stMainThreadArg = (ST_MAIN_THREAD_ARG *)pParam;
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Start Main Thread [%d]", stMainThreadArg->dwPort);

	BOOL bContinued = TRUE;
	CServer *pServer = NULL;
	pServer = (CServer *)stMainThreadArg->Server;
	if (pServer == NULL) {
		bContinued = FALSE;
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to get arguement of server");
	}

	DWORD dwRet;
	BOOL bInit = TRUE;
	while (bContinued)
	{
		if (bInit) {
			dwRet = pServer->InitServer(stMainThreadArg->dwPort);
			if (dwRet != E_RET_SUCCESS) {
				bContinued = FALSE;
				WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to initialize server");
				break;
			}
			bInit = FALSE;
		}

		dwRet = pServer->AcceptServer(stMainThreadArg->dwPort);
		if (dwRet != E_RET_SUCCESS) {
			WriteLog(E_LOG_LEVEL_ERROR, "[Thread] Fail to start server");
			continue;
		}

		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] Wait for 1 second");
		::Sleep(1000);
	}

	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread] End Thread");
	return 0;
}
