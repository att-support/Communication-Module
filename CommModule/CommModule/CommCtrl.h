#pragma once
#include <map>
#include <Util/Logger.h>
#include <Util/Mutex.h>
#include "CommMain.h"

class CommCtrl
{
public:
	// �C���X�^���X�擾
	static CommCtrl* GetIns();
	// �ʐM���Ƃ̃p�����[�^�ݒ�
	int SetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize);
	// ���X�i�[�o�^
	int AppendListener(ICommListener* pListener);
	// ���X�i�[�o�^(ID�w��)
	int AppendListener(int id, ICommListenerById* pListener);
	// ���X�i�[�폜
	void RemoveListener();
	// ���X�i�[�폜(ID�w��)
	void RemoveListener(int id);
	// �N���C�A���g�����ݒ�i�v���Z�X�p�X�Ɛ���|�[�g�j
	int SetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port);
	// �T�[�o�ʐM�J�n
	int StartServer(int id);
	// �N���C�A���g�ʐM�J�n
	int StartClient(int port);
	// �ʐM��~
	int Stop(int id);
	// �f�[�^����(���M)
	int PushData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData);
	// �f�[�^�o��(��M)
	int PullData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData);
	// ���O�ۑ���f�B���N�g���p�X�ݒ�
	void SetLogDirPath(const char* pLogDirPath);

private:
	static CommCtrl _myInstance;					// ���C���X�^���X
	std::map<int, CommMain*> _commMainMap;			// CommMain�I�u�W�F�N�g�}�b�v(key:id)
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

	// �R���X�g���N�^
	CommCtrl();
	// �R�s�[�R���X�g���N�^
	CommCtrl(CommCtrl& obj) {};
	// �f�X�g���N�^
	virtual ~CommCtrl();
};

