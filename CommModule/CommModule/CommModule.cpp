// CommModule.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <Util/Logger.h>
#include "CommModule.h"
#include "CommCmdBuilder.h"

//
// 通信モジュール
//

//
// コンストラクタ
//
CommModule::CommModule(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize)
{
	_bmlIfCommType = bmlIfCommType;
	_queueSize = queueSize;
	_sendQue = new CommDataQue(_queueSize);
	_receiveQue = new CommDataQue(_queueSize);
	_tmpCmd = NULL;
	_commPort = 0;
	_isWaitingReply = false;
	_isSendBusy = false;
	_isReceiveBusy = false;
	_busyOffSendFlag = false;
	_isConnected = false;
	_owner = NULL;
	_logName = "[CommModule(type=Unknown port:Unknown)] ";
	_sendCounter = 0;
	_threadLoopIntervalMsec = 0;
	_term = false;
}

//
// デストラクタ
//
CommModule::~CommModule()
{
	delete _sendQue;
	delete _receiveQue;
}

//
// 通信開始
//
int CommModule::Start()
{
	_term = false;
	return StartThread();
}

//
// 通信停止指示
//
int CommModule::Stop()
{
	_term = true;
	TcpStop();
	StopThreadAsync();
	return 0;
}

//
// 通信停止待ち
//
void CommModule::WaitStop()
{
	WaitThreadEnd();
}

