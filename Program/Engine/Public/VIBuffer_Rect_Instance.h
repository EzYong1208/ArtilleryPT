#pragma once

#include "VIBuffer.h"

//	�ν��Ͻ� �ϳ� �� ǥ���Ǿ�� �� ����� �������μ� �ν��Ͻ��� ������ŭ ������ �ִ� ���� ����

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
	//	�ν��Ͻ��� ���� == m_pVBInstance�� �߰����� ���ؽ� ������ ����
	ID3D11Buffer*						m_pVBInstance;
	D3D11_BUFFER_DESC					m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA				m_VBInstSubResource;
	//	��� �ν��Ͻ�
	_uint								m_iNumInstance = 0;

	PARTICLEDESC						m_ParticleDesc;
	_float*								m_pSpeeds = nullptr;

public:
	static CVIBuffer_Rect_Instance*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual	CComponent*					Clone(void* pArg);
	virtual void						Free() override;
};

END