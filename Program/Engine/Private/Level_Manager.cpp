#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}


HRESULT CLevel_Manager::OpenLevel(_uint iLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == pNextLevel)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//	최초할때는 클리어할 필요가 없다
	if (nullptr != m_pCurrentLevel)
	{
		//	현재 레벨 인덱스(밑에 지우기 전이기 때문에 현재 레벨임)
		if (FAILED(pGameInstance->Clear_LevelResource(m_iCurrentLevelIndex)))
			return	E_FAIL;
	}

	//	기존의 레벨의 것들을 먼저 날리고
	Safe_Release(m_pCurrentLevel);

	//	다음 레벨걸 넣음?
	m_pCurrentLevel = pNextLevel;

	//	현재 들어오는 인덱스가 중요함
	m_iCurrentLevelIndex = iLevelIndex;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CLevel_Manager::Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return -1;

	return m_pCurrentLevel->Tick(TimeDelta);
}

_int CLevel_Manager::LateTick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return -1;

	return m_pCurrentLevel->LateTick(TimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
