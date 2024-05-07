#include "AObject.h"
#include "GameInstance.h"

AObject::AObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_RenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

	RELEASE_INSTANCE(CGameInstance);
}

void AObject::Test()
{
	// 앱이 끝나고 GameInstance에서 Release를 실행하기 전에
	//_RenderObject->Free();
}

void AObject::SetTexture(string TextureKey)
{
	// RenderObject의 SetTexture 호출
	_RenderObject->SetTexture(TextureKey);
}

void AObject::SetTransform(_float4x4 Transform)
{
	// RenderObject의 SetTransform 호출
	_RenderObject->SetTransform(Transform);
}
