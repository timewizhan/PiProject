#ifndef _COMMON_
#define _COMMON_

#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>

#include "Protocol.h"
#include "DataQueue.h"

enum E_RET_TYPE
{
	E_RET_SUCCESS = 0,
	E_RET_ACCEPT_FAIL,
	E_RET_IOCP_FAIL,
	E_RET_RECV_FAIL,
	E_RET_FAIL
};

enum E_THREAD_TYPE
{
	E_THREAD_CONTROL = 0,
	E_THREAD_HEALTH,
	E_THREAD_WEB
};

#endif