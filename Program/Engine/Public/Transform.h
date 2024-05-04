#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = 0.f;
		_float		fRotationPerSec = 0.f;
		_float		fScalingPerSec = 0.f;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector					Get_Scale(STATE eState) const;
	_vector					Get_State(STATE eState) const;

	_matrix					Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_matrix					Get_WorldMatrixInverse() const { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
	_float4x4				Get_WorldFloat4x4() const { return m_WorldMatrix; }

public:
	void					Set_State(STATE eState, _fvector vState);
	void					Set_TransformDesc(const TRANSFORMDESC& TransformDesc);
	void					Set_WorldMatrix(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;

public:
	HRESULT					Bind_OnShader(class CShader* pShader, const char* pValueName);

public:
	HRESULT					Go_Straight(_double TimeDelta, class CNavigation* pNavigation = nullptr);
	HRESULT					Go_Right(_double TimeDelta);
	HRESULT					Go_Left(_double TimeDelta);
	HRESULT					Go_Backward(_double TimeDelta, class CNavigation* pNavigation = nullptr);

	HRESULT					Turn(_fvector vAxis, _double TimeDelta);
	HRESULT					Rotation(_fvector vAxis, _float fRadian);

	//	어떤 스케일 값으로 고정시키겠다
	HRESULT					Scaled(_fvector vScale);
	//	점차 증가시키겠다
	HRESULT					Scaling(_double TimeDelta);
	HRESULT					Chase(_fvector vTargetPos, _double TimeDelta);
	HRESULT					LookAt(_fvector vTargetPos, _double TimeDelta);

private:
	TRANSFORMDESC			m_TransformDesc;
	_float4x4				m_WorldMatrix;

public:
	static CTransform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*		Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END