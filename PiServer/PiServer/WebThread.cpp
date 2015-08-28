#include "WebThread.h"
#include "Log.h"

#define MAX_BUFFER 32

////////////////////////////////////////////////////////////////////////////////////////
CWebThread::CWebThread()
{
}

////////////////////////////////////////////////////////////////////////////////////////
CWebThread::~CWebThread()
{
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CWebThread::RecvDataFromWebServer(DWORD &refdwValue)
{
	char *szBuf = NULL;
	szBuf = new char[MAX_BUFFER];
	::ZeroMemory(szBuf, MAX_BUFFER);

	int iRecv = 0;
	iRecv = ::recv(m_socket, (char *)szBuf, MAX_BUFFER, 0);
	
	std::string strMessage = szBuf;
	DWORD dwRet;
	dwRet = ParseData(strMessage);
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_WEB_PORT] Fail to parse data");
		return E_RET_FAIL;
	}

	refdwValue = ::atoi(strMessage.c_str());
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CWebThread::ParseData(std::string &refstrMessage)
{
	/*
		Check "?" for "GET" method
	*/
	DWORD dwPos;
	dwPos = refstrMessage.find_first_of("?");
	if (dwPos == std::string::npos) {
		return E_RET_SUCCESS;
	}
	refstrMessage = refstrMessage.substr(dwPos + 1);
	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CWebThread::StartWebThread(SOCKET socket)
{
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_WEB_PORT] Star Web Thread");
	DWORD dwValue = 0;
	DWORD dwRet;
	m_socket = socket;

	dwRet = RecvDataFromWebServer(dwValue);
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_WEB_PORT] Fail to receive data from web server");
		return E_RET_FAIL;
	}
	
	WriteLog(E_LOG_LEVEL_DEBUG, "[Thread_WEB_PORT] Push data to queue [%d]", dwValue);
	dwRet = CDataQueue::CreateDataQueue()->PushDataToQueue(dwValue);
	if (dwRet != E_RET_SUCCESS) {
		WriteLog(E_LOG_LEVEL_ERROR, "[Thread_WEB_PORT] Fail to push data to queue");
		return E_RET_FAIL;
	}
	
	return E_RET_SUCCESS;
}
