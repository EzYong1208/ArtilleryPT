#pragma once

#include "Base.h"

//	지형 최적화를 위해 지형 만들면 쿼드트리 하나 만들 생각
//	가장 큰 지형의 정보를 표현한 쿼드트리
//	4개의 자식을 가짐, m_pChild 4개

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	//	왼쪽 위 LT(Left Top), 오른쪽 위 RT(Right Top), 오른쪽 아래 RB(Right Bottom), 왼쪽 아래 LB(Left Bottom)
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
	//	가장 큰 지형을 생성, (지형 사각형 점)4개 코너를 구성하고 있었던 정점의 인덱스를 알고 있으면 그 점 2개의 반 하면 가운데 정점 알수 있고
	//	갯수만 던져놔도 알수 있다(가장 큰 지형이니깐 왼쪽 아래가 원점, 갯수를 알면 다른 점도 알수 있으니)
	static	CQuadTree*		Create(_uint iNumVerticesX, _uint iNumVerticesZ);
	static	CQuadTree*		Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void			Free() override;
};

END