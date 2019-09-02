#include "stdafx.h"
#include "CommApi.h"
#include "CommCtrl.h"

//-------- APIs -----------------------------------------------------

//ManApiSetParameterInitComm
//
//�ʐM��ʂ��Ƃ̒ʐM�|�[�g�ԍ�����уL���[�T�C�Y�̎w����s���܂��B
//
// ����
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							�ʐM��ʁi�u�萔�v�V�[�g�Q�Ɓj	
// [IN]	int					port
//							�f�[�^�ʐM�p�|�[�g�ԍ��B
//							APP��AttModule��TCP/IP�T�[�o�ƂȂ�
//							���̃|�[�g�ԍ��ő҂��󂯁ABML�u���E
//							�U���ڏ풓�A�v����BmlAppIf��TCP/IP
//							�N���C�A���g�ƂȂ�A���̃|�[�g�ԍ���
//							�ڑ����܂��B	
// [IN]	int					queueSize
//							����M�L���[�T�C�Y(��)
//							1�`
//							APP��AttModule�ABML�u���E�U���ڏ풓
//							�A�v����BmlAppIf���ɁA���̑���M�L���[
//							�T�C�Y�ŏ��������s���܂��B
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int ManApiSetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize)
{
	return CommCtrl::GetIns()->SetParameterInitComm(id,bmlIfCommType,port,queueSize);
}

//ManApiAppendListener
//
//�ʐM��������̃R�[���o�b�N�N���X�C���X�^���X��o�^���܂��B
//�������s�ŕ����̈قȂ�R�[���o�b�N�N���X�C���X�^���X��o�^�\�ł��B
//
// ����
// [IN]	ICommListener*		pListener
//							�R�[���o�b�N�N���X�C���X�^���X�o�^	
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int ManApiAppendListener(ICommListener* pListener)
{
	return CommCtrl::GetIns()->AppendListener(pListener);
}

//ManApiAppendListenerById
//
//�ʐM��������̃R�[���o�b�N�N���X�C���X�^���X��o�^���܂��B
//�������s�ŕ����̈قȂ�R�[���o�b�N�N���X�C���X�^���X��o�^�\�ł��B
//
// ����
// [IN]	ICommListener*		pListener
//							�R�[���o�b�N�N���X�C���X�^���X�o�^	
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int ManApiAppendListenerById(int id, ICommListenerById* pListener)
{
	return CommCtrl::GetIns()->AppendListener(id, pListener);
}

//ManApiRemoveListener
//
//�ʐM��������o�^�ς݂̃R�[���o�b�N�N���X�C���X�^���X���폜���܂��B
//(�N���C�A���g�p)
//
// ����		�Ȃ�
// �߂�l	�Ȃ�
//
void ManApiRemoveListener()
{
	CommCtrl::GetIns()->RemoveListener();
}

//ManApiRemoveListenerById
//
//�ʐM��������o�^�ς݂̃R�[���o�b�N�N���X�C���X�^���X���폜���܂��B
//(�T�[�o�[�p)
//
// ����
// [IN] int					id
//							�ʐM�Ώێ���ID�B�Ăяo�����ŒʐM�Ώۂ��ƂɈ�ӂɎw��B
// �߂�l	�Ȃ�
//
void ManApiRemoveListenerById(int id)
{
	CommCtrl::GetIns()->RemoveListener(id);
}

//ManApiSetParameterInitBrowser
//
//BML�u���E�U���ڏ풓�A�v���̏��݂Ɛ���p�ʐM�|�[�g�ԍ��̐ݒ��				
//�s���܂��B
//
// ����
// [IN]	const char* 		pBmlBrowserPath
//							BML�u���E�U���ڏ풓�A�v���̃t���p�X
//							(������)���w�肵�܂��B������̓k��
//							�~�߂���Ă���K�v������܂��B
// [IN]	int					port
//							�����ڑ�����ђʐM����p�|�[�g�ԍ��B
//							APP��AttModule��TCP/IP�T�[�o�ƂȂ�
//							���̃|�[�g�ԍ��ő҂��󂯁ABML�u���E
//							�U���ڏ풓�A�v����BmlAppIf��TCP/IP
//							�N���C�A���g�ƂȂ�A���̃|�[�g�ԍ�
//							�ɐڑ����܂��B	
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_ERROR		BML�u���E�U���ڏ풓�A�v�������݂��Ȃ���	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int ManApiSetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port)
{
	return CommCtrl::GetIns()->SetParameterInitBrowser(id,pBmlBrowserPath,port);
}

//ManApiStartServer
//
//BML�u���E�U���ڏ풓�A�v���̋N���ƁA���ݒʐM���J�n���܂��B				
//
// ����		�Ȃ�
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_ERROR		���s	
//
int ManApiStartServer(int id)
{
	return CommCtrl::GetIns()->StartServer(id);
}

//ManApiStartClient
//
//BML�u���E�U���ڏ풓�A�v���̋N���ƁA���ݒʐM���J�n���܂��B				
//
// ����
// [IN]	int					port
//							�����ڑ�����ђʐM����p�|�[�g�ԍ��B
//							APP��AttModule��TCP/IP�T�[�o�ƂȂ�
//							���̃|�[�g�ԍ��ő҂��󂯁ABML�u���E
//							�U���ڏ풓�A�v����BmlAppIf��TCP/IP
//							�N���C�A���g�ƂȂ�A���̃|�[�g�ԍ�
//							�ɐڑ����܂��B	
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_ERROR		���s	
//
int ManApiStartClient(int port)
{
	return CommCtrl::GetIns()->StartClient(port);
}

