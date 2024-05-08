#pragma once

#include "Engine_Defines.h"
#include "AObject.h"
#include "GameInstance.h"

BEGIN(Engine)

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
	virtual void Tick(_double TimeDelta) {};
	virtual void LateTick(AGameObject* pRenderObject) { /*_RenderObject->Add_RenderGroup(pRenderObject);*/ };
	void Render();

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);

protected:
	unique_ptr<CRenderObject> _RenderObject;
};

END