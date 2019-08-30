#pragma once

//-------- Return Codes ---------------------------------------------
#define MAN_API_RET_SUCCSESS		 0	//正常。成功。
#define MAN_API_RET_ERROR			-1	//エラー。失敗。
#define MAN_API_RET_BUSY			-2	//ビジー。
#define MAN_API_RET_BAD_ARG			-3	//引数異常。
#define MAN_API_RET_NOT_EXIST		-4	//存在しない。見つからない。
#define MAN_API_RET_INIT			-5  //初期化中。

//-------- Enums ----------------------------------------------------
typedef enum {
	BML_IF_COMM_TYPE_CTRL = 0,			//通信制御用（外部指定禁止）
	BML_IF_COMM_TYPE_UNIT_INFO = 1,		//受信機情報通知用
	BML_IF_COMM_TYPE_PLAYER_INFO,		//プレイヤー情報通知用
	BML_IF_COMM_TYPE_REMOCON_EVENT,		//リモコン状態通知用
	BML_IF_COMM_TYPE_SECTION,			//セクションデータ通知用
	BML_IF_COMM_TYPE_STC,				//STC通知用
	BML_IF_COMM_TYPE_STATUS_INFO,		//BMLブラウザからの状態受信用
	BML_IF_COMM_TYPE_AUDIO_CTRL_INFO	//音声制御情報受信用
} E_BML_IF_COMM_TYPE;

typedef enum {
	BML_IF_COMM_CB_INFO_DISCONNECTED,  //通信切断
	BML_IF_COMM_CB_INFO_TERM_REQ       //終了要求
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
