#include "..\Public\QuadTree.h"
#include "Frustum.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::NativeConstruct(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	/*
	6 7 8
	3 4 5
	0 1 2

	LT = 3 * 3 - 3
	RT = 3 * 3 - 1
	RB = 3 - 1
	LB = 0
	*/

	m_iCorner[CORNER_LT] = iNumVerticesX * iNumVerticesZ - iNumVerticesX;
	m_iCorner[CORNER_RT] = iNumVerticesX * iNumVerticesZ - 1;
	m_iCorner[CORNER_RB] = iNumVerticesX - 1;
	m_iCorner[CORNER_LB] = 0;

	/*
	6 7 8
	3 4 5
	0 1 2

	LC = (6 + 0) / 2
	TC = (6 + 8) / 2
	RC = (8 + 2) / 2
	BC = (0 + 2) / 2
	Center = (6 + 2) / 2
	*/

	//	Left Center, Top Center, Right Center, Bottom Center
	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
	iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
	iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
	iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;
	m_iCenter = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RB]) >> 1;

	/*
	LT TC RT
	LC  C RC
	LB BC RB
	*/
	//	자식 쿼드트리 생성
	m_pChild[CHILD_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	SetUp_Neighbor();

	return S_OK;
}

HRESULT CQuadTree::NativeConstruct(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorner[CORNER_LT] = iLT;
	m_iCorner[CORNER_RT] = iRT;
	m_iCorner[CORNER_RB] = iRB;
	m_iCorner[CORNER_LB] = iLB;

	//	정점과 정점의 차이가 1이면 더 이상 자식을 안만들어도 됨
	//	조건문에 걸리면 return S_OK로 재귀함수 탈출
	if (1 == iRT - iLT ||
		1 == iLT - iLB)
		return S_OK;

	/*
	6 7 8
	3 4 5
	0 1 2

	LC = (6 + 0) / 2
	TC = (6 + 8) / 2
	RC = (8 + 2) / 2
	BC = (0 + 2) / 2
	Center = (6 + 2) / 2
	*/

	//	Left Center, Top Center, Right Center, Bottom Center
	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
	iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
	iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
	iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;
	m_iCenter = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RB]) >> 1;

	/*
	LT TC RT
	LC  C RC
	LB BC RB
	*/
	//	자식 쿼드트리 생성, 재귀함수
	m_pChild[CHILD_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	return S_OK;
}

HRESULT CQuadTree::Culling(CFrustum * pFrustum, const _float3 * pVerticesPos, FACEINDICES32 * pIndices, _uint * pNumFace, _fvector vCamPosition)
{
	//	자식에 대해서 계속 비교하다가 끝나면
	if (nullptr == m_pChild[CHILD_LT] ||
		true == Draw_LOD(pVerticesPos, vCamPosition))
	{
		//	이웃이 없을 때 true 로 초기화
		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		//	내 주변 이웃들이 그려져있는 for 문 돌면서 체크
		for (_uint i = 0; i < NEIGHBOR_END; ++i)
		{
			//	이웃이 있으면
			if (nullptr != m_pNeighbor[i])
				isDraw[i] = m_pNeighbor[i]->Draw_LOD(pVerticesPos, vCamPosition);
		}

		_bool		isIn[4] = {
			//	pVerticesPos(정점의 위치)의 각 코너가 절두체 안에 있는지 확인
			//	판단하고자 하는 점의 위치(XMLoadFloat3(&pVerticesPos[m_iCorner])), fRange(디폴트 0)
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]]))
		};

		//	4개의 이웃이 싹 다 그려져 있는 상태라면
		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			//	삼각형 2개로 그림

			//	4개의 코너가 더이상 쪼개질수 없는 쿼드트리면 비교
			//	오른쪽 위 삼각형(3개 중의 하나가 들어와 있으면)
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RB];
				++(*pNumFace);
			}

			//	왼쪽 하단 삼각형
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_LB];
				++(*pNumFace);
			}

			//	더 할게 없으면 return S_OK;
			return S_OK;
		}

		//	위의 조건(4개의 이웃이 싹 다 그려져 있는 상태라면)가 하나라도 안맞으면
		//	4개 중 하나가 나보다 더 그려졌더라면(균열)

		//	Left Center, Top Center, Right Center, Bottom Center
		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
		iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
		iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
		iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;

		//	오른쪽 위 삼각형
		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			//	위쪽이 분열됐으면 삼각형 2개로 큰거 1개 그림
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = iTC;
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);

				pIndices[*pNumFace]._0 = m_iCenter;
				pIndices[*pNumFace]._1 = iTC;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RT];
				++(*pNumFace);
			}
			//	분열 안됐으면 삼각형 큰거 1개 그림
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);
			}

			//	오른쪽이 분열됐으면 삼각형 2개로 큰거 1개 그림
			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._1 = iRC;
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);

				pIndices[*pNumFace]._0 = m_iCenter;
				pIndices[*pNumFace]._1 = iRC;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RB];
				++(*pNumFace);
			}
			//	분열 안됐으면 삼각형 큰거 1개 그림
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);
			}
		}

		//	왼쪽 하단 삼각형
		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			//	왼쪽이 분열됐으면 삼각형 2개로 큰거 1개 그림
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = iLC;
				++(*pNumFace);

				pIndices[*pNumFace]._0 = iLC;
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_LB];
				++(*pNumFace);
			}
			//	분열 안됐으면 삼각형 큰거 1개 그림
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_LB];
				++(*pNumFace);
			}

			//	아래쪽이 분열됐으면 삼각형 2개로 큰거 1개 그림
			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LB];
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = iBC;
				++(*pNumFace);

				pIndices[*pNumFace]._0 = iBC;
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RB];
				++(*pNumFace);
			}
			//	분열 안됐으면 삼각형 큰거 1개 그림
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LB];
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RB];
				++(*pNumFace);
			}
		}
		return S_OK;
	}

	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	//	만약에 겹쳐 있다면(안에 있다면)
	//	pVerticesPos(정점의 위치)의 센터가 절두체 안에 있는지 확인
	//	판단하고자 하는 점의 위치(XMLoadFloat3(&pVerticesPos[m_iCenter])), fRadius
	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		//	자식에 대해서 또 비교
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFace, vCamPosition);
		}
	}

	return S_OK;
}

