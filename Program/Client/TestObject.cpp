#include "TestObject.h"

TestObject::TestObject()
{

}

TestObject::~TestObject()
{
	_RenderObject->Free();
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
	