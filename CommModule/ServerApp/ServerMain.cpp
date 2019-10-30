#include "pch.h"
#include <string>
#include "ServerMain.h"

ServerMain::ServerMain(int pattern)
{
	this->pattern = pattern;
#ifdef _DEBUG
	_hDll = LoadLibrary("CommModule.dll");
	//_hDll = LoadLibrary("c:\\tmp\\CommModule.dll");
#else
	_hDll = LoadLibrary("CommModule.dll");
	//_hDll = LoadLibrary("c:\\tmp\\CommModule.dll");
#endif
	if (_hDll == NULL) {
		printf("LoadLibrary(%d)\n", GetLastError());
	}
	ManApiSetParameterInitComm = (FUNC_ManApiSetParameterInitComm)GetProcAddress(_hDll, "ManApiSetParameterInitComm");
	if (ManApiSetParameterInitComm == NULL) return;

	ManApiAppendListenerById = (FUNC_ManApiAppendListenerById)GetProcAddress(_hDll, "ManApiAppendListenerById");
	if (ManApiAppendListenerById == NULL) return;

	ManApiRemoveListenerById = (FUNC_ManApiRemoveListenerById)GetProcAddress(_hDll, "ManApiRemoveListenerById");
	if (ManApiRemoveListenerById == NULL) return;

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

	ManApiSetLogDirPath("c:\\tmp\\comm_server");
}


ServerMain::~ServerMain()
{
	if (!FreeLibrary(_hDll)) {
		printf("FreeLibrary(%d)\n", GetLastError());
		return;
	}
}

