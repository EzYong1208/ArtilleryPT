#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(pComponentTag, iIndex);
}

CGameObject * CObject_Manager::Get_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIndex);
}

CLayer * CObject_Manager::Get_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (m_pLayers[iLevelIndex].end() == iter)
		return nullptr;

	//	map<const _tchar*, class CLayer*>* m_pLayers
	return iter->second;
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	//	m_iNumLevels 만큼 동적할당
	m_pLayers = new LAYERS[m_iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	//	이미 이름이 같은 프로토타입이 있거나 넣으려는 객체가 nullptr 이면 E_FAIL 반환
	if (nullptr == pPrototype ||
		nullptr != Find_PrototypeTag(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	//	원형객체 검색
	//	원형객체가 없으면 E_FAIL 반환
	CGameObject*	pPrototype = Find_PrototypeTag(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	//	원형객체가 있고, 그 원형객체가 Clone(pArg) 함수를 수행함
	//	nullptr 을 반환하면 E_FAIL 반환
	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	//	해당 레벨 들어가서 이미 Layer 클래스가 있는지 확인 (객체가 추가된적이 있는지)
	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	//	추가된 적이 없다면 Layer를 하나 추가해서 거기에 게임오브젝트 추가
	//	처음에 시작할때는 Layer가 없으니 생성
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		if (nullptr == pLayer)
			return	E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return	E_FAIL;

		//	레이어에 객체를 추가, 맵컨테이너에다 인덱스로 추가
		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	//	이미 있는 레이어라면 그냥 게임오브젝트만 추가
	else
	{
		//	나의 예약된 컨테이너 크기보다 크면 E_FAIL
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}

	//	Layer 클래스에 사본객체를 추가할 때에는 레퍼런스카운트를 증가 안 시킴
	//	원형객체를 참조하는게 아니라 복제(생성) 하는거니까 (Clone 함수)

	return S_OK;
}

HRESULT CObject_Manager::Clear_LevelObject(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
		_int a = 0;
	}

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

_int CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (0 > Pair.second->Tick(TimeDelta))
				return -1;
		}
	}

	return _int();
}

_int CObject_Manager::LateTick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (0 > Pair.second->LateTick(TimeDelta))
				return -1;
		}
	}

	return _int();
}

CGameObject * CObject_Manager::Find_PrototypeTag(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));

	//	찾으려는 원형객체가 없다면 nullptr 반환
	if (iter == m_Prototypes.end())
		return nullptr;

	//	있다면 원형객체 반환. map<const _tchar*, class CGameObject*>
	return	iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	//	레벨별로 구분이 되어있기 때문에 m_pLayers[iLevelIndex] 로 해당 레벨에 맞는 배열 위치로 가서 탐색해야함
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return	nullptr;

	//	map<const _tchar*, class CLayer*>* m_pLayers
	return iter->second;
}

void CObject_Manager::Free()
{
	//	사본 먼저 지우고
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	//	원형 지우기
	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
