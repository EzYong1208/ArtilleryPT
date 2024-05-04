#include "..\Public\Cell.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::NativeConstruct(const _float3 * pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
	
	//	점 B에서 A를 뺀게 LINE_AB
	XMStoreFloat3(&m_vLineDir[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	//	점 C에서 B를 뺀게 LINE_BC
	XMStoreFloat3(&m_vLineDir[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	//	점 A에서 C를 뺀게 LINE_CA
	XMStoreFloat3(&m_vLineDir[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

	m_iIndex = iIndex;

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	//	같다 비교함수 XMVector3Equal (== 안됨)
	//	A가 같으면서 나의 B 또는 C가 같으면 true, 하나도 안걸리면 false
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int * pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		//	코사인의 결과를 알아야해서 둘다 Normalize
		_vector		vDestDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		//	내적하기 위한 vSourDir, 수직인거 구할려면 x랑 z 자리 스왑하고 x자리에 - 붙이면 됨
		_vector		vSourDir = XMVector3Normalize(XMLoadFloat4(&_float4(m_vLineDir[i].z * -1.f, 0.f, m_vLineDir[i].x, 0.f)));

		//	내적 XMVector3Dot(), 실수 결과지만 vector 리턴할테니 GetX
		//	코사인 그래프로는 양의 수가 나왔을때 밖에 나갔다는 조건
		//	밖으로 나갔을 때
		if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vSourDir)))
		{
			//	음수가 아닌 경우(이웃이 있다), 이때면 해당 인덱스를 그대로 대입
			if (0 <= m_iNeighbor[i])
				*pNeighborIndex = m_iNeighbor[i];

			//	이웃이 없으면 false
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(CVIBuffer_Triangle * pVIBuffer, CShader * pShader, _float4 vColor)
{
	//	Render에서 받아오고 Render 안에서 얘가 들고있는 정점정보로 업데이트 하는 작업하고 VIBuffer의 Render를 호출
	pVIBuffer->Update(m_vPoints);

	pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));

	if (FAILED(pVIBuffer->Render(pShader, 0)))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		MSGBOX("Failed to Creating CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
