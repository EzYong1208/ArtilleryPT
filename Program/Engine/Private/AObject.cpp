#include "AObject.h"
#include "RenderObject.h"
#include "GameInstance.h"

void AObject::Test()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RenderObject_ptr = CRenderObject::Create(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

	RELEASE_INSTANCE(CGameInstance);
}

void AObject::SetTexture(string TextureKey)
{
	// RenderObject�� SetTexture ȣ��
	RenderObject_ptr->SetTexture(TextureKey);
}

void AObject::SetTransform(_float4x4 Transform)
{
	// RenderObject�� SetTransform ȣ��
	RenderObject_ptr->SetTransform(Transform);
}
