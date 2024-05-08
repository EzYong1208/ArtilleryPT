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
	//MainApp::Release() 가 static 함수이기 때문에 불가능.
	//_TestObject->Release();
}