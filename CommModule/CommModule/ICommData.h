#pragma once

#include "CommDef.h"

//
// �ʐM�f�[�^�C���^�t�F�[�X�i�������z�N���X�j
//

class ICommData {
public:
	virtual ~ICommData() {};
	//����M�f�[�^�̊i�[
	virtual void SetData(const unsigned char* pData, int dataSize) = 0;
	//����M�f�[�^(�̈�擪�|�C���^)�̎擾
	virtual unsigned char* GetData() = 0;
	//����M�f�[�^�T�C�Y�̎擾
	virtual int GetDataSize() = 0;
	//�C���X�^���X�̕���
	virtual ICommData* Clone() = 0;
};