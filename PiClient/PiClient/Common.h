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

enum E_RET_TYPE
{
	E_RET_SUCCESS = 0,
	E_RET_ACCEPT_FAIL,
	E_RET_IOCP_FAIL,
	E_RET_SEND_FAIL,
	E_RET_RECV_FAIL,
	E_RET_FAIL
};

#endif