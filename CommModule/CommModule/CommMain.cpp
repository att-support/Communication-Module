#include "stdafx.h"
#include <Tcp/TcpCommon.h>
#include <Util/Logger.h>
#include "CommDef.h"
#include "CommMain.h"
#include "CommLogger.h"
#include "CommModuleClient.h"
#include "CommModuleServer.h"
#include "CommCmdBuilder.h"

//
// 通信制御メイン
//

//
// コンストラクタ（隠蔽）
//
CommMain::CommMain(int id)
{
	_id = id;
	_ctrlCommModule = NULL;
	_isServer = false;
	_isTerminating = false;
}

//
// デストラクタ（隠蔽）
//
CommMain::~CommMain()
{
	_StopProc();
	CommLogger::Destroy();
}

//
// 通信ごとのパラメータ設定
//
int CommMain::SetParameterInitComm(E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize)
{
	LOG("[CommMain(id=%d)] Set parameter. (type=%d port=%d queue-size=%d)", _id, bmlIfCommType, port, queueSize);
	CommCtrlInfo ctrlInfo(bmlIfCommType, port, queueSize);
	_ctrlInfoList.push_back(ctrlInfo);
	return MAN_API_RET_SUCCSESS;
}

//
// リスナー登録
//
int CommMain::AppendListener(ICommListener* pListener)
{
	LOG("[CommMain(id=%d)] Set listener.", _id);
	_pListenerList.push_back(pListener);
	return MAN_API_RET_SUCCSESS;
}

//
// リスナー登録(ID指定)
//
int CommMain::AppendListener(ICommListenerById* pListener)
{
	LOG("[CommMain(id=%d)] Set listener. (id=%d)", _id, _id);
	_pListenerListById.push_back(pListener);
	return MAN_API_RET_SUCCSESS;
}

//
// リスナー削除
//
void CommMain::RemoveListener()
{
	LOG("[CommMain(id=%d)] Remove listener.", _id);
	_pListenerList.clear();
}

//
// リスナー削除(ID指定)
//
void CommMain::RemoveListenerById()
{
	LOG("[CommMain(id=%d)] Remove listener. (id=%d)", _id, _id);
	_pListenerListById.clear();
}

//
// クライアント初期設定（プロセスパスと制御ポート）
//
int CommMain::SetParameterInitBrowser(const char* pBmlBrowserPath, int port)
{
	LOG("[CommMain(id=%d)] Set process file path. (path=%s control-port=%d)", _id, pBmlBrowserPath, port);
	_bmlBrowserPath = pBmlBrowserPath;
	_ctrlPort = port;
	return MAN_API_RET_SUCCSESS;
}

//
// サーバ通信開始
//
int CommMain::StartServer()
{
	LOG("[CommMain(id=%d)] Start server.", _id);

	if (TcpCommon::Startup() != 0) {
		return MAN_API_RET_ERROR;
	}
	_ctrlCommModule = new CommModuleServer(BML_IF_COMM_TYPE_CTRL, 1, _ctrlPort);
	_ctrlCommModule->SetOwner(this);
	if (_ctrlCommModule->Start() != 0) {
		LOG("[CommMain(id=%d)] [ERROR] Server comminucation module start failed. (port=%d)", _id, _ctrlPort);
		return MAN_API_RET_ERROR;
	}

	bool isError = false;
	for (CommCtrlInfoList::iterator itr = _ctrlInfoList.begin();
		itr != _ctrlInfoList.end(); itr++)
	{
		CommModule* mod = new CommModuleServer((*itr).GetType(), (*itr).GetQueSize(), (*itr).GetPort());
		mod->SetOwner(this);
		_dataCommModuleList[(*itr).GetType()] = mod;
		if (mod->Start() != 0) {
			isError = true;
			LOG("[CommMain(id=%d)] [ERROR] Server comminucation module start failed. (port=%d)", _id, (*itr).GetPort());
			break;
		}
		LOG("[CommMain(id=%d)] Add client info. (type=%d qsize=%d port=%d)", _id,
			(*itr).GetType(), (*itr).GetQueSize(), (*itr).GetPort());
	}

	if (isError) {
		_StopProc();
		return MAN_API_RET_ERROR;
	}
	
	//Start browser app.
	char buf[6];
	sprintf(buf, "%d", _ctrlPort);
	std::string strPort(buf);
	std::string cl = _bmlBrowserPath + " " + strPort;
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { sizeof(STARTUPINFO) };

	LOG("[CommMain(id=%d)] Start process. (path=%s ctrl-port=%d)", _id, _bmlBrowserPath.c_str(), _ctrlPort);
	int result = CreateProcess(NULL, (LPSTR)cl.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	// For a form application
	//	int result = CreateProcess(NULL, (LPSTR)cl.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	if (result == 0) {
		LOG("[CommMain(id=%d)] [ERROR] Start process failed. (path=%s)", _id, _bmlBrowserPath.c_str());
		_StopProc();
		return MAN_API_RET_ERROR;
	}

	_isServer = true;

	return MAN_API_RET_SUCCSESS;
}

