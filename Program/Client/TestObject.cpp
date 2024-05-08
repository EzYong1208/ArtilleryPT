#include "TestObject.h"

TestObject::TestObject()
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
	