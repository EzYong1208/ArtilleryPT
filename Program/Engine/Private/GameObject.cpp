#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"

const _tchar* CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

//	원본 객체에 있는 그래픽 디바이스를 얕은 복사 하지만 레퍼런스 카운트를 관리할 수가 있다 / 안만들어도 디폴트 복사 생성자가 알아서 만들어져서 해주긴 하지만 레퍼런스 카운트 관리를 못하니 굳이 만듬
CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	return Find_Component(pComponentTag);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void* pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (nullptr != pArg)
		m_pTransform->Set_TransformDesc(*(CTransform::TRANSFORMDESC*)pArg);

	if (nullptr != Find_Component(m_pTransformTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_Components.emplace(m_pTransformTag, m_pTransform);

	Safe_AddRef(m_pTransform);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CGameObject::Tick(_double TimeDelta)
{
	return _int();
}

_int CGameObject::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);

	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGameObject::Hitted()
{
	++m_iHit;

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Compute_CamDistance(CTransform * pTransform)
{
	//if (nullptr == m_pGraphic_Device)
	//	return E_FAIL;

	//_float4x4		ViewMatrix;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	//D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	//_float3		vCamPosition = *(_float3*)&ViewMatrix.m[3][0];

	//_float3		vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

	//_float3		vDir = vPosition - vCamPosition;

	//m_fCamDistance = D3DXVec3Length(&vDir);

	if (nullptr == m_pDevice)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	_vector			vCamPos = ViewMatrix.r[3];

	_vector			vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

	_vector			vDir = vPosition - vCamPos;

	m_fCamDistance = XMVectorGetX(XMVector3Length(vDir));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransform);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();
}
