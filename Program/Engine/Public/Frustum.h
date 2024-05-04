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
	//	내가 판별하기 위한 객체의 위치 _fvector vPoint
	_bool			isIn_WorldSpace(_fvector vPoint, _float fRange = 0.0f);
	_bool			isIn_LocalSpace(_fvector vPoint, _float fRange = 0.0f);

private:
	_float3			m_vPoint[8];				//	절두체를 구성하기 위한 점 8개
	_float3			m_vPointInWorld[8];			//	월드 스페이스 상의 점 8개
	_float3			m_vPointInLocal[8];			//	로컬 스페이스 상의 점 8개

	_float4			m_PlaneInWorld[6];			//	월드 스페이스 상의 면 6개
	_float4			m_PlaneInLocal[6];			//	로컬 스페이스 상의 면 6개
		
private:
	//	점 8개를 넣어주면 평면 6개를 구해서 리턴(결과 pPlanes)
	void			Compute_Plane(const _float3* pPoints, _float4* pPlanes);

public:
	virtual void	Free() override;
};

END