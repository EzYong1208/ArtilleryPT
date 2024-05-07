#pragma once

#include "Engine_Defines.h"

//	모든 객체들의 부모
//	레퍼런스 카운트를 관리함

BEGIN(Engine)

class ENGINE_DLL CBase abstract	//	abstract: 추상클래스화 했다(절대 독립적으로 동적할당 할 수 없음)
{
protected:
	CBase();
	virtual	~CBase() = default;			//	= default == { }, 소멸자에서만 사용할 수 있음, 소멸자 기능을 사용하고 싶지 않을 때

public:
	//	음수가 될 일이 없으니 unsigned
	unsigned long	AddRef();			//	레퍼런스 카운트를 증가시킴
	unsigned long	Release();			//	레퍼런스 카운트를 감소시키거나 삭제한다

private:
	unsigned long	m_dwRefCnt = 0;

public:
	virtual	void	Free() = 0;
};

END
