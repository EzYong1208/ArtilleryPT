#pragma once

#include "Engine_Defines.h"

//	��� ��ü���� �θ�
//	���۷��� ī��Ʈ�� ������

BEGIN(Engine)

class ENGINE_DLL CBase abstract	//	abstract: �߻�Ŭ����ȭ �ߴ�(���� ���������� �����Ҵ� �� �� ����)
{
protected:
	CBase();
	virtual	~CBase() = default;			//	= default == { }, �Ҹ��ڿ����� ����� �� ����, �Ҹ��� ����� ����ϰ� ���� ���� ��

public:
	//	������ �� ���� ������ unsigned
	unsigned long	AddRef();			//	���۷��� ī��Ʈ�� ������Ŵ
	unsigned long	Release();			//	���۷��� ī��Ʈ�� ���ҽ�Ű�ų� �����Ѵ�

private:
	unsigned long	m_dwRefCnt = 0;

public:
	virtual	void	Free() = 0;
};

END
