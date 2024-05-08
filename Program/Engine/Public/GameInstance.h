#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Timer_Manager.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Key_Manager.h"
#include "Font_Manager.h"
#include "AResourceManager.h"
#include "RenderObject.h"

//	�������� ���(Ŭ���̾�Ʈ�� �����ֱ� ����) �Ŵ��� Ŭ�������� ������
//	���̺귯�� ũ�⸦ ���ҽ�ų �� �ִ�? ����ڰ� ���� ����� �Լ��鸸 open�Ϸ���(open �ϰ� �����͸� ����)

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

public:
	CGameInstance();
	virtual	~CGameInstance() = default;

public:
	//	������ �ʱ�ȭ�ϴ� �Լ����� �߰�, ��ġ �ʱ�ȭ
	//	��ġ �ʱ�ȭ�� ���� �׷��� ����̽� ���� ����ü�� ���ڰ����� �־���
	HRESULT				Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	_int				Tick_Engine(_double TimeDelta);
	HRESULT				Clear_LevelResource(_uint iLevelIndex);

public:	//	For.Graphic_Device
	HRESULT				Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT				Clear_DepthStencil_View();
	HRESULT				Present();

public:	//	For.Input_Device
	_byte				Get_DIKeyState(_ubyte eKeyID);
	_long				Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState);
	_byte				Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);

public:	//	For.Timer_Manager
	_float				Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT				Add_Timer(const _tchar* pTimerTag);

public:	//	For.Component_Manager
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent*			Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	HRESULT				AddRenderDesc(_uint iLevelIndex);

public: //	For.PipeLine
	HRESULT				Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);
	_matrix				Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4			Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4			Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_vector				Get_CamPosition();

public: //	For.Key_Manger
	bool				Key_Pressing(_int _key);
	bool				Key_Down(_int _key);
	bool				Key_Up(_int _key);

public:	//	For.Font_Manager
	HRESULT				Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT				Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor);

private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CKey_Manager*				m_pKey_Manager = nullptr;
	CFont_Manager*				m_pFont_Manager = nullptr;
	unique_ptr<AResourceManager> _AResourceManager;

public:
	//	�Ҵ��س��� ��� �Ŵ������� �� �����Ϸ��� �Լ�
	static	void		Release_Engine();
	virtual	void		Free()	override;

	ID3D11Device*								Get_Device() { return m_pGraphic_Device->Get_Device(); }
	ID3D11DeviceContext*						Get_DeviceContext() { return m_pGraphic_Device->Get_DeviceContext(); }
	IDXGISwapChain*								Get_SwapChain() { return m_pGraphic_Device->Get_SwapChain(); }

	map<const _tchar*, class CComponent*>*		Get_Prototype_Components(); // Component_Manager ���� ������Ʈ ������Ÿ�� �� ������ �Լ�
};

END