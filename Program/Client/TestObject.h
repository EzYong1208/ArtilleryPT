#pragma once

#include "AGameObject.h"

class TestObject : public AGameObject
{
public:
	TestObject();

public:
	void Test();

public:
	static unique_ptr<TestObject> Create();
	virtual void Release() override;
};
