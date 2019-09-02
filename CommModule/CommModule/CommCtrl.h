#pragma once
#include <map>
#include <Util/Logger.h>
#include <Util/Mutex.h>
#include "CommMain.h"

class CommCtrl
{
public:
	// インスタンス取得
	static CommCtrl* GetIns();
	// 通信ごとのパラメータ設定
	int SetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
	// リスナー登録
	int AppendListener(ICommListener* pListener);
	// リスナー登録(ID指定)
	int AppendListener(int id, ICommListenerById* pListener);
	// リスナー削除
	void RemoveListener();
	// リスナー削除(ID指定)
	void RemoveListener(int id);
	// クライアント初期設定（プロセスパスと制御ポート）
	int SetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port);
	// サーバ通信開始
	int StartServer(int id);
	// クライアント通信開始
	int StartClient(int port);
	// 通信停止
	int Stop(int id);
	// データ入力(送信)
	int PushData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
	// データ出力(受信)
	int PullData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
	// ログ保存先ディレクトリパス設定
	void SetLogDirPath(const char* pLogDirPath);

private:
	static CommCtrl _myInstance;					// 自インスタンス
	std::map<int, CommMain*> _commMainMap;			// CommMainオブジェクトマップ(key:id)
	Mutex _locker;
	bool _term;
	void Lock() {
		_locker.Lock();
	}
	void Unlock() {
		_locker.Unlock();
	}

	CommMain* GetCommMain(int id);
	void DestroyCommMain(int id);

	// コンストラクタ
	CommCtrl();
	// コピーコンストラクタ
	CommCtrl(CommCtrl& obj) {};
	// デストラクタ
	virtual ~CommCtrl();
};

