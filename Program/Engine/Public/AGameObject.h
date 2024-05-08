#pragma once

#include "Engine_Defines.h"
#include "AObject.h"
#include "GameInstance.h"

class AGameObject : public AObject
{
public:
	// TestObject���� ��ũ ������ ��� ���ܼ� ����� �����ڿ� Release �Լ��� ������.
	AGameObject() 
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_RenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

		RELEASE_INSTANCE(CGameInstance);
	};

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);

public:
	virtual void Release() override {};

protected:
	unique_ptr<CRenderObject> _RenderObject;
};
