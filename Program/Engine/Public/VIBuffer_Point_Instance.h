#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer
{
public:
	typedef struct tagParticleDesc
	{
		_float		fMinSpeed, fMaxSpeed;
		_float3		vMoveDir;
	}PARTICLEDESC;

public:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(class CShader* pShader, _uint iPassIndex);

public:
	void Update(_double TimeDelta);

private:
	/* 인스턴스하나당 표현되어야할 행렬을 정점으로서 인스턴스의 갯수만큼 가지고 있는 정점 버퍼 . */
	ID3D11Buffer*				m_pVBInstance;
	D3D11_BUFFER_DESC			m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA		m_VBInstSubResource;
	_uint						m_iNumInstance = 0;

	PARTICLEDESC				m_ParticleDesc;
	_float*						m_pSpeeds = nullptr;

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END