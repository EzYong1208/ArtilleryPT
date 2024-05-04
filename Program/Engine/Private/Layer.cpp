#include "..\Public\Layer.h"
#include "GameObject.h"


CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	if (iIndex >= m_Objects.size())
		return nullptr;

	auto	iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

CGameObject * CLayer::Get_GameObject(_uint iIndex)
{
	if (iIndex >= m_Objects.size())
		return nullptr;

	auto iter = m_Objects.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	m_Objects.push_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_double TimeDelta)
{
	auto iter = m_Objects.begin();
	for (; iter != m_Objects.end();)
	{
		_int ObjectEvent = (*iter)->Tick(TimeDelta);
		switch (ObjectEvent)
		{
		case 0: //	NoEvent
			++iter;
			break;

		case ISDEAD:	//	사망판정시 오브젝트를 제거합니다.
			Safe_Release(*iter);
			iter = m_Objects.erase(iter);
			break;

		case -1:
			return -1;

		default:
			break;
		}

	}

	return 0;
}

_int CLayer::LateTick(_double TimeDelta)
{
	for (auto& pGameObject : m_Objects)
	{
		if (0 > pGameObject->LateTick(TimeDelta))
			return -1;
	}

	return 0;
}

CLayer * CLayer::Create()
{
	CLayer*		pLayer = new CLayer();

	return pLayer;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	m_Objects.clear();
}