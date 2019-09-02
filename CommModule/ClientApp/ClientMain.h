#pragma once

typedef int(__stdcall *FUNC_ManApiSetParameterInitComm)(E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
typedef int(__stdcall *FUNC_ManApiAppendListener)(ICommListener* pListener);
typedef void(__stdcall *FUNC_ManApiRemoveListener)();
typedef int(__stdcall *FUNC_ManApiSetParameterInitBrowser)(const char* pBmlBrowserPath, int port);
typedef int(__stdcall *FUNC_ManApiStartServer)();
typedef int(__stdcall *FUNC_ManApiStartClient)(int port);
typedef int(__stdcall *FUNC_ManApiStop)();
typedef int(__stdcall *FUNC_InputApiPushData)(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
typedef int(__stdcall *FUNC_InputApiPushDataById)(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
typedef int(__stdcall *FUNC_OutputApiPullData)(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
typedef int(__stdcall *FUNC_OutputApiPullDataById)(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
typedef ICommData*(__stdcall *FUNC_CreateCommData)();
typedef void(__stdcall *FUNC_ReleaseCommData)(ICommData** pData);
typedef void(__stdcall *FUNC_ManApiSetLogDirPath)(const char* pLogDirPath);


class ClientMain : ICommListener
{
public:
	ClientMain();
	virtual ~ClientMain();
	void TestStart(int port);

	void NotifyInfo(E_BML_IF_COMM_TYPE bmlIfCommType,
		E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo);

private:
	FUNC_ManApiSetParameterInitComm ManApiSetParameterInitComm;
	FUNC_ManApiAppendListener ManApiAppendListener;
	FUNC_ManApiRemoveListener ManApiRemoveListener;
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

