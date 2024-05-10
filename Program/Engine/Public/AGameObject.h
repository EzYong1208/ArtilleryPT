#pragma once

#include "Engine_Defines.h"
#include "AObject.h"
#include "GameInstance.h"

BEGIN(Engine)

class AGameObject : public AObject
{
public:
	// TestObject에서 링크 오류가 계속 생겨서 헤더에 생성자 함수를 구현함.
	AGameObject() 
		: isDead(false)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		unique_ptr<CRenderObject> pRenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());
		_RenderObject = move(pRenderObject);

		RELEASE_INSTANCE(CGameInstance);
	};

public:
	virtual void Tick(_double TimeDelta) {};
	virtual void LateTick(AGameObject* pRenderObject) { _RenderObject->Add_RenderGroup(pRenderObject); };
	void Render();

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);
	void SetAGameObjectDead() {
		isDead = true;
	}

	bool isAGameObjectDead() const {
		return isDead;
	}

protected:
	unique_ptr<CRenderObject> _RenderObject;
	bool isDead;
};

END