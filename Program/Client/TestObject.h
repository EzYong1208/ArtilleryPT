#pragma once

#include "AGameObject.h"

class TestObject : public AGameObject
{
public:
	TestObject();
	~TestObject();

public:
	void NativeConstruct();
	void Tick(_double deltaTime) override;
	void LateTick(AGameObject* pRenderObject) override;

	void SetCoord(_float fX, _float fY, _float fSizeX, _float fSizeY);
};
