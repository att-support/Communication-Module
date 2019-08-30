#pragma once

#include <string>
#include <Util/SysTime.h>
#include <Util/BinData.h>
#include <Util/Conv.h>
#include "CommLogger.h"

//
// 電文種別
//
#define CMD_TYPE_INFO		"INFO"
#define CMD_TYPE_DATA		"DATA"
#define CMD_TYPE_STAT		"STAT"
#define CMD_TYPE_TERM		"TERM"

//
// 要求/応答種別
//
#define CMD_ACT_REQ			"REQ"
#define CMD_ACT_ACK			"ACK"
#define CMD_ACT_NAK			"NAK"
#define CMD_ACT_BUSY		"BSY"

//
// 定数
//
#define CMD_HEADER_LEN		30	// 1+4+3+14+8
#define CMD_MIN_LEN			(CMD_HEADER_LEN + 1)	// minimum command length
#define CMD_TYPE_LEN		4
#define CMD_ACT_LEN			3
#define CMD_TS_LEN			14
#define CMD_DATA_LEN_LEN	8
#define CMD_MAX_DATA_LEN	99999999

//
// 応答種別
//
typedef enum {
	CMD_REP_UNKNOWN,
	CMD_REP_ACK,
	CMD_REP_NAK,
	CMD_REP_BUSY
}CMD_REP_CODE;

//
// 電文（共通・基底）
//

class CommCommand
{
public:
	// コンストラクタ
	CommCommand();
	// デストラクタ
	virtual ~CommCommand();
	// クローン生成
	virtual CommCommand* Clone();

	// 電文作成
	void MakeCommand(unsigned char** data, int& dataLen);
	// 電文設定
	int SetCommand(const unsigned char* data, int dataLen);
	// ヘッダ固定部設定
	int SetHeader(const unsigned char* data, int dataLen);
	// データ部設定(ヘッダ固定部以外の残り)
	int SetPartialData(const unsigned char* data, int dataLen);
	// 残り受信サイズ取得
	int GetRemainSize();
	// ヘッダ固定部コピー
	void CopyHeader(CommCommand* cmd);
	// 電文種別設定
	void SetType(const std::string& type);
	// 電文種別取得
	std::string GetType();
	// 要求/応答種別設定
	std::string GetAction();
	// 要求/応答種別取得
	void SetAction(const std::string& act);
	// 応答種別設定
	void SetReplyCode(CMD_REP_CODE code);
	// 応答種別取得
	CMD_REP_CODE GetReplyCode();
	// タイムスタンプ取得
	SysTime GetTimestamp();
	// 代入オペレーター
	CommCommand& operator=(const CommCommand& obj) {
		_cmdType = obj._cmdType;
		_cmdAct = obj._cmdAct;
		_timestamp = obj._timestamp;
		_cmdDataLen = obj._cmdDataLen;
		_isComplete = obj._isComplete;
		return *this;
	}

protected:
	const int _stx = 0x02;				// STX
	const int _etx = 0x03;				// ETX
	std::string _cmdType;				// 電文種別
	std::string _cmdAct;				// 要求/応答種別
	SysTime _timestamp;					// タイムスタンプ
	int _cmdDataLen;					// データ長
	bool _isComplete;					// 電文完成状態 (true:完成)
	std::string _logName;				// ログ出力用プリフィクス名称

	// 電文コピー
	void Copy(CommCommand* cmd);
	// データ長取得
	virtual int _GetDataLen();
	// データ設定（書き出し）
	virtual void _SetDataToCommand(unsigned char*& data);
	// データ設定（読み出し）
	virtual int _SetDataFromCommand(const unsigned char*& data);
};

