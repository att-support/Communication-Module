#pragma once
#include "CommModule.h"
#include <Tcp/TcpClient.h>

//
// �ʐM���W���[���i�N���C�A���g�j
//

class CommModuleClient : public CommModule
{
public:
	// �R���X�g���N�^
	CommModuleClient(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, const std::string& ipAddr, int port);
	// �f�X�g���N�^
	virtual ~CommModuleClient();

private:
	TcpClient* _tcpClient;		// TCP/IP�N���C�A���g

	//
	// CommModule class override methods
	//
	// ���M����
	int Send(const unsigned char* cmdData, int cmdLen);

	//
	// Thread class override methods
	//
	// �X���b�h�O����
	int ThreadInitProc();
	// �X���b�h�㏈��
	int ThreadTermProc();
};

