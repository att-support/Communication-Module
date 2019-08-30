#include "stdafx.h"
#include "CommCmdBuilder.h"


//
// 電文生成クラス
//

//
// コンストラクタ
//
CommCmdBuilder::CommCmdBuilder()
{
}

//
// デストラクタ
//
CommCmdBuilder::~CommCmdBuilder()
{
}

//
// 電文生成
//
CommCommand* CommCmdBuilder::CreateCommand(CommCommand* srcCmd)
{
	CommCommand* cmd = NULL;
	if (srcCmd->GetAction() == CMD_ACT_REQ) {
		if (srcCmd->GetType() == CMD_TYPE_INFO) {
			cmd = new CommCmdCtrl();
		}
		else if (srcCmd->GetType() == CMD_TYPE_STAT) {
			cmd = new CommCmdStat();
		}
		else if (srcCmd->GetType() == CMD_TYPE_DATA) {
			cmd = new CommCmdData();
		}
		else if (srcCmd->GetType() == CMD_TYPE_TERM) {
			cmd = new CommCommand();
		}
		else {
			cmd = new CommCommand();
		}
	}
	//Reply
	else {
		cmd = new CommCommand();
	}
	cmd->CopyHeader(srcCmd);
	return cmd;
}

//
// 応答電文生成
//
CommCommand* CommCmdBuilder::CreateReplyCommand(CommCommand* cmd, const std::string& replyCode)
{
	CommCommand* replyCmd = new CommCommand();
	replyCmd->SetType(cmd->GetType());
	replyCmd->SetAction(replyCode);
	return replyCmd;
}

//
// 通信制御情報電文生成
//
CommCmdCtrl* CommCmdBuilder::CreateCtrlCommand()
{
	return new CommCmdCtrl();
}

//
// データ送信電文生成
//
CommCmdData* CommCmdBuilder::CreateDataCommand()
{
	return new CommCmdData();
}

//
// 状態通知電文生成
//
CommCmdStat* CommCmdBuilder::CreateStatCommand()
{
	return new CommCmdStat();
}

//
// 終了通知電文生成
//
CommCommand* CommCmdBuilder::CreateTermCommand()
{
	CommCommand* cmd = new CommCommand();
	cmd->SetType(CMD_TYPE_TERM);
	cmd->SetAction(CMD_ACT_REQ);
	return cmd;
}

