#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

public:
	enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

public:
	//	Desc : Description, ���� ��ġ ��ü�� ����
	//	��ġ �ʱ�ȭ�Ҷ� �ʿ��� �ڵ�, ȭ�� ������. Ŭ���̾�Ʈ���� ���� �޾ƿ��� �ҷ���
	typedef struct tagGraphicDeviceDesc
	{
		HWND				hWnd;
		unsigned int		iWinCX, iWinCY;
		WINMODE				eWinMode;
	}GRAPHICDESC;

private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
	// com ��ü : ������ ��� �Ծ��� ���� �ϵ��� �����Ͽ� ���� ����� �����ϴ� ��ü

public:
	ID3D11Device*			Get_Device() { return m_pDevice; }
	ID3D11DeviceContext*	Get_DeviceContext() { return m_pDeviceContext; }
	IDXGISwapChain*			Get_SwapChain() { return m_pSwapChain; }

public:
	HRESULT						Ready_Graphic_Device(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT						Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT						Clear_DepthStencil_View();
	HRESULT						Present();

private:
	//	�޸� �Ҵ�(��������, �ε�������, �ؽ��� �ε�), �� ��ü�� ���ؼ� �� ����� �۾� ��
	ID3D11Device*				m_pDevice = nullptr;

	//	���ε� �۾�. ��� ����(���� ���۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture())
	//	��ġ���ٰ� �̸� ���𰡸� �Ҷ��� �۾�, �׸��ٴ� ���, �ؽ��� ���ε� ����� ��������(�̰� ���̴��� ��������)
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	//	9���� ������ �����߾��� ������ �����µ� 11������ ���� ��������
	IDXGISwapChain*				m_pSwapChain = nullptr;

	//	ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;			//	����ġ��(�׸��� ���� �� �ִ� ���), ��°����� ����Ÿ������ �����ϰڴ�(DXGI_USAGE_RENDER_TARGET_OUTPUT)
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

private:
	HRESULT						Ready_SwapChain(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT						Ready_BackBufferRenderTargetView();
	HRESULT						Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	virtual	void				Free()	override;
};

END