#include "..\Public\Base.h"

using namespace Engine;

CBase::CBase()
{
}

unsigned long CBase::AddRef()
{
	//	증가한 이후의 레퍼런스 카운트의 값을 리턴
	return	++m_dwRefCnt;
}

unsigned long CBase::Release()
{
	//	감소하기 이전의 레퍼런스 카운트의 값을 리턴
	//	1일때 0으로 감소하면 0으로 감소한건지 삭제가 된건지 판단되지 않으므로 후치연산으로 감소하기 이전의 레퍼런스 카운트 값을 리턴하게 함
	if (0 != m_dwRefCnt)
		return	m_dwRefCnt--;

	else
	{
		Free();
		delete	this;
		return	0;
	}
}

/*
상속관계에서 소멸자 순서(최하위 자식 -> 부모 순), 무조건
소멸자 호출이 아닌 Free형식으로 할거면(부모의 소멸 시점을 정하고 싶으면/자식만 하고 부모는 나중에 하고 싶을때)
CPlayer::Free()
{
///
CObject::Free();
~~~
}
*/