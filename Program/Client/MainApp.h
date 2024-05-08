#pragma once

#include "Base.h"

class TestObject;

class MainApp
{
public:
	MainApp();
	~MainApp();

private:
	//	ó�� ������ ���� ���� ����
	void Open_Level();

private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

public:
	//	static �Լ��̱� ������ ��������� ����� �� ����
	static void Create();
	static void Release();

private:
	unique_ptr<TestObject> _TestObject;
};
