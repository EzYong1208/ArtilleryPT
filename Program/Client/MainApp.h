#pragma once

#include "Base.h"

class TestObject;

class MainApp
{
public:
	MainApp();
	~MainApp();

private:
	//	처음 레벨을 뭐로 할지 정함
	void Open_Level();

private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

public:
	//	static 함수이기 때문에 멤버변수를 사용할 수 없다
	static void Create();
	static void Release();

private:
	unique_ptr<TestObject> _TestObject;
};