void CQuadTree::SetUp_Neighbor()
{
	//	자식 쿼드트리의 자식 쿼드트리가 nullptr 일 때 이웃 더이상 세팅하지 않고 나가기
	if (nullptr == m_pChild[CHILD_LT]->m_pChild[CHILD_LT])
		return;

	//	제일 큰 쿼드트리의 자식 쿼드트리의 이웃을 세팅
	//	생각 - Neighbor.png 참고
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];


	//	자식 쿼드트리의 자식 쿼드트리의 이웃을 세팅
	//	왼쪽 이웃이 있다면
	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		//	왼쪽위 자식의 왼쪽 이웃은 왼쪽 이웃의 오른쪽 위 자식
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	//	오른쪽 이웃이 있다면
	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	//	위쪽 이웃이 있다면
	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	//	아래쪽 이웃이 있다면
	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	for (auto& pChild : m_pChild)
	{
		if (nullptr != pChild)
			pChild->SetUp_Neighbor();
	}
}

_bool CQuadTree::Draw_LOD(const _float3 * pVerticesPos, _fvector vCamPosition)
{
	//	멀리 있는 지형은 그냥 큼직한 삼각형 2개로 그리고 가까이 있는건 쪼개져서 자세하게 그리자
	//	카메라로부터 절두체의 거리, 멀리 있는건 그리고 가까이 있는건 한번 더 쪼개서 그린다는 형식
	//	절두체(카메라까지의 거리)와 쿼드트리 정점에서의 거리, 쿼드트리 가로 너비
	//	쿼드트리의 너비가 더 크면 절두체가 안에 있다->가까이 있다 판단해서 더 쪼개서 그림

	//	캠위치에 VerticesPos의 센터를 뺀거의 거리를 구해 Length를 GetX 해야하는데 귀찮아서 m128_f32[0] << X
	_float		fDistance = XMVector3Length(vCamPosition - XMLoadFloat3(&pVerticesPos[m_iCenter])).m128_f32[0];

	//	가로너비(RT - LT)
	_float		fWidth = XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])).m128_f32[0];

	//	거리가 너비보다 클때 그린다
	//	약간의 조정 * 0.1f, 너무 빨리 통과해버려서 fDistance를 걍 1/10 줄임
	if (fDistance * 0.1f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	CQuadTree*	pInstance = new CQuadTree();

	if (FAILED(pInstance->NativeConstruct(iNumVerticesX, iNumVerticesZ)))
	{
		MSGBOX("Failed to Created CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*	pInstance = new CQuadTree();

	if (FAILED(pInstance->NativeConstruct(iLT, iRT, iRB, iLB)))
	{
		MSGBOX("Failed to Created CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pQuadTree : m_pChild)
		Safe_Release(pQuadTree);
}