//
// クライアント通信開始
//
int CommMain::StartClient(int port)
{
	LOG("[CommMain(id=%d)] Start client. (port=%d)", _id, port);

	if (TcpCommon::Startup() != 0) {
		return MAN_API_RET_ERROR;
	}
	_ctrlPort = port;
	_ctrlCommModule = new CommModuleClient(BML_IF_COMM_TYPE_CTRL, 1, "127.0.0.1", _ctrlPort);
	_ctrlCommModule->SetOwner(this);
	if (_ctrlCommModule->Start() != 0) {
		LOG("[CommMain(id=%d)] [ERROR] Start failed. communication module for control (port=%d)", _id, _ctrlPort);
		return MAN_API_RET_ERROR;
	}

	_isServer = false;

	return MAN_API_RET_SUCCSESS;
}

//
// 通信停止
//
int CommMain::Stop()
{
	int retCode = MAN_API_RET_SUCCSESS;
	if (_isServer) {
		_isServer = false;
		LOG("[CommMain(id=%d)] Stop server.", _id);

		CommCommand* cmd = CommCmdBuilder::CreateTermCommand();
		int ret = _ctrlCommModule->SendCommand(cmd);
		delete cmd;
		if (ret != 0) {
			retCode = MAN_API_RET_ERROR;
		}
	}
	else {
		LOG("[CommMain(id=%d)] Stop client.", _id);
	}
	_StopProc();

	TcpCommon::Cleanup();
	return retCode;
}

//
// データ入力(送信)
//
int CommMain::PushData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
#ifdef _DEBUG_LOG_
	LOG("[CommMain(id=%d)] Push data. (type=%d)", _id, bmlIfCommType);
#endif

	if (_dataCommModuleList.find(bmlIfCommType) == _dataCommModuleList.end()) {
		LOG("[CommMain(id=%d)] [ERROR] Push data failed. Unknown type. (type=%d)", _id, bmlIfCommType);
		return MAN_API_RET_ERROR;
	}
	CommModule* mod = _dataCommModuleList[bmlIfCommType];

	return mod->PushData(pData);
}

//
// データ出力(受信)
//
int CommMain::PullData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
#ifdef _DEBUG_LOG_
	LOG("[CommMain(id=%d)] Pull data. (type=%d)", bmlIfCommType);
#endif

	if (_dataCommModuleList.find(bmlIfCommType) == _dataCommModuleList.end()) {
		LOG("[CommMain(id=%d)] [WARN]  Pull data failed. Unknown type. (type=%d)", _id, bmlIfCommType);
		return MAN_API_RET_NOT_EXIST;
	}
	CommModule* mod = _dataCommModuleList[bmlIfCommType];

	return mod->PullData(pData);
}

//
// 通信制御情報受信コールバック(CommModuleInterface)
//
void CommMain::NotifyCtrlInfo(CommCtrlInfoList& ctrlInfoList)
{
	LOG("[CommMain(id=%d)] Received CtrlInfo. (info-count=%d)", _id, _ctrlInfoList.size());

	if (_isServer) return;
	if (_isTerminating) return;

	_ctrlInfoList = ctrlInfoList;
	for (CommCtrlInfoList::iterator itr = _ctrlInfoList.begin();
		itr != _ctrlInfoList.end(); itr++)
	{
		CommModule* mod = new CommModuleClient((*itr).GetType(), (*itr).GetQueSize(), "127.0.0.1", (*itr).GetPort());
		mod->SetOwner(this);
		_dataCommModuleList[(*itr).GetType()] = mod;
		LOG("[CommMain(id=%d)] Start connection. (type=%d qsize=%d port=%d)", _id,
			(*itr).GetType(), (*itr).GetQueSize(), (*itr).GetPort());
		if (mod->Start() != 0) {
			LOG("[CommMain(id=%d)] [ERROR] Start connection failed. (type=%d qsize=%d port=%d)", _id,
				(*itr).GetType(), (*itr).GetQueSize(), (*itr).GetPort());
			for (std::map<E_BML_IF_COMM_TYPE,CommModule*>::iterator itr = _dataCommModuleList.begin();
				itr != _dataCommModuleList.end(); itr++)
			{
				(*itr).second->Stop();
				delete (*itr).second;
			}
			_dataCommModuleList.clear();
			return;
		}
	}
}

