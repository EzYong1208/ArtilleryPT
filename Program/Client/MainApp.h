#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

extern HINSTANCE _hInst;
extern HWND _hWnd;

enum class MainAppError
{
	Success,
	InitializeEngine_Failed,
	Ready_PrototypeComponent_Failed,
	Ready_PrototypeGameObject_Failed,
	Open_Level_Failed,
	MAX
};

class MainApp
{
public:
	MainApp();
	~MainApp();

public:
	MainAppError NativeConstruct();
	void Tick(_double TimeDelta);
	void LateTick();
	void Render();

private:
	//	ó�� ������ ���� ���� ����
	void Open_Level();
	void Ready_Prototype();

private:
	ID3D11Device* _Device = nullptr;
	ID3D11DeviceContext* _DeviceContext = nullptr;
	CGameInstance* _GameInstance = nullptr;
	CRenderer* _Renderer = nullptr;

public:
	//	static �Լ��̱� ������ ��������� ����� �� ����
	static MainApp* Create();
	void Release();
};
