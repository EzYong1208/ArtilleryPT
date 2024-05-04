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
	//	������ pHeightMap(������ ó���ϴ°� ���� ������)
	virtual HRESULT				NativeConstruct_Prototype(const _tchar* pHeightMap);
	virtual HRESULT				NativeConstruct(void* pArg) override;

public:
	//	������ �׸��� �ִ� TransformCom �� ��������
	void						Culling(class CTransform* pTransform);

private:
	//	���� ���� ����, ���� ���������� �����ϱ� ���� m_iNumVerticesX, Z
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