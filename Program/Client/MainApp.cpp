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
	//MainApp::Release() �� static �Լ��̱� ������ �Ұ���.
	//_TestObject->Release();
}