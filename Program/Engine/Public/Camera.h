#pragma once

//	ī�޶��� �������� ������ -> Ŭ���̾�Ʈ
//	�佺���̽� ��ȯ����� �����ϴ°� ����°�->����

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float3						vEye;		//	ī�޶��� ���� ���� ���������� ��ġ
		_float3						vAt;		//	���� �������� �ٶ󺸰� �ִ� ���� ��ġ
		_float3						vAxisY;		//	y�� ���ǹ���
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
public: /* ImGui ���� ����� �Լ���*/
	CAMERADESC&				Get_Camera_Desc() { return m_CameraDesc; }
#endif
};

END