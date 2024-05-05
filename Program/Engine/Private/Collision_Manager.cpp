#include "..\Public\Collision_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Collision_AABB(_tchar* pSourTag, _tchar* pDestTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*	pSourCollider;
	CCollider*	pDestCollider;

	pSourCollider = (CCollider*)pGameInstance->Get_Component(5, pSourTag, TEXT("Com_OBB"));
	pDestCollider = (CCollider*)pGameInstance->Get_Component(5, pDestTag, TEXT("Com_OBB"));

	//m_pAABBCom->Collision_AABB(pSourCollider, pDestCollider);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CCollision_Manager::Free()
{

}
