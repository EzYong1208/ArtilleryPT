#pragma once

#include "Base.h"

//	현재 화면에 보여줘야 할 레벨의 주소를 보관함
//	레벨 교체시에 기존 레벨을 정리(삭제)하고, 새로운 레벨로 저장함

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	//	할당하기 위한 함수
	HRESULT			OpenLevel(_uint	iLevelIndex, class CLevel* pNextLevel);		//	pNextLevel : 어떤 레벨을 저장할지
	_int			Tick(_double TimeDelta);
	_int			LateTick(_double TimeDelta);
	HRESULT			Render();

private:
	_uint			m_iCurrentLevelIndex = 0;		//	현재 레벨의 인덱스를 보관하기 위한 멤버변수 <- Client_Defines에 LEVEL 열거체 보관
	class CLevel*	m_pCurrentLevel = nullptr;		//	레벨을 저장

public:
	const _uint&	Get_LevelIndex() const { return m_iCurrentLevelIndex; }
	const CLevel*	Get_Level() const { return m_pCurrentLevel; }

public:
	virtual	void	Free() override;
};

END