#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)

private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT			Initialize();
	void			Update();
	HRESULT			Transform_ToLocalSpace(class CTransform* pTransform);

public:
	//	���� �Ǻ��ϱ� ���� ��ü�� ��ġ _fvector vPoint
	_bool			isIn_WorldSpace(_fvector vPoint, _float fRange = 0.0f);
	_bool			isIn_LocalSpace(_fvector vPoint, _float fRange = 0.0f);

private:
	_float3			m_vPoint[8];				//	����ü�� �����ϱ� ���� �� 8��
	_float3			m_vPointInWorld[8];			//	���� �����̽� ���� �� 8��
	_float3			m_vPointInLocal[8];			//	���� �����̽� ���� �� 8��

	_float4			m_PlaneInWorld[6];			//	���� �����̽� ���� �� 6��
	_float4			m_PlaneInLocal[6];			//	���� �����̽� ���� �� 6��
		
private:
	//	�� 8���� �־��ָ� ��� 6���� ���ؼ� ����(��� pPlanes)
	void			Compute_Plane(const _float3* pPoints, _float4* pPlanes);

public:
	virtual void	Free() override;
};

END