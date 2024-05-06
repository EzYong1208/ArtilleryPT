#include "MainApp.h"
#include "Loader.h"

HRESULT MainApp::Open_Level()
{
	Loader* pLoader = new Loader();

	pLoader->LoadingTest();

	return S_OK;
}

MainApp* MainApp::Create()
{
	MainApp* pInstance = new MainApp();

	pInstance->Open_Level();

	return pInstance;
}
