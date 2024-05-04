#pragma once

#include "Base.h"

//	���� ȭ�鿡 ������� �� ������ �ּҸ� ������
//	���� ��ü�ÿ� ���� ������ ����(����)�ϰ�, ���ο� ������ ������

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	//	�Ҵ��ϱ� ���� �Լ�
	HRESULT			OpenLevel(_uint	iLevelIndex, class CLevel* pNextLevel);		//	pNextLevel : � ������ ��������
	_int			Tick(_double TimeDelta);
	_int			LateTick(_double TimeDelta);
	HRESULT			Render();

private:
	_uint			m_iCurrentLevelIndex = 0;		//	���� ������ �ε����� �����ϱ� ���� ������� <- Client_Defines�� LEVEL ����ü ����
	class CLevel*	m_pCurrentLevel = nullptr;		//	������ ����

public:
	const _uint&	Get_LevelIndex() const { return m_iCurrentLevelIndex; }
	const CLevel*	Get_Level() const { return m_pCurrentLevel; }

public:
	virtual	void	Free() override;
};

END