//
// 状態通知受信コールバック(CommModuleInterface)
//
void CommMain::NotifyStat(bool isBusy)
{
	LOG("[CommMain(id=%d)] Received status notify. (Is %sbusy)", _id, (isBusy ? "" : "not "));
}

//
// 終了通知受信コールバック(CommModuleInterface)
//
void CommMain::NotifyTerm()
{
	LOG("[CommMain(id=%d)] Received terminate notify!.", _id);

	if (_isServer) return;

	//通信停止
	_TermClient();

	for (std::vector<ICommListener*>::iterator itr = _pListenerList.begin();
		itr != _pListenerList.end(); itr++)
	{
		(*itr)->NotifyInfo(BML_IF_COMM_TYPE_CTRL, BML_IF_COMM_CB_INFO_TERM_REQ);
	}
	for (std::vector<ICommListenerById*>::iterator itr = _pListenerListById.begin();
		itr != _pListenerListById.end(); itr++)
	{
		(*itr)->NotifyInfo(_id, BML_IF_COMM_TYPE_CTRL, BML_IF_COMM_CB_INFO_TERM_REQ);
	}
}

//
// 接続コールバック(CommModuleInterface)
//
void CommMain::NotifyConnected(E_BML_IF_COMM_TYPE commType, int port)
{
	LOG("[CommMain(id=%d)] Received connected notify. (type=%d port=%d)", _id, commType, port);

	if (_isTerminating) return;

	if (commType == BML_IF_COMM_TYPE_CTRL) {
		CommCmdCtrl* cmd = CommCmdBuilder::CreateCtrlCommand();
		int i = 1;
		for (CommCtrlInfoList::iterator itr = _ctrlInfoList.begin();
			itr != _ctrlInfoList.end(); itr++, i++)
		{
			cmd->AddInfo(*itr);
			LOG("[CommMain(id=%d)] Communicate Info [%d] type=%d port=%d queue-size=%d", _id,
				i, (*itr).GetType(), (*itr).GetPort(), (*itr).GetQueSize());
		}
		_ctrlCommModule->SendCommand(cmd);
		delete cmd;
	}
}

//
// 切断コールバック(CommModuleInterface)
//
void CommMain::NotifyDisconnected(E_BML_IF_COMM_TYPE commType, int port)
{
	LOG("[CommMain(id=%d)] Received disconnected notify. (type=%d port=%d)", _id, commType, port);

	if (_isTerminating) return;

	for (std::vector<ICommListener*>::iterator itr = _pListenerList.begin();
		itr != _pListenerList.end(); itr++)
	{
		(*itr)->NotifyInfo(commType, BML_IF_COMM_CB_INFO_DISCONNECTED);
	}
	for (std::vector<ICommListenerById*>::iterator itr = _pListenerListById.begin();
		itr != _pListenerListById.end(); itr++)
	{
		(*itr)->NotifyInfo(_id, commType, BML_IF_COMM_CB_INFO_DISCONNECTED);
	}
}

//
// クライアント終了処理
//
void CommMain::_TermClient()
{
	//制御用通信モジュール以外を終了
	//（本処理はクライアントの制御用通信モジュールからのコールバックで実行されるため）
	_isTerminating = true;
	for (std::map<E_BML_IF_COMM_TYPE, CommModule*>::iterator itr = _dataCommModuleList.begin();
		itr != _dataCommModuleList.end(); itr++)
	{
		LOG("[CommMain(id=%d)] Stop. (type=%d)", _id, (*itr).first);

		(*itr).second->Stop();
		delete (*itr).second;
	}
	_dataCommModuleList.clear();
}

//
// 終了処理
//
void CommMain::_StopProc()
{
	_isTerminating = true;
	for (std::map<E_BML_IF_COMM_TYPE, CommModule*>::iterator itr = _dataCommModuleList.begin();
		itr != _dataCommModuleList.end(); itr++)
	{
		LOG("[CommMain(id=%d)] Stop. (type=%d)", _id, (*itr).first);

		(*itr).second->Stop();
		delete (*itr).second;
	}
	_dataCommModuleList.clear();

	if (_ctrlCommModule != NULL) {
		LOG("[CommMain(id=%d)] Stop. (type=%d)", _id, BML_IF_COMM_TYPE_CTRL);

		_ctrlCommModule->Stop();
		delete _ctrlCommModule;
		_ctrlCommModule = NULL;
	}
}
