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

class ResourceManager final : public AObject
{
public:
	ResourceManager();

public:
	static unique_ptr<ResourceManager> Create();
	virtual void Release() override;

public:
	GameError Add_Prototype_Textures();

private:
	map<string, string> ResourceMap;
};
