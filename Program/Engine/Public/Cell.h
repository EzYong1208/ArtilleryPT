#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	_uint				Get_Index() { return m_iIndex; }
	_vector				Get_Point(POINTS ePoint) { return XMLoadFloat3(&m_vPoints[ePoint]); }
	void				Set_NeighborIndex(LINES eLine, _int iNeighborIndex) { m_iNeighbor[eLine] = iNeighborIndex; }

public:
	HRESULT				NativeConstruct(const _float3* pPoints, _uint iIndex);
	_bool				Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool				isIn(_fvector vPosition, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	//	렌더 함수 안에서 버퍼를 갱신할려고 인자를 받아옴
	HRESULT				Render(class CVIBuffer_Triangle* pVIBuffer, class CShader* pShader, _float4 vColor);
#endif // _DEBUG


private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

private:
	_int					m_iNeighbor[LINE_END] = { -1, -1, -1 };		//	이웃 셀의 인덱스
	_float3					m_vPoints[POINT_END];	//	셀의 점
	_float3					m_vLineDir[LINE_END];	//	변을 이루고 있는 방향벡터 3개
	_uint					m_iIndex = 0;			//	셀들의 인덱스

public:
	static CCell*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _uint iIndex);
	virtual void		Free() override;
};

END