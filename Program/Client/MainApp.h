#pragma once

#include "Base.h"

class MainApp
{
public:
	MainApp();
	~MainApp();

private:
	//	ó�� ������ ���� ���� ����
	HRESULT Open_Level();

private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

public:
	//	static �Լ��̱� ������ ��������� ����� �� ����
	static	MainApp* Create();
	static void Release();
};
