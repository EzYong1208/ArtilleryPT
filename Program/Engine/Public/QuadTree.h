#pragma once

#include "Base.h"

//	���� ����ȭ�� ���� ���� ����� ����Ʈ�� �ϳ� ���� ����
//	���� ū ������ ������ ǥ���� ����Ʈ��
//	4���� �ڽ��� ����, m_pChild 4��

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	//	���� �� LT(Left Top), ������ �� RT(Right Top), ������ �Ʒ� RB(Right Bottom), ���� �Ʒ� LB(Left Bottom)
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT					NativeConstruct(_uint iNumVerticesX, _uint iNumVerticesZ);
	HRESULT					NativeConstruct(_uint iLT, _uint iRT, _uint iRB, _uint iLB);

public:
	HRESULT					Culling(class CFrustum* pFrustum, const _float3* pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFace, _fvector vCamPosition);

private:
	CQuadTree*				m_pChild[CHILD_END] = { nullptr };
	CQuadTree*				m_pNeighbor[NEIGHBOR_END] = { nullptr };
	_uint					m_iCorner[CORNER_END] = { 0 };
	_uint					m_iCenter = 0;

private:
	void					SetUp_Neighbor();
	_bool					Draw_LOD(const _float3* pVerticesPos, _fvector vCamPosition);

public:
	//	���� ū ������ ����, (���� �簢�� ��)4�� �ڳʸ� �����ϰ� �־��� ������ �ε����� �˰� ������ �� �� 2���� �� �ϸ� ��� ���� �˼� �ְ�
	//	������ �������� �˼� �ִ�(���� ū �����̴ϱ� ���� �Ʒ��� ����, ������ �˸� �ٸ� ���� �˼� ������)
	static	CQuadTree*		Create(_uint iNumVerticesX, _uint iNumVerticesZ);
	static	CQuadTree*		Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void			Free() override;
};

END