//ManApiStop
//
//BML�u���E�U���ڏ풓�A�v���֏I���w����^���A�ʐM���I�����܂��B				
//
// ����		�Ȃ�
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����	
//			MAN_API_RET_ERROR		���s	
//
int ManApiStop(int id)
{
	return CommCtrl::GetIns()->Stop(id);
}

//InputApiPushData
//
//�f�[�^�𑗐M���܂��B
//
// ����
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							�ʐM��ʁi�u�萔�v�V�[�g�Q�Ɓj
// [IN]	ICommData*	 		pData
//							���M�f�[�^
//							CommData�N���X�̐�������єj���͌Ă�
//							�o�����ōs���ĉ������B
//							CommData�N���X�̐�����ASetData()
//							���\�b�h�ő��M�f�[�^��ݒ肵�A������
//							�^���Ď��s���ĉ������B
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����
//			MAN_API_RET_ERROR		���s
//			MAN_API_RET_BUSY		�r�W�[
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int InputApiPushData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	return CommCtrl::GetIns()->PushData(0, bmlIfCommType, pData);
}

//InputApiPushData
//
//�f�[�^�𑗐M���܂��B
//
// ����
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							�ʐM��ʁi�u�萔�v�V�[�g�Q�Ɓj
// [IN]	ICommData*	 		pData
//							���M�f�[�^
//							CommData�N���X�̐�������єj���͌Ă�
//							�o�����ōs���ĉ������B
//							CommData�N���X�̐�����ASetData()
//							���\�b�h�ő��M�f�[�^��ݒ肵�A������
//							�^���Ď��s���ĉ������B
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����
//			MAN_API_RET_ERROR		���s
//			MAN_API_RET_BUSY		�r�W�[
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int InputApiPushDataById(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	return CommCtrl::GetIns()->PushData(id, bmlIfCommType, pData);
}

//OutputApiPullData
//
//�f�[�^����M���܂��B
//
// ����
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							�ʐM��ʁi�u�萔�v�V�[�g�Q�Ɓj
// [IN]	ICommData**	 		pData
//							��M�f�[�^
//							CommData�N���X�̐����͖{���C�u������
//							�s���܂��B
//							�j���͌Ăяo�����ōs���ĉ������B
//							�j���ɂ�ReleaseCommData() API���g�p��
//							�ĉ������B
//							��M�f�[�^���Ȃ��ꍇ��NULL��Ԃ��܂��B
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����
//			MAN_API_RET_ERROR		���s
//			MAN_API_RET_NOT_EXIST		�f�[�^�Ȃ�	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int OutputApiPullData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	return CommCtrl::GetIns()->PullData(0, bmlIfCommType, pData);
}

//OutputApiPullData
//
//�f�[�^����M���܂��B
//
// ����
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							�ʐM��ʁi�u�萔�v�V�[�g�Q�Ɓj
// [IN]	ICommData**	 		pData
//							��M�f�[�^
//							CommData�N���X�̐����͖{���C�u������
//							�s���܂��B
//							�j���͌Ăяo�����ōs���ĉ������B
//							�j���ɂ�ReleaseCommData() API���g�p��
//							�ĉ������B
//							��M�f�[�^���Ȃ��ꍇ��NULL��Ԃ��܂��B
// �߂�l	�G���[�R�[�h��Ԃ��܂��B	
//			MAN_API_RET_SUCCSESS	����
//			MAN_API_RET_ERROR		���s
//			MAN_API_RET_NOT_EXIST		�f�[�^�Ȃ�	
//			MAN_API_RET_BAD_ARG		�����ُ�
//
int OutputApiPullDataById(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	return CommCtrl::GetIns()->PullData(id, bmlIfCommType, pData);
}

//CreateCommData
//
//����M�f�[�^�C���X�^���X�𐶐����܂��B
//
// ����		�Ȃ�
// �߂�l	�Ȃ�
//
ICommData* CreateCommData()
{
	return new CommData();
}

//ReleaseCommData
//
//����M�f�[�^�C���X�^���X��������܂��B
//
// ����
// [IN]	CommData**	 		pData
//							���M�f�[�^�C���X�^���X�̃_�u���|�C���^
// �߂�l	�Ȃ�
//
void ReleaseCommData(ICommData** pData)
{
	if ((pData != NULL) && (*pData != NULL)) {
		delete *pData;
		*pData = NULL;
	}
}

//ManApiSetLogDirPath
//
//���O�̕ۑ�����w�肵�܂��B				
//
// ����
// [IN]	const char* 		pLogDirPath
//							���O�ۑ���f�B���N�g���̃t���p�X�B
//							���݂��Ȃ��ꍇ�͍쐬���܂��B
// �߂�l	�Ȃ�	
//
void ManApiSetLogDirPath(const char* pLogDirPath)
{
	CommCtrl::GetIns()->SetLogDirPath(pLogDirPath);
}