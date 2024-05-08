#pragma once

#include "Engine_Defines.h"
#include "RenderObject.h"

BEGIN(Engine)

class ENGINE_DLL AObject
{
public:
	AObject();
	virtual ~AObject() = default;

public:
	virtual void Release() {};

};

END