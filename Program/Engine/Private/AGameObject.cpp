#include "AGameObject.h"

void AGameObject::SetTexture(string TextureKey)
{
	// RenderObject�� SetTexture ȣ��
	_RenderObject->SetTexture(TextureKey);
}

void AGameObject::SetTransform(_float4x4 Transform)
{
	// RenderObject�� SetTransform ȣ��
	_RenderObject->SetTransform(Transform);
}
