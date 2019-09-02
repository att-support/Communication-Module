#include "stdafx.h"
#include "CommCtrl.h"

CommCtrl CommCtrl::_myInstance;

//
// �ʐM���䃁�C��
//
// �V���O���g���p�^�[������


//
// �C���X�^���X�擾
//
CommCtrl* CommCtrl::GetIns() {
	return &_myInstance;
}

//
// �R���X�g���N�^�i�B���j
//
CommCtrl::CommCtrl()
{
	_term = false;
}

//
// �f�X�g���N�^�i�B���j
//
CommCtrl::~CommCtrl()
{
	for (std::map<int, CommMain*>::iterator itr = _commMainMap.begin();
		itr != _commMainMap.end(); itr++)
	{
		delete (*itr).second;
	}
	_commMainMap.clear();
	CommLogger::Destroy();
}

//
// �ʐM���Ƃ̃p�����[�^�ݒ�
//
int CommCtrl::SetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize)
{
	LOG("[CommCtrl] Set parameter. (id=%d type=%d port=%d queue-size=%d)", id, bmlIfCommType, port, queueSize);
	return GetCommMain(id)->SetParameterInitComm(bmlIfCommType, port, queueSize);
}

//
// ���X�i�[�o�^
//
int CommCtrl::AppendListener(ICommListener* pListener)
{
	LOG("[CommCtrl] Set listener.");
	return GetCommMain(0)->AppendListener(pListener);
}

//
// ���X�i�[�o�^(ID�w��)
//
int CommCtrl::AppendListener(int id, ICommListenerById* pListener)
{
	LOG("[CommCtrl] Set listener. (id=%d)", id);
	return GetCommMain(id)->AppendListener(pListener);
}

//
// ���X�i�[�폜
//
void CommCtrl::RemoveListener()
{
	LOG("[CommCtrl] Remove listener.");
	return GetCommMain(0)->RemoveListener();
}

//
// ���X�i�[�폜(ID�w��)
//
void CommCtrl::RemoveListener(int id)
{
	LOG("[CommCtrl] Remove listener.");
	return GetCommMain(id)->RemoveListenerById();
}

//
// �N���C�A���g�����ݒ�i�v���Z�X�p�X�Ɛ���|�[�g�j
//
int CommCtrl::SetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port)
{
	LOG("[CommCtrl] Set process file path. (id=%d path=%s control-port=%d)", id, pBmlBrowserPath, port);
	return GetCommMain(id)->SetParameterInitBrowser(pBmlBrowserPath, port);
}

//
// �T�[�o�ʐM�J�n
//
int CommCtrl::StartServer(int id)
{
	_term = false;
	LOG("[CommCtrl] Start server. (id=%d)", id);
	return GetCommMain(id)->StartServer();
}

//
// �N���C�A���g�ʐM�J�n
//
int CommCtrl::StartClient(int port)
{
	LOG("[CommCtrl] Start client. (port=%d)", port);
	return GetCommMain(0)->StartClient(port);
}

//
// �ʐM��~
//
int CommCtrl::Stop(int id)
{
	_term = true;
	LOG("[CommCtrl] Stop. (id=%d)", id);
	int retCode = GetCommMain(id)->Stop();
	DestroyCommMain(id);
	return retCode;
}

//
// �f�[�^����(���M)
//
int CommCtrl::PushData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	if (_term) return MAN_API_RET_INIT;
#ifdef _DEBUG_LOG_
	LOG("[CommCtrl] Push data. (id-%d type=%d)", id, bmlIfCommType);
#endif
	return GetCommMain(id)->PushData(bmlIfCommType, pData);
}

//
// �f�[�^�o��(��M)
//
int CommCtrl::PullData(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	if (_term) return MAN_API_RET_NOT_EXIST;
#ifdef _DEBUG_LOG_
	LOG("[CommCtrl] Pull data. (id=%d type=%d)", id, bmlIfCommType);
#endif
	return GetCommMain(id)->PullData(bmlIfCommType, pData);
}

//
// ���O�ۑ���f�B���N�g���p�X�ݒ�
//
void CommCtrl::SetLogDirPath(const char* pLogDirPath)
{
	CommLogger::Init("_COMM.log", pLogDirPath);
}

CommMain* CommCtrl::GetCommMain(int id)
{
	CommMain* commMain = NULL;
	Lock();
	if (_commMainMap.find(id) == _commMainMap.end()) {
		commMain = new CommMain(id);
		_commMainMap[id] = commMain;
	}
	else {
		commMain = _commMainMap[id];
	}
	Unlock();
	return commMain;
}

void CommCtrl::DestroyCommMain(int id)
{
	Lock();
	if (_commMainMap.find(id) != _commMainMap.end()) {
		delete _commMainMap[id];
		_commMainMap.erase(id);
	}
	Unlock();
}

