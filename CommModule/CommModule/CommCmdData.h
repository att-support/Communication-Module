#pragma once
#include "CommCommand.h"
#include "CommData.h"

//
// データ送信要求電文
//

class CommCmdData :
	public CommCommand
{
public:
	CommCmdData();
	virtual ~CommCmdData();
	CommCommand* Clone();

	void SetCommandData(ICommData* data);
	ICommData* GetCommandData();

private:
	ICommData* _cmdData;

	int _GetDataLen();
	void _SetDataToCommand(unsigned char*& data);
	int _SetDataFromCommand(const unsigned char*& data);
};

