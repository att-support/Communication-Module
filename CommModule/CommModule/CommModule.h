#pragma once

#include <Util/Thread.h>
#include <Tcp/TcpReceiveInterface.h>
#include "CommDataQue.h"
#include "CommCommand.h"
#include "CommDef.h"
#include "CommModuleInterface.h"

//
// 通信モジュール
//
// ※スレッド機能継承・TCP/IP送受信機能継承
// ※純粋仮想関数保有（CommModuleClientまたはCommModuleServerとして利用）

class CommModule : public Thread, public TcpReceiveInterface
{
public:
	// コンストラクタ
	CommModule(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize);
	// デストラクタ
	virtual ~CommModule();
	// 通信開始
	int Start();
	// 通信停止
	int Stop();
	// 電文送信
	int SendCommand(CommCommand* cmd);
	// 送信データ蓄積(エンキュー)
	int PushData(ICommData* pData);
	// 受信データ取得(デキュー)
	int PullData(ICommData** pData);
	// オーナー(CommCtrl)設定
	void SetOwner(CommModuleInterface* owner);

protected:
	CommDataQue* _sendQue;					// 送信キュー
	CommDataQue* _receiveQue;				// 受信キュー
	E_BML_IF_COMM_TYPE _bmlIfCommType;		// クライアントプロセスファイルパス
	int _queueSize;							// 送受信キューサイズ(個)
	ICommListener* _pListener;				// リスナー
	CommCommand* _tmpCmd;					// 受信電文テンポラリ(ヘッダ格納用)
	int _commPort;							// 通信ポート
	CommModuleInterface* _owner;			// オーナー(CommCtrl)
	bool _isWaitingReply;					// 応答受信待ち
	bool _isSendBusy;						// 送信ビジー状態 (true:ビジー)
	bool _isReceiveBusy;					// 受信ビジー状態 (true:ビジー)
	bool _isConnected;						// 接続状態 (true:接続中)
	bool _busyOffSendFlag;					// ビジー解除状態通知送信フラグ
	std::string _logName;					// ログ出力時のプリフィクス名称
	int _sendCounter;						// 送信カウンター（一定回送信ごとにスレッド処理にスリープを挿入するため）

	// 送信処理（純粋仮想関数）
	virtual int Send(const unsigned char* cmdData, int cmdLen) = 0;

	//
	// Thread class override methods
	//
	// スレッド処理
	virtual int ThreadProc();
	// スレッド前処理
	virtual int ThreadInitProc();
	// スレッド後処理
	virtual int ThreadTermProc();

	//
	// TcpReceiveInterface override methods
	//
	// TCP/IP受信コールバック
	virtual int ReceivedData(const unsigned char* data, int dataSize, const std::string& ipAddr);
	// TCP/IPクライアント接続コールバック
	virtual void Connected(const std::string& ipAddr);
	// TCP/IPサーバ接続コールバック（未使用）
	virtual void ConnectedToServer(const std::string& ipAddr, int port) {};
	// TCP/IP切断コールバック
	virtual void Disconnected(const std::string& ipAddr, int port);
};


