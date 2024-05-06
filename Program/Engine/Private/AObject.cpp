#include "AObject.h"
#include "GameInstance.h"

void AObject::Test()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//RenderObject_ptr = CRenderObject::Create(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());
	_RenderObject = make_unique<CRenderObject>(pGameInstance->Get_Device(), pGameInstance->Get_DeviceContext());

	RELEASE_INSTANCE(CGameInstance);

	// ���� ������ GameInstance���� Release�� �����ϱ� ����
	//_RenderObject->Free();
}

void AObject::SetTexture(string TextureKey)
{
	// RenderObject�� SetTexture ȣ��
	_RenderObject->SetTexture(TextureKey);
}

void AObject::SetTransform(_float4x4 Transform)
{
	// RenderObject�� SetTransform ȣ��
	_RenderObject->SetTransform(Transform);
}
