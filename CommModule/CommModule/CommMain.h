#pragma once

#include "CommData.h"
#include "CommDef.h"
#include "CommModule.h"
#include "CommModuleInterface.h"
#include "CommCtrlInfo.h"
#include <string>
#include <vector>
#include <map>

//
// 通信制御メイン
//
// シングルトンパターン実装
// 各通信の開始/停止、および管理を行う

class CommMain : CommModuleInterface
{
public:
	// コンストラクタ
	CommMain(int id);
	// デストラクタ
	virtual ~CommMain();

	// 通信ごとのパラメータ設定
	int SetParameterInitComm(E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
	// リスナー登録
	int AppendListener(ICommListener* pListener);
	// リスナー登録
	int AppendListener(ICommListenerById* pListener);
	// リスナー削除
	void RemoveListener();
	// リスナー削除(ID指定)
	void RemoveListenerById();
	// クライアント初期設定（プロセスパスと制御ポート）
	int SetParameterInitBrowser(const char* pBmlBrowserPath, int port);
	// サーバ通信開始
	int StartServer();
	// クライアント通信開始
	int StartClient(int port);
	// 通信停止
	int Stop();
	// データ入力(送信)
	int PushData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
	// データ出力(受信)
	int PullData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);

	// 通信制御情報受信コールバック(CommModuleInterface)
	void NotifyCtrlInfo(CommCtrlInfoList& ctrlInfoList);
	// 状態通知受信コールバック(CommModuleInterface)
	void NotifyStat(bool isBusy);
	// 終了通知受信コールバック(CommModuleInterface)
	void NotifyTerm();
	// 接続コールバック(CommModuleInterface)
	void NotifyConnected(E_BML_IF_COMM_TYPE commType, int port);
	// 切断コールバック(CommModuleInterface)
	void NotifyDisconnected(E_BML_IF_COMM_TYPE commType, int port);

private:
	int _id;										// ID
	CommCtrlInfoList _ctrlInfoList;					// 通信制御情報
	std::vector<ICommListener*> _pListenerList;		// リスナー
	std::vector<ICommListenerById*> _pListenerListById;		// リスナー
	std::string _bmlBrowserPath;					// クライアントプロセスファイルパス
	std::string _logDirPath;						// ログ出力先ディレクトリパス
	int _ctrlPort;									// 制御用ポート
	CommModule* _ctrlCommModule;					// 制御用通信モジュール
	std::map<E_BML_IF_COMM_TYPE, CommModule*> _dataCommModuleList;
													// データ通信用通信モジュールリスト(Key:通信種別)
	bool _isServer;									// サーバ起動判定 (true:サーバ起動/false:クライアント起動)
	bool _isTerminating;							// 終了処理中判定 (true:終了処理中)

	// 終了処理
	void _StopProc();
	// クライアント終了処理
	void _TermClient();

};

