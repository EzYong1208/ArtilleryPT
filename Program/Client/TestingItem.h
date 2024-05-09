#pragma once
#include "AGameObject.h"

class TestingItem : public AGameObject
{
public:
	TestingItem();
	~TestingItem();

public:
	void NativeConstruct();
	void Tick(_double deltaTime) override;
	void LateTick(AGameObject* pRenderObject) override;
};

