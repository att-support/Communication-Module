#pragma once

#include "ICommData.h"
#include "CommDef.h"

//
// �ʐM�f�[�^
//

class CommData : public ICommData {
public:
	CommData();
	virtual ~CommData();
	//����M�f�[�^�̊i�[
	void SetData(const unsigned char* pData, int dataSize);
	//����M�f�[�^(�̈�擪�|�C���^)�̎擾
	unsigned char* GetData();
	//����M�f�[�^�T�C�Y�̎擾
	int GetDataSize();
	//�C���X�^���X�̕���
	ICommData* Clone();
private:
	//����M�f�[�^
	unsigned char* _data;
	//����M�f�[�^�T�C�Y
	int _dataSize;
};