// スレッド処理
int ServerMain::ThreadProc() {
	int ret = 0;
	if (!_isRunning) {
		return -1;
	}
	if ((pushCounter >= 20) || (pushCounter == 0)) {
		pushCounter = 0;
		pushCount++;

		E_BML_IF_COMM_TYPE type;
		ICommData* commData;

		type = BML_IF_COMM_TYPE_UNIT_INFO;
		const unsigned char data1[] = "BML_IF_COMM_TYPE_UNIT_INFO";
		commData = CreateCommData();
		commData->SetData(data1, sizeof(data1));
		printf("InputApiPushDataById() id=%d type=%d %s\n", pattern, type, data1);
		ret = InputApiPushDataById(pattern, type, commData);
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

		type = BML_IF_COMM_TYPE_PLAYER_INFO;
		const unsigned char data2[] = "BML_IF_COMM_TYPE_PLAYER_INFO";
		commData = CreateCommData();
		commData->SetData(data2, sizeof(data2));
		printf("InputApiPushDataById() id=%d type=%d %s\n", pattern, type, data2);
		ret = InputApiPushDataById(pattern, type, commData);
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

		type = BML_IF_COMM_TYPE_REMOCON_EVENT;
		const unsigned char data3[] = "BML_IF_COMM_TYPE_REMOCON_EVENT";
		commData = CreateCommData();
		commData->SetData(data3, sizeof(data3));
		printf("InputApiPushDataById() id=%d type=%d %s\n", pattern, type, data3);
		ret = InputApiPushDataById(pattern, type, commData);
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

		type = BML_IF_COMM_TYPE_SECTION;
		const unsigned char data4[] = "BML_IF_COMM_TYPE_SECTION";
		commData = CreateCommData();
		commData->SetData(data4, sizeof(data4));
		printf("InputApiPushDataById() id=%d type=%d %s\n", pattern, type, data4);
		ret = InputApiPushDataById(pattern, type, commData);
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

		type = BML_IF_COMM_TYPE_STC;
		const unsigned char data5[] = "BML_IF_COMM_TYPE_STC";
		commData = CreateCommData();
		commData->SetData(data5, sizeof(data5));
		printf("InputApiPushDataById() id=%d type=%d %s\n", pattern, type, data5);
		ret = InputApiPushDataById(pattern, type, commData);
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
	if (pushCount >= 10) {
		_isRunning = false;
	}

	ICommData* commData = NULL;
	ret = OutputApiPullDataById(pattern, BML_IF_COMM_TYPE_AUDIO_CTRL_INFO, &commData);
	if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
		int dataSize = commData->GetDataSize();
		printf("OutputApiPullDataById() -> OK (id=%d type=%d size=%d data=%s)\n", pattern, BML_IF_COMM_TYPE_AUDIO_CTRL_INFO, dataSize, commData->GetData());
	}
	else if (ret == MAN_API_RET_NOT_EXIST) {
		//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_AUDIO_CTRL_INFO);
	}
	else if (ret == MAN_API_RET_BUSY) {
		printf("OutputApiPullDataById() -> Busy (id=%d type=%d)\n", pattern, BML_IF_COMM_TYPE_AUDIO_CTRL_INFO);
	}
	else {
		printf("OutputApiPullDataById() -> Error (id=%d type=%d ret=%d)\n", pattern, BML_IF_COMM_TYPE_AUDIO_CTRL_INFO, ret);
	}
	ReleaseCommData(&commData);

	if (testCounter >= 50) {
		commData = NULL;
		ret = OutputApiPullDataById(pattern, BML_IF_COMM_TYPE_STATUS_INFO, &commData);
		if ((ret == MAN_API_RET_SUCCSESS) && (commData != NULL)) {
			int dataSize = commData->GetDataSize();
			//printf("OutputApiPullDataById() -> OK (id=%d type=%d size=%d data=%s)\n", pattern, BML_IF_COMM_TYPE_STATUS_INFO, dataSize, commData->GetData());
		}
		else if (ret == MAN_API_RET_NOT_EXIST) {
			//printf("[Debug] type=%d Not Received\n", BML_IF_COMM_TYPE_STATUS_INFO);
		}
		else if (ret == MAN_API_RET_BUSY) {
			printf("OutputApiPullDataById() -> Busy (id=%d type=%d)\n", pattern, BML_IF_COMM_TYPE_STATUS_INFO);
		}
		else {
			printf("OutputApiPullDataById() -> Error (id=%d type=%d ret=%d)\n", pattern, BML_IF_COMM_TYPE_STATUS_INFO, ret);
		}
		ReleaseCommData(&commData);
	}

	pushCounter++;
	testCounter++;
	Sleep(100);
	return 0;
}
// スレッド前処理
int ServerMain::ThreadInitProc() {
	int ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_UNIT_INFO, 55001 + (pattern*10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_PLAYER_INFO, 55002 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_REMOCON_EVENT, 55003 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_SECTION, 55004 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_STC, 55005 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_STATUS_INFO, 55006 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitComm(pattern, BML_IF_COMM_TYPE_AUDIO_CTRL_INFO, 55007 + (pattern * 10), 100);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitComm() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiAppendListenerById(pattern, this);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("FUNC_ManApiAppendListenerById() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiSetParameterInitBrowser(pattern, "C:\\tmp\\ClientApp.exe", 55000 + (pattern * 10));
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiSetParameterInitBrowser() failed. ret=%d\n", ret);
		return -1;
	}
	ret = ManApiStartServer(pattern);
	if (ret != MAN_API_RET_SUCCSESS) {
		printf("ManApiStartServer() failed. ret=%d\n", ret);
		return -1;
	}

	pushCounter = 0;
	pushCount = 0;
	testCounter = 0;
	_isRunning = true;
	return 0;
}
// スレッド後処理
int ServerMain::ThreadTermProc() {
	ManApiRemoveListenerById(pattern);
	ManApiStop(pattern);
	return 0;
}


void ServerMain::TestStart()
{
	StartThread();
}

void ServerMain::TestStop() {
	ManApiStop(pattern);
	_isRunning = false;
	StopThread(3);
}

void ServerMain::NotifyInfo(int id, E_BML_IF_COMM_TYPE bmlIfCommType, E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo)
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

	printf("[CallBack] id=%d TYPE=%d CB=%d(%s)\n", id, (int)bmlIfCommType, (int)bmlIfCommCbInfo, strCb.c_str());
}
