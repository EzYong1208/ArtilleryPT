#pragma once

#include "MainApp.h"

//	LEVEL_STATIC -> ������ ������� ��� �������� ���� ��ü�� (�÷��̾�, ȭ��ǥ ǥ�� �̹��� ��)
enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TEST, LEVEL_END };

class Loader
{
private:
	explicit Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	~Loader() = default;

public:
	CRITICAL_SECTION Get_CS() { return CS; }
	LEVEL Get_LevelID() const { return eLevel; }
	_bool IsFinished() const { return isFinished; }

public:
	HRESULT	NativeConstruct(LEVEL eNextLevel);		//	������ ����, ������ �Լ� ����
	HRESULT LoadingTest();

public:
	HRESULT	Add_Prototype_Textures();

public:
	string Get_ResourceMap_Path(string PrototypeTag);

private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	LEVEL eLevel = LEVEL_END;
	_bool isFinished = false;

	HANDLE hThread;		//	������ �ڵ�
	CRITICAL_SECTION CS;

private:
	map<string, string> ResourceMap;

public:
	static Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	void Free();

};

