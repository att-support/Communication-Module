#pragma once
#include "CommModule.h"
#include <Tcp/TcpClient.h>

//
// 通信モジュール（クライアント）
//

class CommModuleClient : public CommModule
{
public:
	// コンストラクタ
	CommModuleClient(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, const std::string& ipAddr, int port);
	// デストラクタ
	virtual ~CommModuleClient();

private:
	TcpClient* _tcpClient;		// TCP/IPクライアント

	//
	// CommModule class override methods
	//
	// 送信処理
	int Send(const unsigned char* cmdData, int cmdLen);

	//
	// Thread class override methods
	//
	// スレッド前処理
	int ThreadInitProc();
	// スレッド後処理
	int ThreadTermProc();
};

