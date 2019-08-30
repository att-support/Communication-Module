#pragma once

//-------- Return Codes ---------------------------------------------
#define MAN_API_RET_SUCCSESS		 0	//����B�����B
#define MAN_API_RET_ERROR			-1	//�G���[�B���s�B
#define MAN_API_RET_BUSY			-2	//�r�W�[�B
#define MAN_API_RET_BAD_ARG			-3	//�����ُ�B
#define MAN_API_RET_NOT_EXIST		-4	//���݂��Ȃ��B������Ȃ��B
#define MAN_API_RET_INIT			-5  //���������B

//-------- Enums ----------------------------------------------------
typedef enum {
	BML_IF_COMM_TYPE_CTRL = 0,			//�ʐM����p�i�O���w��֎~�j
	BML_IF_COMM_TYPE_UNIT_INFO = 1,		//��M�@���ʒm�p
	BML_IF_COMM_TYPE_PLAYER_INFO,		//�v���C���[���ʒm�p
	BML_IF_COMM_TYPE_REMOCON_EVENT,		//�����R����Ԓʒm�p
	BML_IF_COMM_TYPE_SECTION,			//�Z�N�V�����f�[�^�ʒm�p
	BML_IF_COMM_TYPE_STC,				//STC�ʒm�p
	BML_IF_COMM_TYPE_STATUS_INFO,		//BML�u���E�U����̏�Ԏ�M�p
	BML_IF_COMM_TYPE_AUDIO_CTRL_INFO	//�����������M�p
} E_BML_IF_COMM_TYPE;

typedef enum {
	BML_IF_COMM_CB_INFO_DISCONNECTED,  //�ʐM�ؒf
	BML_IF_COMM_CB_INFO_TERM_REQ       //�I���v��
} E_BML_IF_COMM_CB_INFO;

//-------- Interface ------------------------------------------------
class ICommListener {
public:
	ICommListener() {};
	virtual ~ICommListener() {};
	virtual void NotifyInfo(E_BML_IF_COMM_TYPE bmlIfCommType,
		E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo) = 0;
};

class ICommListenerById {
public:
	ICommListenerById() {};
	virtual ~ICommListenerById() {};
	virtual void NotifyInfo(int id,
		E_BML_IF_COMM_TYPE bmlIfCommType,
		E_BML_IF_COMM_CB_INFO bmlIfCommCbInfo) = 0;
};
