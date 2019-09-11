#pragma once
#include "CommModule.h"
#include <Tcp/TcpServer.h>

//
// �ʐM���W���[���i�T�[�o�j
//

class CommModuleServer : public CommModule
{
public:
	// �R���X�g���N�^
	CommModuleServer(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, int port);
	// �f�X�g���N�^
	virtual ~CommModuleServer();

private:
	TcpServer* _tcpServer;			// TCP/IP�T�[�o

	//
	// CommModule class override methods
	//
	// ���M����
	int Send(const unsigned char* cmdData, int cmdLen);
	// TCP/IP�ʐM��~����
	int TcpStop();

	//
	// Thread class override methods
	//
	// �X���b�h�O����
	int ThreadInitProc();
	// �X���b�h�㏈��
	int ThreadTermProc();

};

