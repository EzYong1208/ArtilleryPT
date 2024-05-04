#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	//	높낮이 pHeightMap(맵툴로 처리하는게 좋지 않을까)
	virtual HRESULT				NativeConstruct_Prototype(const _tchar* pHeightMap);
	virtual HRESULT				NativeConstruct(void* pArg) override;

public:
	//	지형을 그리고 있는 TransformCom 을 가져오자
	void						Culling(class CTransform* pTransform);

private:
	//	가로 정점 갯수, 세로 정점갯수를 보관하기 위한 m_iNumVerticesX, Z
	_uint						m_iNumVerticesX = 0;
	_uint						m_iNumVerticesZ = 0;
	FACEINDICES32*				m_pIndices = nullptr;
	class CQuadTree*			m_pQuadTree = nullptr;

public:
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMap);
	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;
};

END