#pragma once

#include "Util/Thread.h"

typedef int(__stdcall *FUNC_ManApiSetParameterInitComm)(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
typedef int(__stdcall *FUNC_ManApiAppendListenerById)(int id, ICommListenerById* pListener);
typedef void(__stdcall *FUNC_ManApiRemoveListenerById)(int id);
typedef int(__stdcall *FUNC_ManApiSetParameterInitBrowser)(int id, const char* pBmlBrowserPath, int port);
typedef int(__stdcall *FUNC_ManApiStartServer)(int id);
typedef int(__stdcall *FUNC_ManApiStartClient)(int port);
typedef int(__stdcall *FUNC_ManApiStop)(int id);
typedef int(__stdcall *FUNC_InputApiPushData)(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
typedef int(__stdcall *FUNC_InputApiPushDataById)(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
typedef int(__stdcall *FUNC_OutputApiPullData)(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
typedef int(__stdcall *FUNC_OutputApiPullDataById)(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
typedef ICommData*(__stdcall *FUNC_CreateCommData)();
typedef void(__stdcall *FUNC_ReleaseCommData)(ICommData** pData);
typedef void(__stdcall *FUNC_ManApiSetLogDirPath)(const char* pLogDirPath);


class ServerMain : public Thread, ICommListenerById
{
public:
	ServerMain(int pattern);
	virtual ~ServerMain();
	void TestStart();
	void TestStop();

	void NotifyInfo(int id, E_BML_IF_COMM_TYPE bmlIfCommType,
		E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo);

private:
	int pattern;
	int pushCounter;
	int pushCount;
	int testCounter;
	//
	// Thread class override methods
	//
	// スレッド処理
	virtual int ThreadProc();
	// スレッド前処理
	virtual int ThreadInitProc();
	// スレッド後処理
	virtual int ThreadTermProc();

	FUNC_ManApiSetParameterInitComm ManApiSetParameterInitComm;
	FUNC_ManApiAppendListenerById ManApiAppendListenerById;
	FUNC_ManApiRemoveListenerById ManApiRemoveListenerById;
	FUNC_ManApiSetParameterInitBrowser ManApiSetParameterInitBrowser;
	FUNC_ManApiStartServer ManApiStartServer;
	FUNC_ManApiStartClient ManApiStartClient;
	FUNC_ManApiStop ManApiStop;
	FUNC_InputApiPushData InputApiPushData;
	FUNC_InputApiPushDataById InputApiPushDataById;
	FUNC_OutputApiPullData OutputApiPullData;
	FUNC_OutputApiPullDataById OutputApiPullDataById;
	FUNC_CreateCommData CreateCommData;
	FUNC_ReleaseCommData ReleaseCommData;
	FUNC_ManApiSetLogDirPath ManApiSetLogDirPath;
	HMODULE _hDll;
	bool _isRunning;
};

