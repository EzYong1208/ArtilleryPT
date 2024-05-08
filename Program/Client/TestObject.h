#pragma once

#include "AGameObject.h"

class TestObject : public AGameObject
{
public:
	TestObject();

public:
	void Tick(_double deltaTime) override;
	void LateTick(AGameObject* pRenderObject) override;

public:
	void Test();
};
