#pragma once

#include "AObject.h"

BEGIN(Engine)

enum class GameError
{
	Success,
	HaveNoResource,
	ObjectCreateFail,
	MAX
};

class AResourceManager final : public AObject
{
public:
	AResourceManager();

public:
	GameError Add_Prototype_Textures();

private:
	map<string, string> ResourceMap;
};

END