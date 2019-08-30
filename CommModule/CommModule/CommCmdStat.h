#pragma once
#include "CommCommand.h"

//
// èÛë‘í ímìdï∂
//

class CommCmdStat :
	public CommCommand
{
public:
	CommCmdStat();
	virtual ~CommCmdStat();
	CommCommand* Clone();

	bool IsBusy();
	void SetBusy(bool isBusy);

private:
	bool _isBusy;

	int _GetDataLen();
	void _SetDataToCommand(unsigned char*& data);
	int _SetDataFromCommand(const unsigned char*& data);

};

