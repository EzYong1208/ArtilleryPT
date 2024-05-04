#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

public:
	enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

public:
	//	Desc : Description, 현재 장치 객체의 정보
	//	장치 초기화할때 필요한 핸들, 화면 사이즈. 클라이언트에서 값을 받아오게 할려고
	typedef struct tagGraphicDeviceDesc
	{
		HWND				hWnd;
		unsigned int		iWinCX, iWinCY;
		WINMODE				eWinMode;
	}GRAPHICDESC;

private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
	// com 객체 : 언어와의 통신 규약을 갖고 하드웨어를 통제하여 각종 기능을 제공하는 객체

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
	//	메모리 할당(정점버퍼, 인덱스버퍼, 텍스쳐 로드), 컴 객체를 통해서 뭘 만드는 작업 등
	ID3D11Device*				m_pDevice = nullptr;

	//	바인딩 작업. 기능 실행(정점 버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture())
	//	장치에다가 미리 무언가를 할때의 작업, 그린다는 기능, 텍스쳐 바인딩 기능은 빠져있음(이건 쉐이더로 던져야함)
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	//	9때는 스스로 생성했었기 때문에 없었는데 11에서는 직접 만들어야함
	IDXGISwapChain*				m_pSwapChain = nullptr;

	//	ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;			//	스케치북(그림을 당할 수 있는 대상), 출력가능한 렌더타겟으로 설정하겠다(DXGI_USAGE_RENDER_TARGET_OUTPUT)
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

private:
	HRESULT						Ready_SwapChain(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT						Ready_BackBufferRenderTargetView();
	HRESULT						Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	virtual	void				Free()	override;
};

END