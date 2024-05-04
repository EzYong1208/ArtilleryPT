#include "..\Public\Base.h"

using namespace Engine;

CBase::CBase()
{
}

unsigned long CBase::AddRef()
{
	//	������ ������ ���۷��� ī��Ʈ�� ���� ����
	return	++m_dwRefCnt;
}

unsigned long CBase::Release()
{
	//	�����ϱ� ������ ���۷��� ī��Ʈ�� ���� ����
	//	1�϶� 0���� �����ϸ� 0���� �����Ѱ��� ������ �Ȱ��� �Ǵܵ��� �����Ƿ� ��ġ�������� �����ϱ� ������ ���۷��� ī��Ʈ ���� �����ϰ� ��
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
��Ӱ��迡�� �Ҹ��� ����(������ �ڽ� -> �θ� ��), ������
�Ҹ��� ȣ���� �ƴ� Free�������� �ҰŸ�(�θ��� �Ҹ� ������ ���ϰ� ������/�ڽĸ� �ϰ� �θ�� ���߿� �ϰ� ������)
CPlayer::Free()
{
///
CObject::Free();
~~~
}
*/