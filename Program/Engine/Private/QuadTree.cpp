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
	//	�ڽ� ����Ʈ�� ����
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

	//	������ ������ ���̰� 1�̸� �� �̻� �ڽ��� �ȸ��� ��
	//	���ǹ��� �ɸ��� return S_OK�� ����Լ� Ż��
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
	//	�ڽ� ����Ʈ�� ����, ����Լ�
	m_pChild[CHILD_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	return S_OK;
}

HRESULT CQuadTree::Culling(CFrustum * pFrustum, const _float3 * pVerticesPos, FACEINDICES32 * pIndices, _uint * pNumFace, _fvector vCamPosition)
{
	//	�ڽĿ� ���ؼ� ��� ���ϴٰ� ������
	if (nullptr == m_pChild[CHILD_LT] ||
		true == Draw_LOD(pVerticesPos, vCamPosition))
	{
		//	�̿��� ���� �� true �� �ʱ�ȭ
		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		//	�� �ֺ� �̿����� �׷����ִ� for �� ���鼭 üũ
		for (_uint i = 0; i < NEIGHBOR_END; ++i)
		{
			//	�̿��� ������
			if (nullptr != m_pNeighbor[i])
				isDraw[i] = m_pNeighbor[i]->Draw_LOD(pVerticesPos, vCamPosition);
		}

		_bool		isIn[4] = {
			//	pVerticesPos(������ ��ġ)�� �� �ڳʰ� ����ü �ȿ� �ִ��� Ȯ��
			//	�Ǵ��ϰ��� �ϴ� ���� ��ġ(XMLoadFloat3(&pVerticesPos[m_iCorner])), fRange(����Ʈ 0)
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]]))
		};

		//	4���� �̿��� �� �� �׷��� �ִ� ���¶��
		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			//	�ﰢ�� 2���� �׸�

			//	4���� �ڳʰ� ���̻� �ɰ����� ���� ����Ʈ���� ��
			//	������ �� �ﰢ��(3�� ���� �ϳ��� ���� ������)
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_RB];
				++(*pNumFace);
			}

			//	���� �ϴ� �ﰢ��
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_LB];
				++(*pNumFace);
			}

			//	�� �Ұ� ������ return S_OK;
			return S_OK;
		}

		//	���� ����(4���� �̿��� �� �� �׷��� �ִ� ���¶��)�� �ϳ��� �ȸ�����
		//	4�� �� �ϳ��� ������ �� �׷��������(�տ�)

		//	Left Center, Top Center, Right Center, Bottom Center
		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
		iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
		iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
		iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;

		//	������ �� �ﰢ��
		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			//	������ �п������� �ﰢ�� 2���� ū�� 1�� �׸�
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
			//	�п� �ȵ����� �ﰢ�� ū�� 1�� �׸�
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);
			}

			//	�������� �п������� �ﰢ�� 2���� ū�� 1�� �׸�
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
			//	�п� �ȵ����� �ﰢ�� ū�� 1�� �׸�
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_RT];
				pIndices[*pNumFace]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFace]._2 = m_iCenter;
				++(*pNumFace);
			}
		}

		//	���� �ϴ� �ﰢ��
		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			//	������ �п������� �ﰢ�� 2���� ū�� 1�� �׸�
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
			//	�п� �ȵ����� �ﰢ�� ū�� 1�� �׸�
			else
			{
				pIndices[*pNumFace]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFace]._1 = m_iCenter;
				pIndices[*pNumFace]._2 = m_iCorner[CORNER_LB];
				++(*pNumFace);
			}

			//	�Ʒ����� �п������� �ﰢ�� 2���� ū�� 1�� �׸�
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
			//	�п� �ȵ����� �ﰢ�� ū�� 1�� �׸�
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

	//	���࿡ ���� �ִٸ�(�ȿ� �ִٸ�)
	//	pVerticesPos(������ ��ġ)�� ���Ͱ� ����ü �ȿ� �ִ��� Ȯ��
	//	�Ǵ��ϰ��� �ϴ� ���� ��ġ(XMLoadFloat3(&pVerticesPos[m_iCenter])), fRadius
	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		//	�ڽĿ� ���ؼ� �� ��
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
	//	�ڽ� ����Ʈ���� �ڽ� ����Ʈ���� nullptr �� �� �̿� ���̻� �������� �ʰ� ������
	if (nullptr == m_pChild[CHILD_LT]->m_pChild[CHILD_LT])
		return;

	//	���� ū ����Ʈ���� �ڽ� ����Ʈ���� �̿��� ����
	//	���� - Neighbor.png ����
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];


	//	�ڽ� ����Ʈ���� �ڽ� ����Ʈ���� �̿��� ����
	//	���� �̿��� �ִٸ�
	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		//	������ �ڽ��� ���� �̿��� ���� �̿��� ������ �� �ڽ�
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	//	������ �̿��� �ִٸ�
	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	//	���� �̿��� �ִٸ�
	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	//	�Ʒ��� �̿��� �ִٸ�
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
	//	�ָ� �ִ� ������ �׳� ŭ���� �ﰢ�� 2���� �׸��� ������ �ִ°� �ɰ����� �ڼ��ϰ� �׸���
	//	ī�޶�κ��� ����ü�� �Ÿ�, �ָ� �ִ°� �׸��� ������ �ִ°� �ѹ� �� �ɰ��� �׸��ٴ� ����
	//	����ü(ī�޶������ �Ÿ�)�� ����Ʈ�� ���������� �Ÿ�, ����Ʈ�� ���� �ʺ�
	//	����Ʈ���� �ʺ� �� ũ�� ����ü�� �ȿ� �ִ�->������ �ִ� �Ǵ��ؼ� �� �ɰ��� �׸�

	//	ķ��ġ�� VerticesPos�� ���͸� ������ �Ÿ��� ���� Length�� GetX �ؾ��ϴµ� �����Ƽ� m128_f32[0] << X
	_float		fDistance = XMVector3Length(vCamPosition - XMLoadFloat3(&pVerticesPos[m_iCenter])).m128_f32[0];

	//	���γʺ�(RT - LT)
	_float		fWidth = XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])).m128_f32[0];

	//	�Ÿ��� �ʺ񺸴� Ŭ�� �׸���
	//	�ణ�� ���� * 0.1f, �ʹ� ���� ����ع����� fDistance�� �� 1/10 ����
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
