#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CRenderObject;

class ENGINE_DLL AObject
{
public:
	void Test();

	CRenderObject* RenderObject_ptr = nullptr;

protected:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);

};

END