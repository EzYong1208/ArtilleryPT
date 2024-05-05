#include "MainApp.h"
#include "Loader.h"

HRESULT MainApp::Open_Level()
{
	CLoader* Loader = new CLoader();

	Loader->LoadingTest();

	return S_OK;
}

MainApp* MainApp::Create()
{
	MainApp* pInstance = new MainApp();

	pInstance->Open_Level();

	return pInstance;
}
