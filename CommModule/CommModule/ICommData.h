#pragma once

#include "CommDef.h"

//
// 通信データインタフェース（純粋仮想クラス）
//

class ICommData {
public:
	virtual ~ICommData() {};
	//送受信データの格納
	virtual void SetData(const unsigned char* pData, int dataSize) = 0;
	//送受信データ(領域先頭ポインタ)の取得
	virtual unsigned char* GetData() = 0;
	//送受信データサイズの取得
	virtual int GetDataSize() = 0;
	//インスタンスの複製
	virtual ICommData* Clone() = 0;
};