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
	_TestObject = make_unique<TestObject>();
}

void MainApp::Create()
{
}

void MainApp::Release()
{
	//MainApp::Release() �� static �Լ��̱� ������ �Ұ���.
	//_TestObject->Release();
}