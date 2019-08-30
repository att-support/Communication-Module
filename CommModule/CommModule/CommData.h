#pragma once

#include "ICommData.h"
#include "CommDef.h"

//
// 通信データ
//

class CommData : public ICommData {
public:
	CommData();
	virtual ~CommData();
	//送受信データの格納
	void SetData(const unsigned char* pData, int dataSize);
	//送受信データ(領域先頭ポインタ)の取得
	unsigned char* GetData();
	//送受信データサイズの取得
	int GetDataSize();
	//インスタンスの複製
	ICommData* Clone();
private:
	//送受信データ
	unsigned char* _data;
	//送受信データサイズ
	int _dataSize;
};


