#include "MainApp.h"
#include "GameInstance.h"

MainApp::MainApp()
	: _GameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(_GameInstance);
}

MainApp::~MainApp()
{
}

MainAppError MainApp::NativeConstruct()
{
	CGraphic_Device::GRAPHICDESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = _hWnd;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDesc.iWinCX = 1280;
	GraphicDesc.iWinCY = 720;

	if (FAILED(_GameInstance->Initialize_Engine(_hInst, 1, GraphicDesc, &_Device, &_DeviceContext)))
		return MainAppError::InitializeEngine_Failed;

	Ready_Prototype();

	return MainAppError::Success;
}

void MainApp::Tick(_double TimeDelta)
{

}

void MainApp::LateTick()
{

}

void MainApp::Render()
{
	// ���� �� ������� ������ Ŭ������
	// ������ ������ ����, ���� _float4�� ������ ����
	_GameInstance->Clear_BackBuffer_View(_float4(0.49f, 0.49f, 0.49f, 1.f));
	// ���̽��ٽ� ����
	_GameInstance->Clear_DepthStencil_View();

	// ������
	_Renderer->Render();

	// �ٱ׷����� ���۸� ���� �ø��� ü����(����ü��)
	_GameInstance->Present();
}

void MainApp::Open_Level()
{

}

void MainApp::Ready_Prototype()
{
	//	For.Prototype_Component_Renderer
	_GameInstance->Add_Prototype(
		0,
		TEXT("Prototype_Component_Renderer"),
		_Renderer = CRenderer::Create(_Device, _DeviceContext));

	_GameInstance->AddRenderDesc(0);

	//	For.Prototype_Component_Transform
	_GameInstance->Add_Prototype(
		0,
		TEXT("Prototype_Component_Transform"),
		CTransform::Create(_Device, _DeviceContext));

	// For.Prototype_Component_Shader_VTXTEX
	_GameInstance->Add_Prototype(
		0,
		TEXT("Prototype_Component_Shader_VTXTEX"),
		CShader::Create(_Device, _DeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements));

	// For.Prototype_Component_VIBuffer_Rect
	_GameInstance->Add_Prototype(
		0,
		TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(_Device, _DeviceContext));

	Safe_AddRef(_Renderer);
}

MainApp* MainApp::Create()
{
	MainApp* pInstance = new MainApp();

	MainAppError Result = pInstance->NativeConstruct();

	if (Result != MainAppError::Success)
	{
		pInstance->Release();
	}

	return pInstance;
}

void MainApp::Release()
{
	Safe_Release(_Renderer);
	Safe_Release(_Device);
	Safe_Release(_DeviceContext);
	Safe_Release(_GameInstance);

	CGameInstance::Release_Engine();

	delete this;
}