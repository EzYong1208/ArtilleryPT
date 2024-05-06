#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Transform.h"
#include "VIBuffer_Triangle.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	//, m_pVIBuffer(rhs.m_pVIBuffer)
	//, m_pShader(rhs.m_pShader)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

	//Safe_AddRef(m_pVIBuffer);
	//Safe_AddRef(m_pShader);
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNaviDataFilePath, _uint iLevelIndex)
{
	if (iLevelIndex == m_iLevelStage01)
	{
		m_hFile = CreateFile(
			TEXT("../Bin/Data/Stage01.dat"),
			GENERIC_READ,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}
	
	if (iLevelIndex == m_iLevelStage02)
	{
		m_hFile = CreateFile(
			TEXT("../Bin/Data/Stage02.dat"),
			GENERIC_READ,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}

	if (iLevelIndex == m_iLevelBoss)
	{
		m_hFile = CreateFile(
			TEXT("../Bin/Data/Boss.dat"),
			GENERIC_READ,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}

	if (0 == m_hFile)
		return E_FAIL;

	_float3		vPoints[3];

	while (true)
	{
		ReadFile(m_hFile, vPoints, sizeof(_float3) * 3, &m_dwByte, nullptr);

		if (0 == m_dwByte)
			break;

		//	Index는 셀 컨테이너의 사이즈(m_Cells.size()), 아직 푸쉬 안했으니깐 0임
		CCell*		pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, _uint(m_Cells.size()));

		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(m_hFile);

	//	이웃을 세팅
	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

//#ifdef _DEBUG
//	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext);
//	if (nullptr == m_pVIBuffer)
//		return E_FAIL;
//
//	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
//	if (nullptr == m_pShader)
//		return E_FAIL;
//#endif // _DEBUG

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));

	m_iCurrentLevel = m_NaviDesc.iCurrentLevel;

	return S_OK;
}

//	_fvector vPosition : 움직이고 난 결과의 위치
_bool CNavigation::Move_OnNavigation(_fvector vPosition)
{
	_int		iNeighborIndex = -1;

	//	isIn()이 false == 밖으로 나갔음
	if (false == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
	{
		//	나간쪽에 이웃이 있다면 또 움직일 수 있음, 이웃이 없는 쪽으로 나가면 false
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				_int	iCurrentNeighborIndex = -1;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iCurrentNeighborIndex))
				{
					m_iNeighborIndex = m_Cells[iNeighborIndex]->Get_Index();

					if (m_iLevelStage02 == m_iCurrentLevel)
					{
						if (!m_bSwitchWallB)
						{
							if (160 == m_iNeighborIndex ||
								161 == m_iNeighborIndex ||
								162 == m_iNeighborIndex ||
								163 == m_iNeighborIndex ||
								164 == m_iNeighborIndex ||
								165 == m_iNeighborIndex ||
								166 == m_iNeighborIndex ||
								167 == m_iNeighborIndex)
							{
								return false;
							}
						}

						if (!m_bSwitchWallC)
						{
							if (168 == m_iNeighborIndex ||
								169 == m_iNeighborIndex ||
								170 == m_iNeighborIndex ||
								171 == m_iNeighborIndex ||
								172 == m_iNeighborIndex ||
								173 == m_iNeighborIndex ||
								174 == m_iNeighborIndex ||
								175 == m_iNeighborIndex)
							{
								return false;
							}
						}
					}

					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					break;
				}
				else
				{
					if (-1 == iCurrentNeighborIndex)
						return false;
				}

				iNeighborIndex = iCurrentNeighborIndex;
			}
			
			return true;
		}
		//	이웃이 없는 쪽으로 나갔다면
		else
		{
			//	또는 필요에 따라 여기서 슬라이딩을 구현(쌤이라면 CTransform에서 구현, bool 타입이 아니라 State로 구현해서 슬라이드인지 아닌지 판단해서 구현할 수 있을듯)
			return false;
		}
	}
	else
		return true;
}

_vector CNavigation::Get_Height(_fvector vPosition)
{
	_vector		vPlayerPos = vPosition;
	_float		fY = 0.f;
	_vector		vPlane;

	vPlane = XMPlaneFromPoints(m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A), m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B), m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C));

	fY = (-XMVectorGetX(vPlane) * XMVectorGetX(vPlayerPos)
		- XMVectorGetZ(vPlane) * XMVectorGetZ(vPlayerPos)
		- XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);

	vPlayerPos = XMVectorSetY(vPlayerPos, fY);

	return vPlayerPos;
}

//HRESULT CNavigation::Render(CTransform * pTerrainTransform)
//{
//	if (nullptr == m_pVIBuffer)
//		return E_FAIL;
//
//	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);
//
//	pTerrainTransform->Bind_OnShader(m_pShader, "g_WorldMatrix");
//	m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
//	m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
//
//	_uint		iIndex = 0;
//
//	if (-1 == m_NaviDesc.iCurrentIndex)
//	{
//		for (auto& pCell : m_Cells)
//		{
//			if (nullptr != pCell)
//				pCell->Render(m_pVIBuffer, m_pShader, iIndex++ == m_NaviDesc.iCurrentIndex ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(1.f, 1.f, 1.f, 1.f));
//		}
//	}
//	else
//		m_Cells[m_NaviDesc.iCurrentIndex]->Render(m_pVIBuffer, m_pShader, _float4(1.f, 0.f, 0.f, 1.f));
//
//	RELEASE_INSTANCE(CPipeLine);
//
//	return S_OK;
//}

HRESULT CNavigation::SetUp_Neighbor()
{
	//	벡터 컨테이너 2개 루프를 돌면서 같은 셀은 비교하지 않고 다른 셀에 같은 점이 있느냐 비교
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			//	비교할때 주소가 같으면 나가
			if (pSourCell == pDestCell)
				continue;

			//	pSourCell의 A, B 포인트를 넣고 pDestCell에 A, B가 있었다면 pSourCell에 라인 AB를 공유하는 이웃으로 pDestCell의 인덱스를 설정
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell->Get_Index());

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell->Get_Index());

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell->Get_Index());
		}
	}

	return S_OK;
}

void CNavigation::Set_SwitchWallB(_bool bMove)
{
	m_bSwitchWallB = bMove;
}

void CNavigation::Set_SwitchWallC(_bool bMove)
{
	m_bSwitchWallC = bMove;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNaviDataFilePath, _uint iLevelIndex)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNaviDataFilePath, iLevelIndex)))
	{
		MSGBOX("Failed to Creating CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	//Safe_Release(m_pVIBuffer);
	//Safe_Release(m_pShader);
}
