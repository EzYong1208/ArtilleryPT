#pragma once

#include "Base.h"

//	여러개의 타이머들을 생성하여 보관한다

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual	~CTimer_Manager() = default;

public:
	//	Get 할때마다 새로 갱신 (Update 기능이 안에 들어있음)
	_double			Get_TimeDelta(const _tchar* pTimerTag);

public:
	//	map 컨테이너에 추가할 함수, 어떤 키값으로 넣을지 const _tchar* pTimerTag
	HRESULT			Add_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, class CTimer*>			m_Timers;
	typedef map<const _tchar*, class CTimer*>	TIMERS;

private:
	//	중복된 태그가 있는지(태그가 중복되었는지) 확인하는 함수
	class CTimer*	Find_Timer(const _tchar* pTimerTag);

public:
	virtual	void	Free() override;
};

END