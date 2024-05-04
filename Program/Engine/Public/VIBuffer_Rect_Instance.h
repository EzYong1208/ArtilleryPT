#pragma once

#include "VIBuffer.h"

//	인스턴스 하나 당 표현되어야 할 행렬을 정점으로서 인스턴스의 갯수만큼 가지고 있는 정점 버퍼

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer
{
public:
	typedef struct tagParitcleDesc
	{
		_float		fMinSpeed, fMaxSpeed;
		_float3		vMoveDir;
	}PARTICLEDESC;

public:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual	HRESULT						NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT						NativeConstruct(void* pArg) override;
	virtual HRESULT						Render(class CShader* pShader, _uint iPassIndex);

public:
	void								Update(_double TimeDelta);

private:
	//	인스턴스의 갯수 == m_pVBInstance에 추가적인 버텍스 버퍼의 갯수
	ID3D11Buffer*						m_pVBInstance;
	D3D11_BUFFER_DESC					m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA				m_VBInstSubResource;
	//	몇개의 인스턴스
	_uint								m_iNumInstance = 0;

	PARTICLEDESC						m_ParticleDesc;
	_float*								m_pSpeeds = nullptr;

public:
	static CVIBuffer_Rect_Instance*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual	CComponent*					Clone(void* pArg);
	virtual void						Free() override;
};

END