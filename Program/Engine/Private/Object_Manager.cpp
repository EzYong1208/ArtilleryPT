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

	//	m_iNumLevels ��ŭ �����Ҵ�
	m_pLayers = new LAYERS[m_iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	//	�̹� �̸��� ���� ������Ÿ���� �ְų� �������� ��ü�� nullptr �̸� E_FAIL ��ȯ
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

	//	������ü �˻�
	//	������ü�� ������ E_FAIL ��ȯ
	CGameObject*	pPrototype = Find_PrototypeTag(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	//	������ü�� �ְ�, �� ������ü�� Clone(pArg) �Լ��� ������
	//	nullptr �� ��ȯ�ϸ� E_FAIL ��ȯ
	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	//	�ش� ���� ���� �̹� Layer Ŭ������ �ִ��� Ȯ�� (��ü�� �߰������� �ִ���)
	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	//	�߰��� ���� ���ٸ� Layer�� �ϳ� �߰��ؼ� �ű⿡ ���ӿ�����Ʈ �߰�
	//	ó���� �����Ҷ��� Layer�� ������ ����
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		if (nullptr == pLayer)
			return	E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return	E_FAIL;

		//	���̾ ��ü�� �߰�, �������̳ʿ��� �ε����� �߰�
		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	//	�̹� �ִ� ���̾��� �׳� ���ӿ�����Ʈ�� �߰�
	else
	{
		//	���� ����� �����̳� ũ�⺸�� ũ�� E_FAIL
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}

	//	Layer Ŭ������ �纻��ü�� �߰��� ������ ���۷���ī��Ʈ�� ���� �� ��Ŵ
	//	������ü�� �����ϴ°� �ƴ϶� ����(����) �ϴ°Ŵϱ� (Clone �Լ�)

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

	//	ã������ ������ü�� ���ٸ� nullptr ��ȯ
	if (iter == m_Prototypes.end())
		return nullptr;

	//	�ִٸ� ������ü ��ȯ. map<const _tchar*, class CGameObject*>
	return	iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	//	�������� ������ �Ǿ��ֱ� ������ m_pLayers[iLevelIndex] �� �ش� ������ �´� �迭 ��ġ�� ���� Ž���ؾ���
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return	nullptr;

	//	map<const _tchar*, class CLayer*>* m_pLayers
	return iter->second;
}

void CObject_Manager::Free()
{
	//	�纻 ���� �����
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	//	���� �����
	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
