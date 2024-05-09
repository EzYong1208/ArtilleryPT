#include "TestObject.h"

TestObject::TestObject()
{
	_RenderObject->SetCoord_Size(0.f, 0.f, 300.f, 300.f);
}

TestObject::~TestObject()
{
	_RenderObject->Free();
}

void TestObject::NativeConstruct()
{
	
}

void TestObject::Tick(_double TimeDelta)
{
}

void TestObject::LateTick(AGameObject* pRenderObject)
{
	__super::LateTick(pRenderObject);
}

void TestObject::Test()
{
	_RenderObject->Render();
}
	