#pragma once

#include "Engine_Defines.h"
#include "AObject.h"
#include "GameInstance.h"

BEGIN(Engine)

class AGameObject : public AObject
{
public:
	// TestObject에서 링크 오류가 계속 생겨서 헤더에 생성자와 Release 함수를 구현함.
	AGameObject() 
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_RenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

		RELEASE_INSTANCE(CGameInstance);
	};

public:
	void Render();

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);

protected:
	unique_ptr<CRenderObject> _RenderObject;
};

END