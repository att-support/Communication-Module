#include "pch.h"
#include <string>
#include "ClientMain.h"

ClientMain::ClientMain()
{
	_isRunning = false;
#ifdef _DEBUG
	_hDll = LoadLibrary("CommModule.dll");
	//_hDll = LoadLibrary("..\\..\\x64\\Debug\\CommModule.dll");
#else
	_hDll = LoadLibrary("CommModule.dll");
#endif
	if (_hDll == NULL) {
		printf("LoadLibrary(%d)\n", GetLastError());
		return;
	}
	ManApiSetParameterInitComm = (FUNC_ManApiSetParameterInitComm)GetProcAddress(_hDll, "ManApiSetParameterInitComm");
	if (ManApiSetParameterInitComm == NULL) return;

	ManApiAppendListener = (FUNC_ManApiAppendListener)GetProcAddress(_hDll, "ManApiAppendListener");
	if (ManApiAppendListener == NULL) return;

	ManApiSetParameterInitBrowser = (FUNC_ManApiSetParameterInitBrowser)GetProcAddress(_hDll, "ManApiSetParameterInitBrowser");
	if (ManApiSetParameterInitBrowser == NULL) return;

	ManApiStartServer = (FUNC_ManApiStartServer)GetProcAddress(_hDll, "ManApiStartServer");
	if (ManApiStartServer == NULL) return;

	ManApiStartClient = (FUNC_ManApiStartClient)GetProcAddress(_hDll, "ManApiStartClient");
	if (ManApiStartClient == NULL) return;

	ManApiStop = (FUNC_ManApiStop)GetProcAddress(_hDll, "ManApiStop");
	if (ManApiStop == NULL) return;

	InputApiPushData = (FUNC_InputApiPushData)GetProcAddress(_hDll, "InputApiPushData");
	if (InputApiPushData == NULL) return;

	InputApiPushDataById = (FUNC_InputApiPushDataById)GetProcAddress(_hDll, "InputApiPushDataById");
	if (InputApiPushDataById == NULL) return;

	OutputApiPullData = (FUNC_OutputApiPullData)GetProcAddress(_hDll, "OutputApiPullData");
	if (OutputApiPullData == NULL) return;

	OutputApiPullDataById = (FUNC_OutputApiPullDataById)GetProcAddress(_hDll, "OutputApiPullDataById");
	if (OutputApiPullDataById == NULL) return;

	CreateCommData = (FUNC_CreateCommData)GetProcAddress(_hDll, "CreateCommData");
	if (CreateCommData == NULL) return;

	ReleaseCommData = (FUNC_ReleaseCommData)GetProcAddress(_hDll, "ReleaseCommData");
	if (ReleaseCommData == NULL) return;

	ManApiSetLogDirPath = (FUNC_ManApiSetLogDirPath)GetProcAddress(_hDll, "ManApiSetLogDirPath");
	if (ManApiSetLogDirPath == NULL) return;
}


ClientMain::~ClientMain()
{
	if (!FreeLibrary(_hDll)) {
		printf("FreeLibrary(%d)\n", GetLastError());
		return;
	}
}


