#include "AGameObject.h"

void AGameObject::SetTexture(string TextureKey)
{
	// RenderObject의 SetTexture 호출
	_RenderObject->SetTexture(TextureKey);
}

void AGameObject::SetTransform(_float4x4 Transform)
{
	// RenderObject의 SetTransform 호출
	_RenderObject->SetTransform(Transform);
}
