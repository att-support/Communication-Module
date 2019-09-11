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
// �ʐM���䃁�C��
//
// �V���O���g���p�^�[������
// �e�ʐM�̊J�n/��~�A����ъǗ����s��

class CommMain : CommModuleInterface
{
public:
	// �R���X�g���N�^
	CommMain(int id);
	// �f�X�g���N�^
	virtual ~CommMain();

	// �ʐM���Ƃ̃p�����[�^�ݒ�
	int SetParameterInitComm(E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
	// ���X�i�[�o�^
	int AppendListener(ICommListener* pListener);
	// ���X�i�[�o�^
	int AppendListener(ICommListenerById* pListener);
	// ���X�i�[�폜
	void RemoveListener();
	// ���X�i�[�폜(ID�w��)
	void RemoveListenerById();
	// �N���C�A���g�����ݒ�i�v���Z�X�p�X�Ɛ���|�[�g�j
	int SetParameterInitBrowser(const char* pBmlBrowserPath, int port);
	// �T�[�o�ʐM�J�n
	int StartServer();
	// �N���C�A���g�ʐM�J�n
	int StartClient(int port);
	// �ʐM��~
	int Stop();
	// �f�[�^����(���M)
	int PushData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
	// �f�[�^�o��(��M)
	int PullData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);

	// �ʐM�������M�R�[���o�b�N(CommModuleInterface)
	void NotifyCtrlInfo(CommCtrlInfoList& ctrlInfoList);
	// ��Ԓʒm��M�R�[���o�b�N(CommModuleInterface)
	void NotifyStat(bool isBusy);
	// �I���ʒm��M�R�[���o�b�N(CommModuleInterface)
	void NotifyTerm();
	// �ڑ��R�[���o�b�N(CommModuleInterface)
	void NotifyConnected(E_BML_IF_COMM_TYPE commType, int port);
	// �ؒf�R�[���o�b�N(CommModuleInterface)
	void NotifyDisconnected(E_BML_IF_COMM_TYPE commType, int port);

private:
	int _id;										// ID
	CommCtrlInfoList _ctrlInfoList;					// �ʐM������
	std::vector<ICommListener*> _pListenerList;		// ���X�i�[
	std::vector<ICommListenerById*> _pListenerListById;		// ���X�i�[
	std::string _bmlBrowserPath;					// �N���C�A���g�v���Z�X�t�@�C���p�X
	std::string _logDirPath;						// ���O�o�͐�f�B���N�g���p�X
	int _ctrlPort;									// ����p�|�[�g
	CommModule* _ctrlCommModule;					// ����p�ʐM���W���[��
	std::map<E_BML_IF_COMM_TYPE, CommModule*> _dataCommModuleList;
													// �f�[�^�ʐM�p�ʐM���W���[�����X�g(Key:�ʐM���)
	bool _isServer;									// �T�[�o�N������ (true:�T�[�o�N��/false:�N���C�A���g�N��)
	bool _isTerminating;							// �I������������ (true:�I��������)

	// �I������
	void _StopProc();
	// �N���C�A���g�I������
	void _TermClient();

};

