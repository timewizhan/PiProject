#ifndef _HELP_TOOL_
#define _HELP_TOOL_

#include "Common.h"

class CHelpTool
{
public:
	CHelpTool();
	~CHelpTool();

	DWORD	GetSystemInfo(SYSTEM_INFO &refstSystemInfo);
	DWORD	GetAddressInfo(std::vector<std::string> &refvecstrAddress);
	HANDLE	CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads);
	BOOL	AssociateDeviceWithompletionPort(HANDLE hDevice, HANDLE hCompletionPort, DWORD dwCompletionKey);

};

#endif