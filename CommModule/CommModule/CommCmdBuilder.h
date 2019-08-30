#pragma once
#include "CommCommand.h"
#include "CommCmdCtrl.h"
#include "CommCmdData.h"
#include "CommCmdStat.h"

//
// 電文生成クラス
//

class CommCmdBuilder
{
public:
	CommCmdBuilder();
	virtual ~CommCmdBuilder();

	// 電文生成
	static CommCommand* CreateCommand(CommCommand* cmd);
	// 応答電文生成
	static CommCommand* CreateReplyCommand(CommCommand* cmd, const std::string& replyCode);
	// 通信制御情報電文生成
	static CommCmdCtrl* CreateCtrlCommand();
	// データ送信電文生成
	static CommCmdData* CreateDataCommand();
	// 状態通知電文生成
	static CommCmdStat* CreateStatCommand();
	// 終了通知電文生成
	static CommCommand* CreateTermCommand();
};

