#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_Scale(STATE eState) const
{
	return XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrix.m[eState][0]));
}

_vector CTransform::Get_State(STATE eState) const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
}

void CTransform::Set_TransformDesc(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CShader * pShader, const char * pValueName)
{
	//	��ġ�ؼ� ����Ѵ�
	//	XMLoadFloat4x4�� XMMatrix ����� XMMatrixTranspose �� ��ġ�ϰ� XMStoreFloat4x4 �� m_WorldMatrix�� ����
	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	return pShader->Set_RawValue(pValueName, &WorldMatrix, sizeof(_float4x4));
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	//Set_State(CTransform::STATE_POSITION, vPosition);

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition))
		Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition))
		Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	//	���� ���¿��� �߰������� ȸ����ų�Ŷ� ���� ���� ���¸� �������°��� �߿���
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	// vRight = XMVectorSetW(vRight, 0.f);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT)));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP)));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK)));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Scaled(_fvector vScale)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Scaling(_double TimeDelta)
{
	return S_OK;
}

HRESULT CTransform::Chase(_fvector vTargetPos, _double TimeDelta)
{
	return S_OK;
}

HRESULT CTransform::LookAt(_fvector vTargetPos, _double TimeDelta)
{
	//	���� ��ġ, Scale ������ (Scale �� R,U,L ���� ���� ���ϱ� ���� ������)
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);
	_vector vScaleRight = Get_Scale(CTransform::STATE_RIGHT);
	_vector vScaleUp = Get_Scale(CTransform::STATE_UP);
	_vector vScaleLook = Get_Scale(CTransform::STATE_LOOK);

	_vector vRight, vUp, vLook;
	_vector vOne = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	//	Look ���� = (Ÿ����ġ-������ġ)�� ����ȭ * ������
	vLook = vTargetPos - vPosition;
	vLook = XMVector3Normalize(vLook) * vScaleLook;

	vRight = XMVector3Cross(vOne, vLook);
	vRight = XMVector3Normalize(vRight) * vScaleRight;

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp) * vScaleUp;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

CTransform * CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
