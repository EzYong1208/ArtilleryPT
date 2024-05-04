#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	if (FAILED(__super::NativeConstruct(&m_CameraDesc.TransformDesc)))
		return E_FAIL;

	//	Transform 의 WorldMatrix 설정
	//	vLook은 At에서 Eye를 빼면 현재 Look이 구해짐, 노멀라이즈하고 Set_State
	_vector		vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	vLook = XMVector3Normalize(vLook);

	//	vRight 업과(AxisY) 룩(Look)을 외적해서 구하고 노멀라이즈 Set_State
	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	vRight = XMVector3Normalize(vRight);

	//	vUp 룩과 라이트를 외적해서 구하고 노멀라이즈 Set_State
	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->Set_State(CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4(m_CameraDesc.vEye, 1.f)));

	return S_OK;
}

_int CCamera::Tick(_double TimeDelta)
{
	if (nullptr == m_pDevice ||
		nullptr == m_pDeviceContext)
		return -1;

	//	뷰스페이스 세팅
	//	카메라 월드행렬의 역행렬 = 뷰행렬
	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransform->Get_WorldMatrix()))))
		return E_FAIL;

	//	투영스페이스 세팅
	_matrix		ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
	
	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix)))
		return E_FAIL;

	return _int();
}

_int CCamera::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);

	//Safe_Release(m_pTransform);
}
