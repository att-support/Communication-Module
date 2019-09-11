#pragma once
#include "CommModule.h"
#include <Tcp/TcpServer.h>

//
// 通信モジュール（サーバ）
//

class CommModuleServer : public CommModule
{
public:
	// コンストラクタ
	CommModuleServer(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, int port);
	// デストラクタ
	virtual ~CommModuleServer();

private:
	TcpServer* _tcpServer;			// TCP/IPサーバ

	//
	// CommModule class override methods
	//
	// 送信処理
	int Send(const unsigned char* cmdData, int cmdLen);
	// TCP/IP通信停止処理
	int TcpStop();

	//
	// Thread class override methods
	//
	// スレッド前処理
	int ThreadInitProc();
	// スレッド後処理
	int ThreadTermProc();

};

