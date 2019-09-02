#include "stdafx.h"
#include "CommApi.h"
#include "CommCtrl.h"

//-------- APIs -----------------------------------------------------

//ManApiSetParameterInitComm
//
//通信種別ごとの通信ポート番号およびキューサイズの指定を行います。
//
// 引数
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							通信種別（「定数」シート参照）	
// [IN]	int					port
//							データ通信用ポート番号。
//							APP側AttModuleはTCP/IPサーバとなり
//							このポート番号で待ち受け、BMLブラウ
//							ザ搭載常駐アプリ側BmlAppIfはTCP/IP
//							クライアントとなり、このポート番号に
//							接続します。	
// [IN]	int					queueSize
//							送受信キューサイズ(数)
//							1～
//							APP側AttModule、BMLブラウザ搭載常駐
//							アプリ側BmlAppIf共に、この送受信キュー
//							サイズで初期化を行います。
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_BAD_ARG		引数異常
//
int ManApiSetParameterInitComm(int id, E_BML_IF_COMM_TYPE bmlIfCommType, int port, int queueSize)
{
	return CommCtrl::GetIns()->SetParameterInitComm(id,bmlIfCommType,port,queueSize);
}

//ManApiAppendListener
//
//通信処理からのコールバッククラスインスタンスを登録します。
//複数実行で複数の異なるコールバッククラスインスタンスを登録可能です。
//
// 引数
// [IN]	ICommListener*		pListener
//							コールバッククラスインスタンス登録	
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_BAD_ARG		引数異常
//
int ManApiAppendListener(ICommListener* pListener)
{
	return CommCtrl::GetIns()->AppendListener(pListener);
}

//ManApiAppendListenerById
//
//通信処理からのコールバッククラスインスタンスを登録します。
//複数実行で複数の異なるコールバッククラスインスタンスを登録可能です。
//
// 引数
// [IN]	ICommListener*		pListener
//							コールバッククラスインスタンス登録	
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_BAD_ARG		引数異常
//
int ManApiAppendListenerById(int id, ICommListenerById* pListener)
{
	return CommCtrl::GetIns()->AppendListener(id, pListener);
}

//ManApiRemoveListener
//
//通信処理から登録済みのコールバッククラスインスタンスを削除します。
//(クライアント用)
//
// 引数		なし
// 戻り値	なし
//
void ManApiRemoveListener()
{
	CommCtrl::GetIns()->RemoveListener();
}

//ManApiRemoveListenerById
//
//通信処理から登録済みのコールバッククラスインスタンスを削除します。
//(サーバー用)
//
// 引数
// [IN] int					id
//							通信対象識別ID。呼び出し元で通信対象ごとに一意に指定。
// 戻り値	なし
//
void ManApiRemoveListenerById(int id)
{
	CommCtrl::GetIns()->RemoveListener(id);
}

//ManApiSetParameterInitBrowser
//
//BMLブラウザ搭載常駐アプリの所在と制御用通信ポート番号の設定を				
//行います。
//
// 引数
// [IN]	const char* 		pBmlBrowserPath
//							BMLブラウザ搭載常駐アプリのフルパス
//							(文字列)を指定します。文字列はヌル
//							止めされている必要があります。
// [IN]	int					port
//							初期接続および通信制御用ポート番号。
//							APP側AttModuleはTCP/IPサーバとなり
//							このポート番号で待ち受け、BMLブラウ
//							ザ搭載常駐アプリ側BmlAppIfはTCP/IP
//							クライアントとなり、このポート番号
//							に接続します。	
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_ERROR		BMLブラウザ搭載常駐アプリが存在しない等	
//			MAN_API_RET_BAD_ARG		引数異常
//
int ManApiSetParameterInitBrowser(int id, const char* pBmlBrowserPath, int port)
{
	return CommCtrl::GetIns()->SetParameterInitBrowser(id,pBmlBrowserPath,port);
}

//ManApiStartServer
//
//BMLブラウザ搭載常駐アプリの起動と、相互通信を開始します。				
//
// 引数		なし
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_ERROR		失敗	
//
int ManApiStartServer(int id)
{
	return CommCtrl::GetIns()->StartServer(id);
}

