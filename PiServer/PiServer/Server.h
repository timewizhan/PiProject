#ifndef _SERVER_
#define _SERVER_

#include "Common.h"
#include "HelpTool.h"
#include "WebThread.h"

#define MAX_OPEN_PORT		3

enum E_SERVER_PORT
{
	CLIENT_CONTROL_PORT	= 50513,
	CLIENT_HEALTH_PORT  = 50514,
	WEBSERVER_PORT		= 50515
};

struct ST_SERVER_SOCK
{
	WSADATA wsaData;
	SOCKADDR_IN stSockAddr_in;
	SOCKET socket;
	DWORD dwPort;
	std::vector<std::string> vecstrAddress;

	ST_SERVER_SOCK() : dwPort(0) {}
};

struct ST_CLIENT_SOCK
{
	SOCKET socket;
	SOCKADDR_IN stSockAddr_in;
};

struct ST_MAIN_THREAD_ARG
{
	DWORD	dwPort;
	VOID	*Server;

	ST_MAIN_THREAD_ARG() : dwPort(0), Server(NULL) {}
};

class CServer
{
	ST_MAIN_THREAD_ARG		m_stMainThreadArg;
	ST_SERVER_SOCK			m_stServerSock;
	std::vector<HANDLE>		m_vecHandle;
	CHelpTool				m_cHelpTool;
	HANDLE					m_pTArrayHandle[MAX_OPEN_PORT];

	DWORD InitThread();
	DWORD SetSocket(SOCKET socket);
public:
	CServer();
	~CServer();

	DWORD InitServer(DWORD dwPort, DWORD dwBackLog = 10);
	DWORD AcceptServer(DWORD dwPort);

	DWORD StartServer();
};

unsigned int WINAPI MainThread(VOID *pParam);

#endif