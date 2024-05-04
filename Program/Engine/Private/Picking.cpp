#include "..\Public\Picking.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{

}

HRESULT CPicking::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd)
{
	m_hWnd = hWnd;
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CPicking::Transform_ToWorldSpace()
{
	/* 뷰포트상의 마우스 위치를 구한다. */
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	D3D11_VIEWPORT		ViewPortDesc;
	//ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	_uint	iNumViewports = 0;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	//ViewPortDesc.Width = 1280;
	//ViewPortDesc.Height = 720;
	//ViewPortDesc.MaxDepth = 1.f;

	//m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);


	/* 투영스페이스 상의 마우스 위치를 구한다. */
	/* 0, 0 -> WinCX, WinCY */
	/* -1, 1 -> 1, -1*/
	/* 로컬위치 * 월드 * 뷰 * 투영 */

	_float4		vMousePos;
	vMousePos.x = ptMouse.x / (ViewPortDesc.Width * 0.5f) - 1;
	vMousePos.y = ptMouse.y / -(ViewPortDesc.Height * 0.5f) + 1;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;

	/* 뷰스페이스 상의 위치로 변환한다. */
	/* 로컬위치 * 월드 * 뷰 */
	_matrix		ProjMatrixInverse;

	_vector		MousePos;
	memcpy(&MousePos, &vMousePos, sizeof(_float4));

	_vector		one;
	_float4		i;
	i.x = 0.f;
	i.y = 0.f;
	i.z = 0.f;
	i.w = 1.f;
	memcpy(&one, &i, sizeof(_float4));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	ProjMatrixInverse = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInverse = XMMatrixInverse(nullptr, ProjMatrixInverse);

	MousePos = XMVector3TransformCoord(MousePos, ProjMatrixInverse);
	//XMStoreFloat4(&vMousePos, MousePos);
	
	memcpy(&m_vRayDir, &(MousePos - one), sizeof(_float3));
	//memcpy(&m_vRayDir, &(vMousePos - _float4(0.f, 0.f, 0.f, 1.f)), sizeof(_float3));
	//m_vRayPos = _float3(0.f, 0.f, 0.f);

	/* 월드스페이스 상의 위치로 변환한다. */
	/* 로컬위치 * 월드 */
	_matrix		ViewMatrixInverse;

	ViewMatrixInverse = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInverse = XMMatrixInverse(nullptr, ViewMatrixInverse);

	//D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInverse);
	//D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInverse);

	m_vRayDir = XMVector3TransformNormal(m_vRayDir, ViewMatrixInverse);
	m_vRayPos = XMVector3TransformCoord(m_vRayPos, ViewMatrixInverse);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPicking::Transform_ToLocalSpace(_matrix WorldMatrixInverse)
{
	/*D3DXVec3TransformNormal(&m_vRayDirInLocal, &m_vRayDir, &WorldMatrixInverse);
	D3DXVec3Normalize(&m_vRayDirInLocal, &m_vRayDirInLocal);
	D3DXVec3TransformCoord(&m_vRayPosInLocal, &m_vRayPos, &WorldMatrixInverse);*/

	m_vRayDirInLocal = XMVector3TransformNormal(m_vRayDir, WorldMatrixInverse);
	m_vRayDirInLocal = XMVector3Normalize(m_vRayDirInLocal);
	m_vRayPosInLocal = XMVector3TransformCoord(m_vRayPos, WorldMatrixInverse);

	return S_OK;
}

_bool CPicking::isPick(_float3* pLocalPoints, _float3 * pOut)
{
	//_float		fU, fV, fDist;
	/*if (true == D3DXIntersectTri(&pLocalPoints[0], &pLocalPoints[1], &pLocalPoints[2],
		&m_vRayPosInLocal, &m_vRayDirInLocal, &fU, &fV, &fDist))
	{
		_vector		vOut = m_vRayPosInLocal + m_vRayDirInLocal * fDist;
		_float3		Out;
		memcpy(&Out, &vOut, sizeof(_float3));

		*pOut = Out;
		return true;
	}
	else
	{
		return false;
	}*/

	return true;
}

void CPicking::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
