#include "MainApp.h"
#include "TestObject.h"

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
}

void MainApp::Open_Level()
{
	_TestObject = TestObject::Create();
	_TestObject->Test();
}

MainApp* MainApp::Create()
{
	//MainApp* pInstance = new MainApp();

	//pInstance->Open_Level();

	//return pInstance;
	return nullptr;
}

void MainApp::Release()
{
	//MainApp::Release() 가 static 함수이기 때문에 불가능.
	//_TestObject->Release();
}