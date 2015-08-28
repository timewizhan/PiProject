#include "HelpTool.h"

//#define _WINSOCK_DEPRECATED_NO_WARNINGS

////////////////////////////////////////////////////////////////////////////////////////
CHelpTool::CHelpTool()
{
}

////////////////////////////////////////////////////////////////////////////////////////
CHelpTool::~CHelpTool()
{
}


////////////////////////////////////////////////////////////////////////////////////////
DWORD	CHelpTool::GetSystemInfo(SYSTEM_INFO &refstSystemInfo)
{
	::GetSystemInfo(&refstSystemInfo);
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
HANDLE	CHelpTool::CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	return ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

////////////////////////////////////////////////////////////////////////////////////////
BOOL	CHelpTool::AssociateDeviceWithompletionPort(HANDLE hDevice, HANDLE hCompletionPort, DWORD dwCompletionKey)
{
	HANDLE h = ::CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);
	return (h == hCompletionPort);
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD	CHelpTool::GetAddressInfo(std::vector<std::string> &refvecstrAddress)
{
	char cHostName[256];
	int iRet;
	iRet = ::gethostname(cHostName, sizeof(cHostName));
	if (iRet == SOCKET_ERROR)
		return E_RET_FAIL;

	struct addrinfo hints, *res = NULL;
	::ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype		= SOCK_STREAM;
	hints.ai_family			= AF_INET;
	hints.ai_protocol		= IPPROTO_TCP;
	iRet = ::getaddrinfo(cHostName, "80", &hints, &res);
	if (iRet != 0)
		return E_RET_FAIL;

	struct addrinfo *pstAddrInfoNext = res;
	while (pstAddrInfoNext != NULL)
	{
		struct in_addr addr;
		addr.S_un = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.S_un;

		char szBuf[128];
		inet_ntop(AF_INET, &addr.S_un, szBuf, sizeof(szBuf));
		std::string strAddress = szBuf;
		refvecstrAddress.push_back(strAddress);
		pstAddrInfoNext = pstAddrInfoNext->ai_next;
	}


	if (!refvecstrAddress.size())
		return E_RET_FAIL;
	
	::freeaddrinfo(res);
	return E_RET_SUCCESS;
}



