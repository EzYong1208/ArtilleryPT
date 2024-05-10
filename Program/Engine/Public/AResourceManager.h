#pragma once

#include "Engine_Defines.h"
#include "AObject.h"

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
	GameError Add_Textures();

private:
	map<string, string> ResourceMap;
};
