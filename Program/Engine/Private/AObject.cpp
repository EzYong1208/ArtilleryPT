#include "AObject.h"
#include "GameInstance.h"

void AObject::Test()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//RenderObject_ptr = CRenderObject::Create(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());
	_RenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

	RELEASE_INSTANCE(CGameInstance);

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
