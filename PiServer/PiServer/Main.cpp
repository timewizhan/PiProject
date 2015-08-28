#include "Common.h"
#include "Server.h"
#include "Log.h"

int main(int argc, char **argv)
{
	if (argc < 1) {
		printf("Usage : [Command]");
		return E_RET_FAIL;
	}

	CServer Server;
	DWORD dwRet;
	try
	{
		dwRet = Server.StartServer();
		if (dwRet != E_RET_SUCCESS)
			throw std::exception("Fail to Start Server");

		/*
			Running Server ...
		*/
	}
	catch (std::exception &e)
	{
		WriteLog(E_LOG_LEVEL_ERROR, "%s", e.what());
		return E_RET_FAIL;
	}

	/*
		Succeed to poweroff Server
	*/

	WriteLog(E_LOG_LEVEL_DEBUG, "Succeed to poweroff Server");

	return E_RET_SUCCESS;
}