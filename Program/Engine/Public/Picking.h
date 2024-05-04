#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)

private:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT					NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd);

public:
	HRESULT					Transform_ToWorldSpace();
	HRESULT					Transform_ToLocalSpace(_matrix WorldMatrixInverse);

	_bool					isPick(_float3* pLocalPoints, _float3* pOut = nullptr);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HWND					m_hWnd;

private:
	_vector					m_vRayDir;
	_vector					m_vRayPos;

	_vector					m_vRayDirInLocal;
	_vector					m_vRayPosInLocal;

public:
	virtual void			Free() override;
};

END
