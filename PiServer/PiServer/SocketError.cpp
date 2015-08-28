#include "SocketError.h"
#include "Log.h"

DWORD CheckSocketError(DWORD dwError, E_THREAD_TYPE eThreadType)
{
	DWORD dwRet = E_RET_SUCCESS;
	if (dwError == 0) {
		/*
		Success
		*/
	}
	else if (dwError == WSAENOTSOCK) {
		/*
		Socket operation on nonsocket.
		An operation was attempted on something that is not a socket.
		Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid.
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_TYPE %d] [Error] WSAENOTSOCK", eThreadType);
		::Sleep(1000);
		dwRet = E_RET_FAIL;
	}
	else if (dwError == WSAECONNABORTED) {
		/*
		Software caused connection abort.
		An established connection was aborted by the software in your host computer, possibly due to a data transmission time-out or protocol error.
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_TYPE %d] [Error] WSAECONNABORTED", eThreadType);
		dwRet = E_RET_FAIL;
	}
	else if (dwError == WSAEFAULT) {
		/*
		Bad address.
		The system detected an invalid pointer address in attempting to use a pointer argument of a call.
		This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small.
		For instance, if the length of an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr).
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_TYPE %d] [Error] WSAEFAULT", eThreadType);
		dwRet = E_RET_FAIL;
	}
	else if (dwError == WSAETIMEDOUT) {
		/*
		Connection timed out.
		A connection attempt failed because the connected party did not properly respond after a period of time,
		or the established connection failed because the connected host has failed to respond.
		*/
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_TYPE %d] [Error] WSAETIMEDOUT", eThreadType);
		dwRet = E_RET_FAIL;
	}
	else {
		WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_TYPE %d] [Error] %d",eThreadType , dwError);
		dwRet = E_RET_FAIL;
	}

	return dwRet;
}