#pragma once

#include <Util/Thread.h>
#include <Tcp/TcpReceiveInterface.h>
#include "CommDataQue.h"
#include "CommCommand.h"
#include "CommDef.h"
#include "CommModuleInterface.h"

//
// �ʐM���W���[��
//
// ���X���b�h�@�\�p���ETCP/IP����M�@�\�p��
// ���������z�֐��ۗL�iCommModuleClient�܂���CommModuleServer�Ƃ��ė��p�j

class CommModule : public Thread, public TcpReceiveInterface
{
public:
	// �R���X�g���N�^
	CommModule(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize);
	// �f�X�g���N�^
	virtual ~CommModule();
	// �ʐM�J�n
	int Start();
	// �ʐM��~
	int Stop();
	// �d�����M
	int SendCommand(CommCommand* cmd);
	// ���M�f�[�^�~��(�G���L���[)
	int PushData(ICommData* pData);
	// ��M�f�[�^�擾(�f�L���[)
	int PullData(ICommData** pData);
	// �I�[�i�[(CommCtrl)�ݒ�
	void SetOwner(CommModuleInterface* owner);

protected:
	CommDataQue* _sendQue;					// ���M�L���[
	CommDataQue* _receiveQue;				// ��M�L���[
	E_BML_IF_COMM_TYPE _bmlIfCommType;		// �N���C�A���g�v���Z�X�t�@�C���p�X
	int _queueSize;							// ����M�L���[�T�C�Y(��)
	ICommListener* _pListener;				// ���X�i�[
	CommCommand* _tmpCmd;					// ��M�d���e���|����(�w�b�_�i�[�p)
	int _commPort;							// �ʐM�|�[�g
	CommModuleInterface* _owner;			// �I�[�i�[(CommCtrl)
	bool _isWaitingReply;					// ������M�҂�
	bool _isSendBusy;						// ���M�r�W�[��� (true:�r�W�[)
	bool _isReceiveBusy;					// ��M�r�W�[��� (true:�r�W�[)
	bool _isConnected;						// �ڑ���� (true:�ڑ���)
	bool _busyOffSendFlag;					// �r�W�[������Ԓʒm���M�t���O
	std::string _logName;					// ���O�o�͎��̃v���t�B�N�X����
	int _sendCounter;						// ���M�J�E���^�[�i���񑗐M���ƂɃX���b�h�����ɃX���[�v��}�����邽�߁j

	// ���M�����i�������z�֐��j
	virtual int Send(const unsigned char* cmdData, int cmdLen) = 0;

	//
	// Thread class override methods
	//
	// �X���b�h����
	virtual int ThreadProc();
	// �X���b�h�O����
	virtual int ThreadInitProc();
	// �X���b�h�㏈��
	virtual int ThreadTermProc();

	//
	// TcpReceiveInterface override methods
	//
	// TCP/IP��M�R�[���o�b�N
	virtual int ReceivedData(const unsigned char* data, int dataSize, const std::string& ipAddr);
	// TCP/IP�N���C�A���g�ڑ��R�[���o�b�N
	virtual void Connected(const std::string& ipAddr);
	// TCP/IP�T�[�o�ڑ��R�[���o�b�N�i���g�p�j
	virtual void ConnectedToServer(const std::string& ipAddr, int port) {};
	// TCP/IP�ؒf�R�[���o�b�N
	virtual void Disconnected(const std::string& ipAddr, int port);
};


