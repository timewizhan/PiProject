#include "Client.h"
#include "Log.h"

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage : [Command] [Server Address] [Server Port]\n");
		return 0;
	}

	WriteLog(E_LOG_LEVEL_DEBUG, "[Server Address] [Server Port]", argv[1], argv[2]);
	std::string strAddress	= argv[1];
	DWORD dwPort			= ::atoi(argv[2]);

	CClient Client;
	DWORD dwRet;
	try
	{
		WriteLog(E_LOG_LEVEL_DEBUG, "Start to init client");
		dwRet = Client.InitClient(strAddress, dwPort);
		if (dwRet != E_RET_SUCCESS)
			throw std::exception("Fail to init client");

		WriteLog(E_LOG_LEVEL_DEBUG, "Start to start client");
		dwRet = Client.StartClient();
		if (dwRet != E_RET_SUCCESS)
			throw std::exception("Fail to start client");

		/*
			Never here
		*/
	}
	catch (std::exception &e)
	{
		WriteLog(E_LOG_LEVEL_ERROR, e.what());
		return 0;
	}

	/*
		normally, success
	*/
	WriteLog(E_LOG_LEVEL_DEBUG, "Exit client");
	return 0;
}