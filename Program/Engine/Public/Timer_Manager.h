#pragma once

#include "Base.h"

//	�������� Ÿ�̸ӵ��� �����Ͽ� �����Ѵ�

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual	~CTimer_Manager() = default;

public:
	//	Get �Ҷ����� ���� ���� (Update ����� �ȿ� �������)
	_double			Get_TimeDelta(const _tchar* pTimerTag);

public:
	//	map �����̳ʿ� �߰��� �Լ�, � Ű������ ������ const _tchar* pTimerTag
	HRESULT			Add_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, class CTimer*>			m_Timers;
	typedef map<const _tchar*, class CTimer*>	TIMERS;

private:
	//	�ߺ��� �±װ� �ִ���(�±װ� �ߺ��Ǿ�����) Ȯ���ϴ� �Լ�
	class CTimer*	Find_Timer(const _tchar* pTimerTag);

public:
	virtual	void	Free() override;
};

END