//
// 電文送信
//
int CommModule::SendCommand(CommCommand* cmd)
{
	unsigned char* buf = NULL;
	int bufLen;
	int result = 0;

	// 電文作成
	cmd->MakeCommand(&buf, bufLen);
	if ((buf != NULL) && (bufLen > 0)) {
		// 送信
		if (Send(buf, bufLen) == 0)
		{
#ifdef _DEBUG_LOG_
			LOG(_logName + "Send. (type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
#endif
		}
		else {
			LOG(_logName + "[ERROR] Send failed. (type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
			result = -1;
		}
		delete[] buf;
	}
	else {
		LOG(_logName + "[ERROR] Send failed. Makeing command error. (type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
		result = -1;
	}

	return result;
}

//
// 送信データ蓄積(エンキュー)
//
int CommModule::PushData(ICommData* pData)
{
	if (_term) return MAN_API_RET_INIT;

	// データNULLチェック
	if (pData == NULL) {
		LOG(_logName + "[ERROR] Push data failed. (Data is NULL)");
		return MAN_API_RET_ERROR;
	}
	// 未接続
	if (!_isConnected) {
		LOG(_logName + "[WARN]  Push data failed. (Now initializing.)");
		return MAN_API_RET_INIT;
	}
	// キューフル
	if (_sendQue->IsFull()) {
		LOG(_logName + "[BUSY]  Push data failed. (Queue is full.(queue-count=%d))", _sendQue->Count());
		return MAN_API_RET_BUSY;
	}
	// 送信先ビジー
	if (_isSendBusy) {
		LOG(_logName + "[BUSY]  Push data failed. (Destination is busy)");
		return MAN_API_RET_BUSY;
	}

	// データのクローン生成
	ICommData* clone;
	clone = pData->Clone();
	// エンキュー
	int result = _sendQue->Add(clone);
	if (result != MAN_API_RET_SUCCSESS) {
		LOG(_logName + "[ERROR] Push data failed.");
		delete clone;
		return result;
	}
#ifdef _DEBUG_LOG_
	LOG(_logName + "Push data. (Enqueue) (send queue count=%d)", _sendQue->Count());
#endif
	return MAN_API_RET_SUCCSESS;
}

//
// 受信データ取得(デキュー)
//
int CommModule::PullData(ICommData** pData)
{
	if (_term) return MAN_API_RET_NOT_EXIST;

	// データNULLチェック
	if (pData == NULL) {
		LOG(_logName + "[ERROR] Pull data failed. (Data is NULL)");
		return MAN_API_RET_ERROR;
	}
	if (_receiveQue == NULL) {
		LOG(_logName + "[DEBUG] Pull data failed. (Receive queue is NULL)");
		return MAN_API_RET_NOT_EXIST;
	}
	// データなし
	if (_receiveQue->IsEmpty()) {
		return MAN_API_RET_NOT_EXIST;
	}

	// デキュー
	*pData = _receiveQue->GetTop();
	if (*pData == NULL) {
		return MAN_API_RET_NOT_EXIST;
	}
#ifdef _DEBUG_LOG_
	LOG(_logName + "Pull data. (Dequeue) (receive queue count=%d)", _receiveQue->Count());
#endif

	// 受信ビジー中(通信先にビジー応答を行った場合)は受信ビジーを解除
	// (スレッド処理で通信先にビジー解除が通知される)
	if (_isReceiveBusy) {
		LOG(_logName + "Recovered receive queue full.)");
		_isReceiveBusy = false;
		_busyOffSendFlag = true;
	}

	return MAN_API_RET_SUCCSESS;
}

//
// オーナー(CommMain)設定
//
void CommModule::SetOwner(CommModuleInterface* owner)
{
	_owner = owner;
}

//
// スレッド前処理
//
int CommModule::ThreadInitProc()
{
	return 0;
}

//
// スレッド処理
//
int CommModule::ThreadProc()
{
	// ビジー解除状態通知送信フラグON中なら状態通知(ビジー解除)を送信
	if (_busyOffSendFlag) {
		CommCmdStat* statCmd = CommCmdBuilder::CreateStatCommand();
		statCmd->SetBusy(false);
		if (SendCommand(statCmd) == 0) {
			_busyOffSendFlag = false;
		}
		delete statCmd;
	}

	if (_sendCounter > 1000) {
		_sendCounter = 0;
		Sleep(1);
	}
	_sendCounter++;

	// データなし
	if (_sendQue->IsEmpty()) {
		Sleep(1);
		return 0;
	}
	// 応答待ち中
	if (_isWaitingReply) {
		return 0;
	}
	// 送信ビジー中
	if (_isSendBusy) {
		return 0;
	}

	// デキュー
	ICommData* commData = _sendQue->GetTop();
#ifdef _DEBUG_LOG_
	LOG(_logName + "Dequeue. (send queue count=%d)", _sendQue->Count());
#endif
	// 電文生成
	CommCmdData* cmd = CommCmdBuilder::CreateDataCommand();
	// 電文にデータ設定
	cmd->SetCommandData(commData);
	// 応答待ち状態に遷移
	_isWaitingReply = true;
	// 電文送信
	if (SendCommand(cmd) != 0) {
		// 送信に失敗したら応答待ちを解除
		_isWaitingReply = false;
	}
	// 電文破棄
	delete cmd;

	return 0;
}

//
// スレッド後処理
//
int CommModule::ThreadTermProc()
{
	TcpStop();
	return 0;
}

//
// TCP/IPクライアント接続コールバック
//
void CommModule::Connected(const std::string& ipAddr)
{
	// 接続中に設定
	_isConnected = true;
	// オーナー(CommMain)に通知
	if (_owner != NULL) {
		LOG(_logName + "Notify connected.");
		_owner->NotifyConnected(_bmlIfCommType, _commPort);
	}
}

//
// TCP/IP切断コールバック
//
void CommModule::Disconnected(const std::string& ipAddr, int port)
{
	// 接続中を解除
	_isConnected = false;
	// オーナー(CommMain)に通知
	if (_owner != NULL) {
		LOG(_logName + "Notify disconnected.");
		_owner->NotifyDisconnected(_bmlIfCommType, port);
	}
}

//
// TCP/IP受信コールバック
//
int CommModule::ReceivedData(const unsigned char* data, int dataSize, const std::string& ipAddr)
{
	//
	// 電文ヘッダ固定部未受信
	//
	if (_tmpCmd == NULL) {
		// 仮の電文生成
		CommCommand* tmpCmd = new CommCommand();
		// ヘッダ固定部設定
		if (tmpCmd->SetHeader(data, dataSize) != 0) {
			LOG(_logName + "[ERROR] Receive failed. Illegal command. (Header error.)");
			delete tmpCmd;
			return CMD_HEADER_LEN;
		}
		// 電文保存
		_tmpCmd = tmpCmd;
		// 次回受信サイズ：電文の残りサイズ
		return _tmpCmd->GetRemainSize();
	}
	//
	// 電文ヘッダ固定部受信済み（データ部受信待ち）
	//
	else {
		std::string replyCode = "";		// 応答コード初期化

		// ヘッダ固定部から該当の電文を生成
		CommCommand* cmd;
		cmd = CommCmdBuilder::CreateCommand(_tmpCmd);
		delete _tmpCmd;
		_tmpCmd = NULL;

		if (cmd == NULL) {
			LOG(_logName + "[ERROR] Receive failed. Illegal command. (Unknown command)");
			return CMD_HEADER_LEN;
		}

		// 固定部以下(データ部)を設定
		if (cmd->SetPartialData(data, dataSize) != 0) {
			// 受信失敗
			LOG(_logName + "[ERROR] Receive failed. Illegal command. (Data error.)");

			// ※固定部は受信できているので一応応答は返す

			// 応答電文生成
			replyCode = CMD_ACT_NAK;
			CommCommand* replyCmd = CommCmdBuilder::CreateReplyCommand(cmd, replyCode);
			LOG(_logName + "Send reply (type=%s reply_code=%s)", replyCmd->GetType().c_str(), replyCode.c_str());
			// 応答電文送信
			if (SendCommand(replyCmd) != 0) {
				LOG(_logName + "[ERROR] Send reply failed. (type=%s reply_code=%s)", replyCmd->GetType().c_str(), replyCode.c_str());
			}
			// 応答電文破棄
			delete replyCmd;
			// 受信電文破棄
			delete cmd;
			// 次回受信サイズ：ヘッダ固定部サイズ
			return CMD_HEADER_LEN;
		}

		//
		// 要求電文受信
		//
		if (cmd->GetAction() == CMD_ACT_REQ) {

#ifdef _DEBUG_LOG_
			LOG(_logName + "Received. (type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
#endif
			//
			// 通信制御情報受信
			//
			if (cmd->GetType() == CMD_TYPE_INFO) {
				CommCmdCtrl* cmd_ = dynamic_cast<CommCmdCtrl*>(cmd);
				if (cmd_ != NULL) {
					// オーナー(CommMain)に通知
					if (_owner != NULL) {
						_owner->NotifyCtrlInfo(cmd_->GetCtrlInfoList());
					}
				}
				else {
					LOG(_logName + "[ERROR] Receive failed. Illegal command. (Control command error.)");
				}
				// ACK応答
				replyCode = CMD_ACT_ACK;
			}
			//
			// 状態通知受信
			//
			else if (cmd->GetType() == CMD_TYPE_STAT) {
				CommCmdStat* cmd_ = dynamic_cast<CommCmdStat*>(cmd);
				if (cmd_ != NULL) {
					// ビジー解除の場合
					if (!cmd_->IsBusy()) {
						// 送信ビジー解除
						_isSendBusy = false;
						LOG(_logName + "Recovered Destination(send queue) busy.)");
					}
					// オーナー(CommMain)に通知
					if (_owner != NULL) {
						_owner->NotifyStat(cmd_->IsBusy());
					}
				}
				else {
					LOG(_logName + "[ERROR] Receive failed. Illegal command. (Status command error.)");
				}
				// ACK応答
				replyCode = CMD_ACT_ACK;
			}
			//
			// 終了通知受信
			//
			else if (cmd->GetType() == CMD_TYPE_TERM) {
				// オーナー(CommMain)に通知
				if (_owner != NULL) {
					_owner->NotifyTerm();
				}
				else {
					LOG(_logName + "[ERROR] Receive failed. Illegal command. (Terminate command error.)");
				}
				// 応答なし
			}
			//
			// データ送信電文受信
			//
			else if (cmd->GetType() == CMD_TYPE_DATA) {
				CommCmdData* cmd_ = dynamic_cast<CommCmdData*>(cmd);
				if (cmd_ != NULL) {
					// 受信キューがフル
					if (_receiveQue->IsFull()) {
						// 受信ビジー設定
						_isReceiveBusy = true;
						// ビジー応答
						replyCode = CMD_ACT_BUSY;
						LOG(_logName + "[BUSY]  Receive failed. (receive queue is full. count=%d)", _receiveQue->Count());
					}
					// 受信可能
					else {
						// データなし
						if (cmd_->GetCommandData() == NULL) {
							replyCode = CMD_ACT_NAK;
							LOG(_logName + "[ERROR] Received failed. (Data is nothing.)");
						}
						// データあり
						else {
#ifdef _DEBUG_LOG_
							LOG(_logName + "Received request. (type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
#endif
							// データのクローン生成
							ICommData* clone = cmd_->GetCommandData()->Clone();
							// エンキュー
							int result = _receiveQue->Add(clone);
							// エンキュー成功
							if (result == QUE_RET_SUCCSESS) {
								// ビジー解除
								_isReceiveBusy = false;
								// ACK応答
								replyCode = CMD_ACT_ACK;
#ifdef _DEBUG_LOG_
								LOG(_logName + "Enqueue. (receive queue count=%d)", _receiveQue->Count());
#endif
								// ※データのクローンはデキュー後に解放される
							}
							// オーバーフロー（この前にキューフルチェックしているのでありえないが念のため）
							else if (result == QUE_RET_OVERFLOW) {
								// データのクローン破棄
								delete clone;
								// 受信ビジー設定
								_isReceiveBusy = true;
								// ビジー応答
								replyCode = CMD_ACT_BUSY;
								LOG(_logName + "[BUSY]  Enqueue failed. Overflow. (receive queue count=%d)", _receiveQue->Count());
							}
							// エンキュー失敗
							else {
								// データのクローン破棄
								delete clone;
								// NAK応答
								replyCode = CMD_ACT_NAK;
								LOG(_logName + "[ERROR] Enqueue failed. (receive queue count=%d)", _receiveQue->Count());
							}
						}
					}
				}
				else {
					LOG(_logName + "[ERROR] Receive failed. Illegal command. (Data command error.)");
				}
			}
			// 不明電文
			else {
				replyCode = CMD_ACT_NAK;
				LOG(_logName + "[ERROR] Receive fialed. (Unknown type command. type=%s)", cmd->GetAction().c_str());
			}
		}
		//
		// 応答電文受信
		//
		else {
			// ACK応答受信
			if (cmd->GetReplyCode() == CMD_REP_ACK) {
				_isSendBusy = false;
#ifdef _DEBUG_LOG_
				LOG(_logName + "Received ACK. (type=%s)", cmd->GetType().c_str());
#endif
			}
			// NAK応答受信
			else if (cmd->GetReplyCode() == CMD_REP_NAK) {
				_isSendBusy = false;
				LOG(_logName + "[ERROR] Received NAK. (type=%s)", cmd->GetType().c_str());
			}
			// ビジー応答受信
			else if (cmd->GetReplyCode() == CMD_REP_BUSY) {
				_isSendBusy = true;
				LOG(_logName + "[BUSY]  Received BUSY. (type=%s)", cmd->GetType().c_str());
			}
			// 不明電文受信
			else {
				_isSendBusy = false;
				LOG(_logName + "[ERROR] Receive failed. (Unknown action command. type=%s action=%s)", cmd->GetType().c_str(), cmd->GetAction().c_str());
			}
			_isWaitingReply = false;
		}

		// 応答送信
		if (replyCode != "") {
			// 応答電文生成
			CommCommand* replyCmd = CommCmdBuilder::CreateReplyCommand(cmd, replyCode);
#ifdef _DEBUG_LOG_
			LOG(_logName + "Send reply (type=%s reply_code=%s)", replyCmd->GetType().c_str(), replyCode.c_str());
#endif
			// 応答電文送信
			if (SendCommand(replyCmd) != 0) {
				LOG(_logName + "[ERROR] Send reply failed. (type=%s reply_code=%s)", replyCmd->GetType().c_str(), replyCode.c_str());
			}
			// 応答電文破棄
			delete replyCmd;
		}

		// 受信電文破棄
		delete cmd;
		// 次回受信サイズ：ヘッダ固定部サイズ
		return CMD_HEADER_LEN;
	}

	return 0;
}

