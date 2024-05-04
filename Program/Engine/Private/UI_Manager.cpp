#include "..\Public\UI_Manager.h"
#include "UI.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

_int CUI_Manager::Tick(_double TimeDelta)
{
	// m_UI_Clone Tick
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		auto UI_Iter = m_UI_Clone[i].begin(); // map
		for (; UI_Iter != m_UI_Clone[i].end(); )
		{
			//if ((UI_Iter->second)->Get_UI_State()) // UI_State 가 TRUE 인 것만 Tick 돌림
			//{
				_int UIEvent = (UI_Iter->second)->Tick(TimeDelta);
				if (ISDEAD == UIEvent)
				{
					Safe_Release(UI_Iter->second);
					UI_Iter = m_UI_Clone[i].erase(UI_Iter);
				}
				else
					++UI_Iter;
			//}
			//else
			//	++UI_Iter;
		}
	}

	// m_Disposable_UI Tick
	auto Disposable_UI_Iter = m_Disposable_UI.begin();
	for (; Disposable_UI_Iter != m_Disposable_UI.end(); )
	{
		// Disposable_UI_Iter 들의 UI_State 는 항상 TRUE 이기 때문에 따로 조건검사 안 함
		_int UIEvent = (*Disposable_UI_Iter)->Tick(TimeDelta);
		if (ISDEAD == UIEvent)
		{
			Safe_Release(*Disposable_UI_Iter);
			Disposable_UI_Iter = m_Disposable_UI.erase(Disposable_UI_Iter);
		}
		else
			++Disposable_UI_Iter;
	}

	return _int();
}

_int CUI_Manager::LateTick(_double TimeDelta)
{
	// m_UI_Clone LateTick
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_UI_Clone[i])
		{
			//if (Pair.second->Get_UI_State()) // UI 의 m_bUI_State 이 On 일 때만 LateTick 돌림
				Pair.second->LateTick(TimeDelta);
		}
	}

	// m_Disposable_UI LateTick
	for (auto& pUI : m_Disposable_UI)
		pUI->LateTick(TimeDelta);

	return _int();
}

HRESULT CUI_Manager::Reserve_Container(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_UI_Clone = new map<const _tchar*, class CUI*>[m_iNumLevels];

	return S_OK;
}

HRESULT CUI_Manager::Clear_LevelUI(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	m_UI_Clone[iLevelIndex].clear(); // Safe_Release 하면 안됨 (UI 는 싱글톤처럼 전 게임을 통틀어서 한개만 존재하기 때문에 Safe_Release 하면 다시 Clone 못해옴)

	return S_OK;
}

HRESULT CUI_Manager::Add_UI_Prototype(const _tchar * pPrototypeTag, CUI * pPrototype)
{
#if !defined (USE_IMGUI) // ImGui 에서 이전 레벨로 되돌아갈 때 이미 이전에 만들어졌던 UI 의 원형이 지워져있지 않기 때문에 이 부분이 항상 E_FAIL 이 반환돼서 아예 꺼버림
	if (nullptr != Find_UI_Prototype(pPrototypeTag))
	{
		MSGBOX("UI Prototype is already exist");
		return E_FAIL;
	}
#endif

	// ImGui 에서 이전 레벨로 되돌아갈 때 중복된 UI 원본을 넣으려고 하기 때문에 bool 값을 받아서 false 일 경우(중복될경우) Safe_Release 해버림
	_bool bInserted = m_UI_Prototypes.insert(map<const _tchar*, class CUI*>::value_type(pPrototypeTag, pPrototype)).second;

	if (bInserted == false)
		Safe_Release(pPrototype);

	return S_OK;
}

HRESULT CUI_Manager::Add_UI_Clone(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pCloneTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels) // 매개변수가 최대 레벨인덱스보다 크면 E_FAIL 반환
		return E_FAIL;

	CUI* pUI_Prototpye = Find_UI_Prototype(pPrototypeTag); // 원형이 있는지 탐색
	if (nullptr == pUI_Prototpye)
		return E_FAIL;

	CUI* pUI_Clone = pUI_Prototpye->Clone(pArg); // 원형이 있으면 클론
	if (nullptr == pUI_Clone)
		return E_FAIL;

	auto iter = find_if(m_UI_Clone[iLevelIndex].begin(), m_UI_Clone[iLevelIndex].end(), CTagFinder(pCloneTag));
	if (iter != m_UI_Clone[iLevelIndex].end()) // 이미 같은 이름의 사본이 있으면 FAIL
		return E_FAIL;

	m_UI_Clone[iLevelIndex].emplace(pCloneTag, pUI_Clone); // 사본 컨테이너에 삽입

	return S_OK;
}

HRESULT CUI_Manager::Add_Disposable_UI(CUI * pUI)
{
	if (nullptr == pUI)
		MSGBOX("CUI_Manager : Disposable_UI is nullptr");

	m_Disposable_UI.push_back(pUI);

	return S_OK;
}

CUI * CUI_Manager::Get_UI(_uint iLevelIndex, const _tchar * pCloneTag)
{
	auto iter = find_if(m_UI_Clone[iLevelIndex].begin(), m_UI_Clone[iLevelIndex].end(), CTagFinder(pCloneTag));

	if (iter == m_UI_Clone[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CUI_Manager::UI_On(_uint iLevelIndex, const _tchar * pCloneTag)
{
	//auto iter = find_if(m_UI_Clone[iLevelIndex].begin(), m_UI_Clone[iLevelIndex].end(), CTagFinder(pCloneTag));
	//if (iter == m_UI_Clone[iLevelIndex].end())
	//{
	//	MSGBOX("UI Clone Tag is Wrong");
	//	return E_FAIL; // 잘못된 이름을 입력함
	//}

	//iter->second->Set_UI_On();

	return S_OK;
}

HRESULT CUI_Manager::UI_Off(_uint iLevelIndex, const _tchar * pCloneTag)
{
	//auto iter = find_if(m_UI_Clone[iLevelIndex].begin(), m_UI_Clone[iLevelIndex].end(), CTagFinder(pCloneTag));
	//if (iter == m_UI_Clone[iLevelIndex].end())
	//{
	//	MSGBOX("UI Clone Tag is Wrong");
	//	return E_FAIL; // 잘못된 이름을 입력함
	//}

	//iter->second->Set_UI_Off();

	return S_OK;
}

HRESULT CUI_Manager::All_UI_Off(_uint iLevelIndex)
{
	//for (auto& Pair : m_UI_Clone[iLevelIndex])
	//	Pair.second->Set_UI_Off();

	return S_OK;
}

CUI * CUI_Manager::Find_UI_Prototype(const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_UI_Prototypes.begin(), m_UI_Prototypes.end(), CTagFinder(pPrototypeTag));

	if (iter == m_UI_Prototypes.end())
		return nullptr;

	return iter->second;
}

void CUI_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_UI_Clone[i])
			Safe_Release(Pair.second);
		m_UI_Clone[i].clear();
	}

	Safe_Delete_Array(m_UI_Clone);

	for (auto& Pair : m_UI_Prototypes)
		Safe_Release(Pair.second);
	m_UI_Prototypes.clear();
}
