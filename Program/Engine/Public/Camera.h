#pragma once

//	카메라의 기초적인 움직임 -> 클라이언트
//	뷰스페이스 변환행렬을 세팅하는걸 만드는걸->엔진

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float3						vEye;		//	카메라의 현재 월드 공간에서의 위치
		_float3						vAt;		//	월드 공간에서 바라보고 있는 점의 위치
		_float3						vAxisY;		//	y축 양의방향
		_float						fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC	TransformDesc;
	}CAMERADESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double TimeDelta);
	virtual	_int			LateTick(_double TimeDelta);
	virtual HRESULT			Render();

protected:
	CAMERADESC				m_CameraDesc;
	class CPipeLine*		m_pPipeLine = nullptr;

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;

	CTransform*				Get_Camera_Transform() { return m_pTransform; }

#if defined (USE_IMGUI)
public: /* ImGui 에서 사용할 함수들*/
	CAMERADESC&				Get_Camera_Desc() { return m_CameraDesc; }
#endif
};

END