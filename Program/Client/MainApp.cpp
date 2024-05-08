#include "MainApp.h"
#include "TestObject.h"
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

	if (FAILED(_GameInstance->Initialize_Engine(_hInst, 0, GraphicDesc, &_Device, &_DeviceContext)))
		return MainAppError::InitializeEngine_Failed;

	_TestObject = make_unique<TestObject>();

	return MainAppError::Success;
}

void MainApp::Tick(_double TimeDelta)
{
	_TestObject->Tick(TimeDelta);
}

void MainApp::LateTick()
{
	_TestObject->LateTick(_TestObject.get());
}

void MainApp::Render()
{
	_GameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 1.f, 1.f));
	_GameInstance->Clear_DepthStencil_View();

	_Renderer->Render();

	_GameInstance->Present();
}

void MainApp::Open_Level()
{

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
	//MainApp::Release() 가 static 함수이기 때문에 불가능.
	//_TestObject->Release();

	Safe_Release(_Device);
	Safe_Release(_DeviceContext);
	Safe_Release(_GameInstance);

	CGameInstance::Release_Engine();

	delete this;
}