void ClientMain::TestStart(int port)
{
	char logpath[64] = { 0 };
	sprintf_s(logpath, "c:\\tmp\\comm_client_%d", port);
	//TEST
	ManApiSetLogDirPath(logpath);

	int ret = ManApiAppendListener(this);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiAppendListener() failed. ret=%d\n", ret);
		return;
	}
	ret = ManApiStartClient(port);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiStartClient() failed. ret=%d\n", ret);
		return;
	}

	unsigned char* data6 = new unsigned char[8294408];
	//unsigned char* data6 = new unsigned char[16];

	int pushCounter = 0;
	int pushCount = 0;
	int testCounter = 0;
	_isRunning = true;
	while (_isRunning) {
		if ((pushCounter >= 30) || (pushCounter == 0)) {
			pushCounter = 0;
			pushCount++;
			E_BML_IF_COMM_TYPE type;
			ICommData* commData;

			type = BML_IF_COMM_TYPE_STATUS_INFO;
			//const unsigned char data6[] = "BML_IF_COMM_TYPE_STATUS_INFO";
			commData = CreateCommData();
			//commData->SetData(data6, sizeof(data6));
			commData->SetData(data6, 8294408);
			//commData->SetData(data6, 16);
			//printf("InputApiPushData() type=%d %s\n", type, data6);
			printf("InputApiPushData() type=%d size=8294408\n", type);
			//printf("InputApiPushData() type=%d size=16\n", type);
			ret = InputApiPushData(type, commData);
			if (ret == MAN_API_RET_SUCCSESS) {
				printf("  -> OK\n");
			}
			else if (ret == MAN_API_RET_BUSY) {
				printf("  -> BUSY\n");
			}
			else {
				printf("  -> ERROR (%d)\n", ret);
			}
			ReleaseCommData(&commData);

			type = BML_IF_COMM_TYPE_AUDIO_CTRL_INFO;
			const unsigned char data7[] = "BML_IF_COMM_TYPE_AUDIO_CTRL_INFO";
			commData = CreateCommData();
			commData->SetData(data7, sizeof(data7));
			printf("InputApiPushData() type=%d %s\n", type, data7);
			ret = InputApiPushData(type, commData);
			if (ret == MAN_API_RET_SUCCSESS) {
				printf("  -> OK\n");
			}
			else if (ret == MAN_API_RET_BUSY) {
				printf("  -> BUSY\n");
			}
			else {
				printf("  -> ERROR (%d)\n", ret);
			}
			ReleaseCommData(&commData);
		}


		ICommData* commData = NULL;
		int ret = OutputApiPullData(BML_IF_COMM_TYPE_UNIT_INFO, &commData);
		if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
			int dataSize = commData->GetDataSize();
			printf("OutputApiPullData() -> OK (type=%d size=%d data=%s)\n", BML_IF_COMM_TYPE_UNIT_INFO, dataSize, commData->GetData());
		}
		else if (ret == MAN_API_RET_NOT_EXIST) {
			//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_UNIT_INFO);
		}
		else if (ret == MAN_API_RET_BUSY) {
			printf("OutputApiPullData() -> Busy (type=%d)\n", BML_IF_COMM_TYPE_UNIT_INFO);
		}
		else {
			printf("OutputApiPullData() -> Error (type=%d ret=%d)\n", BML_IF_COMM_TYPE_UNIT_INFO, ret);
		}
		ReleaseCommData(&commData);

		commData = NULL;
		ret = OutputApiPullData(BML_IF_COMM_TYPE_PLAYER_INFO, &commData);
		if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
			int dataSize = commData->GetDataSize();
			printf("OutputApiPullData() -> OK (type=%d size=%d data=%s)\n", BML_IF_COMM_TYPE_PLAYER_INFO, dataSize, commData->GetData());
		}
		else if (ret == MAN_API_RET_NOT_EXIST) {
			//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_PLAYER_INFO);
		}
		else if (ret == MAN_API_RET_BUSY) {
			printf("OutputApiPullData() -> Busy (type=%d)\n", BML_IF_COMM_TYPE_PLAYER_INFO);
		}
		else {
			printf("OutputApiPullData() -> Error (type=%d ret=%d)\n", BML_IF_COMM_TYPE_PLAYER_INFO, ret);
		}
		ReleaseCommData(&commData);

		commData = NULL;
		ret = OutputApiPullData(BML_IF_COMM_TYPE_REMOCON_EVENT, &commData);
		if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
			int dataSize = commData->GetDataSize();
			printf("OutputApiPullData() -> OK (type=%d size=%d data=%s)\n", BML_IF_COMM_TYPE_REMOCON_EVENT, dataSize, commData->GetData());
		}
		else if (ret == MAN_API_RET_NOT_EXIST) {
			//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_REMOCON_EVENT);
		}
		else if (ret == MAN_API_RET_BUSY) {
			printf("OutputApiPullData() -> Busy (type=%d)\n", BML_IF_COMM_TYPE_REMOCON_EVENT);
		}
		else {
			printf("OutputApiPullData() -> Error (type=%d ret=%d)\n", BML_IF_COMM_TYPE_REMOCON_EVENT, ret);
		}
		ReleaseCommData(&commData);

		commData = NULL;
		ret = OutputApiPullData(BML_IF_COMM_TYPE_SECTION, &commData);
		if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
			int dataSize = commData->GetDataSize();
			printf("OutputApiPullData() -> OK (type=%d size=%d data=%s)\n", BML_IF_COMM_TYPE_SECTION, dataSize, commData->GetData());
		}
		else if (ret == MAN_API_RET_NOT_EXIST) {
			//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_SECTION);
		}
		else if (ret == MAN_API_RET_BUSY) {
			printf("OutputApiPullData() -> Busy (type=%d)\n", BML_IF_COMM_TYPE_SECTION);
		}
		else {
			printf("OutputApiPullData() -> Error (type=%d ret=%d)\n", BML_IF_COMM_TYPE_SECTION, ret);
		}
		ReleaseCommData(&commData);

		if (testCounter >= 400) {
			commData = NULL;
			ret = OutputApiPullData(BML_IF_COMM_TYPE_STC, &commData);
			if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
				int dataSize = commData->GetDataSize();
				printf("OutputApiPullData() -> OK (type=%d size=%d data=%s)\n", BML_IF_COMM_TYPE_STC, dataSize, commData->GetData());
			}
			else if (ret == MAN_API_RET_NOT_EXIST) {
				//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_STC);
			}
			else if (ret == MAN_API_RET_BUSY) {
				printf("OutputApiPullData() -> Busy (type=%d)\n", BML_IF_COMM_TYPE_STC);
			}
			else {
				printf("OutputApiPullData() -> Error (type=%d ret=%d)\n", BML_IF_COMM_TYPE_STC, ret);
			}
			ReleaseCommData(&commData);
		}

		pushCounter++;
		testCounter++;
		Sleep(10);
	}
}

void ClientMain::NotifyInfo(E_BML_IF_COMM_TYPE bmlIfCommType, E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo)
{

	std::string strCb = "";
	if (bmlIfCommCbInfo == BML_IF_COMM_CB_INFO_DISCONNECTED) {
		strCb = "Disconnected";
	}
	else if (bmlIfCommCbInfo == BML_IF_COMM_CB_INFO_TERM_REQ) {
		strCb = "Termination";
	}
	else {
		strCb = "Unknown";
	}

	printf("[CallBack] TYPE=%d CB=%d(%s)\n", (int)bmlIfCommType, (int)bmlIfCommCbInfo, strCb.c_str());

	if (bmlIfCommCbInfo == BML_IF_COMM_CB_INFO_TERM_REQ) {
		_isRunning = false;
	}
}