//ManApiStartClient
//
//BMLブラウザ搭載常駐アプリの起動と、相互通信を開始します。				
//
// 引数
// [IN]	int					port
//							初期接続および通信制御用ポート番号。
//							APP側AttModuleはTCP/IPサーバとなり
//							このポート番号で待ち受け、BMLブラウ
//							ザ搭載常駐アプリ側BmlAppIfはTCP/IP
//							クライアントとなり、このポート番号
//							に接続します。	
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_ERROR		失敗	
//
int ManApiStartClient(int port)
{
	return CommCtrl::GetIns()->StartClient(port);
}

//ManApiStop
//
//BMLブラウザ搭載常駐アプリへ終了指示を与え、通信を終了します。				
//
// 引数		なし
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功	
//			MAN_API_RET_ERROR		失敗	
//
int ManApiStop(int id)
{
	return CommCtrl::GetIns()->Stop(id);
}

//InputApiPushData
//
//データを送信します。
//
// 引数
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							通信種別（「定数」シート参照）
// [IN]	ICommData*	 		pData
//							送信データ
//							CommDataクラスの生成および破棄は呼び
//							出し元で行って下さい。
//							CommDataクラスの生成後、SetData()
//							メソッドで送信データを設定し、引数に
//							与えて実行して下さい。
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功
//			MAN_API_RET_ERROR		失敗
//			MAN_API_RET_BUSY		ビジー
//			MAN_API_RET_BAD_ARG		引数異常
//
int InputApiPushData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	return CommCtrl::GetIns()->PushData(0, bmlIfCommType, pData);
}

//InputApiPushData
//
//データを送信します。
//
// 引数
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							通信種別（「定数」シート参照）
// [IN]	ICommData*	 		pData
//							送信データ
//							CommDataクラスの生成および破棄は呼び
//							出し元で行って下さい。
//							CommDataクラスの生成後、SetData()
//							メソッドで送信データを設定し、引数に
//							与えて実行して下さい。
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功
//			MAN_API_RET_ERROR		失敗
//			MAN_API_RET_BUSY		ビジー
//			MAN_API_RET_BAD_ARG		引数異常
//
int InputApiPushDataById(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData* pData)
{
	return CommCtrl::GetIns()->PushData(id, bmlIfCommType, pData);
}

//OutputApiPullData
//
//データを受信します。
//
// 引数
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							通信種別（「定数」シート参照）
// [IN]	ICommData**	 		pData
//							受信データ
//							CommDataクラスの生成は本ライブラリで
//							行います。
//							破棄は呼び出し元で行って下さい。
//							破棄にはReleaseCommData() APIを使用し
//							て下さい。
//							受信データがない場合はNULLを返します。
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功
//			MAN_API_RET_ERROR		失敗
//			MAN_API_RET_NOT_EXIST		データなし	
//			MAN_API_RET_BAD_ARG		引数異常
//
int OutputApiPullData(E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	return CommCtrl::GetIns()->PullData(0, bmlIfCommType, pData);
}

//OutputApiPullData
//
//データを受信します。
//
// 引数
// [IN]	E_BML_IF_COMM_TYPE	bmlIfCommType
//							通信種別（「定数」シート参照）
// [IN]	ICommData**	 		pData
//							受信データ
//							CommDataクラスの生成は本ライブラリで
//							行います。
//							破棄は呼び出し元で行って下さい。
//							破棄にはReleaseCommData() APIを使用し
//							て下さい。
//							受信データがない場合はNULLを返します。
// 戻り値	エラーコードを返します。	
//			MAN_API_RET_SUCCSESS	成功
//			MAN_API_RET_ERROR		失敗
//			MAN_API_RET_NOT_EXIST		データなし	
//			MAN_API_RET_BAD_ARG		引数異常
//
int OutputApiPullDataById(int id, E_BML_IF_COMM_TYPE bmlIfCommType, ICommData** pData)
{
	return CommCtrl::GetIns()->PullData(id, bmlIfCommType, pData);
}

//CreateCommData
//
//送受信データインスタンスを生成します。
//
// 引数		なし
// 戻り値	なし
//
ICommData* CreateCommData()
{
	return new CommData();
}

//ReleaseCommData
//
//送受信データインスタンスを解放します。
//
// 引数
// [IN]	CommData**	 		pData
//							送信データインスタンスのダブルポインタ
// 戻り値	なし
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
//ログの保存先を指定します。				
//
// 引数
// [IN]	const char* 		pLogDirPath
//							ログ保存先ディレクトリのフルパス。
//							存在しない場合は作成します。
// 戻り値	なし	
//
void ManApiSetLogDirPath(const char* pLogDirPath)
{
	CommCtrl::GetIns()->SetLogDirPath(pLogDirPath);
}