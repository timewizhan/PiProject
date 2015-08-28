#ifndef _CLIENT_
#define _CLIENT_

#include "Common.h"
#include "Protocol.h"

#define MAX_THREAD	2
#define SLEEP_TIME	1000

extern BOOL g_bThreadExit;

struct ST_CLIENT_SOCK
{
	WSADATA			wsaData;
	SOCKADDR_IN		stSockAddr_in;
	SOCKET			sockServer;
	DWORD			dwPort;
	std::string		strAddress;

	ST_CLIENT_SOCK() : dwPort(0) {}
};

class CClient
{
	HANDLE				m_hArray[MAX_THREAD];
	ST_CLIENT_SOCK		m_stClientSock;

	DWORD MakeThread();
	DWORD ConnectServer();
public:
	CClient();
	~CClient();

	DWORD InitClient(std::string strAddress, DWORD dwPort);
	DWORD StartClient();
};

unsigned int WINAPI WorkerActionThread(VOID *pParam);
unsigned int WINAPI WorkerHealthThread(VOID *pParam);

int SendHeader(SOCKET sock);

int SendHealthData(SOCKET sock);
int RecvHealthData(SOCKET sock);

int SendActionData(SOCKET sock);
int RecvActionData(SOCKET sock);

#endif _CLIENT_