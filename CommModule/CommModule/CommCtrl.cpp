#include "stdafx.h"
#include "CommCtrl.h"

CommCtrl CommCtrl::_myInstance;

//
// 通信制御メイン
//
// シングルトンパターン実装


//
// インスタンス取得
//
CommCtrl* CommCtrl::GetIns() {
	return &_myInstance;
}

//
// コンストラクタ（隠蔽）
//
CommCtrl::CommCtrl()
{
	_term = false;
}

//
// デストラクタ（隠蔽）
//
CommCtrl::~CommCtrl()
{
	for (std::map<int, CommMain*>::iterator itr = _commMainMap.begin();
		itr != _commMainMap.end(); itr++)
	{
		delete (*itr).second;
	}
	_commMainMap.clear();
	CommLogger::Destroy();
}

//
// 通信ごとのパラメータ設定
//
int CommCtrl::SetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize)
{
	LOG("[CommCtrl] Set parameter. (id=%d type=%d port=%d queue-size=%d)", id, bmlIfCommType, port, queueSize);
	return GetCommMain(id)->SetParameterInitComm(bmlIfCommType, port, queueSize);
}

//
// リスナー登録
//
int CommCtrl::AppendListener(ICommListener* pListener)
{
	LOG("[CommCtrl] Set listener.");
	return GetCommMain(0)->AppendListener(pListener);
}

//
// リスナー登録(ID指定)
//
int CommCtrl::AppendListener(int id, ICommListenerById* pListener)
{
	LOG("[CommCtrl] Set listener. (id=%d)", id);
	return GetCommMain(id)->AppendListener(pListener);
}

//
// リスナー削除
//
void CommCtrl::RemoveListener()
{
	LOG("[CommCtrl] Remove listener.");
	return GetCommMain(0)->RemoveListener();
}

//
// リスナー削除(ID指定)
//
void CommCtrl::RemoveListener(int id)
{
	LOG("[CommCtrl] Remove listener.");
	return GetCommMain(id)->RemoveListenerById();
}

//
// クライアント初期設定（プロセスパスと制御ポート）
//
int CommCtrl::SetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port)
{
	LOG("[CommCtrl] Set process file path. (id=%d path=%s control-port=%d)", id, pBmlBrowserPath, port);
	return GetCommMain(id)->SetParameterInitBrowser(pBmlBrowserPath, port);
}

//
// サーバ通信開始
//
int CommCtrl::StartServer(int id)
{
	_term = false;
	LOG("[CommCtrl] Start server. (id=%d)", id);
	return GetCommMain(id)->StartServer();
}

//
// クライアント通信開始
//
int CommCtrl::StartClient(int port)
{
	LOG("[CommCtrl] Start client. (port=%d)", port);
	return GetCommMain(0)->StartClient(port);
}

//
// 通信停止
//
int CommCtrl::Stop(int id)
{
	_term = true;
	LOG("[CommCtrl] Stop. (id=%d)", id);
	int retCode = GetCommMain(id)->Stop();
	DestroyCommMain(id);
	return retCode;
}

//
// データ入力(送信)
//
int CommCtrl::PushData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	if (_term) return MAN_API_RET_INIT;
#ifdef _DEBUG_LOG_
	LOG("[CommCtrl] Push data. (id-%d type=%d)", id, bmlIfCommType);
#endif
	return GetCommMain(id)->PushData(bmlIfCommType, pData);
}

//
// データ出力(受信)
//
int CommCtrl::PullData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	if (_term) return MAN_API_RET_NOT_EXIST;
#ifdef _DEBUG_LOG_
	LOG("[CommCtrl] Pull data. (id=%d type=%d)", id, bmlIfCommType);
#endif
	return GetCommMain(id)->PullData(bmlIfCommType, pData);
}

//
// ログ保存先ディレクトリパス設定
//
void CommCtrl::SetLogDirPath(const char* pLogDirPath)
{
	CommLogger::Init("_COMM.log", pLogDirPath);
}

CommMain* CommCtrl::GetCommMain(int id)
{
	CommMain* commMain = NULL;
	Lock();
	if (_commMainMap.find(id) == _commMainMap.end()) {
		commMain = new CommMain(id);
		_commMainMap[id] = commMain;
	}
	else {
		commMain = _commMainMap[id];
	}
	Unlock();
	return commMain;
}

void CommCtrl::DestroyCommMain(int id)
{
	Lock();
	if (_commMainMap.find(id) != _commMainMap.end()) {
		delete _commMainMap[id];
		_commMainMap.erase(id);
	}
	Unlock();
}

