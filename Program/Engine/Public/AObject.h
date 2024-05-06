#pragma once

#include "Engine_Defines.h"
#include "RenderObject.h"

BEGIN(Engine)

class ENGINE_DLL AObject
{
public:
	void Test();

	//CRenderObject* RenderObject_ptr = nullptr;
	unique_ptr<CRenderObject> _RenderObject;

protected:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);

};

END