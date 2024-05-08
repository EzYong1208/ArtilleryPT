#include "TestObject.h"

TestObject::TestObject()
{

}

void TestObject::Test()
{
	_RenderObject->Render();
}

unique_ptr<TestObject> TestObject::Create()
{
	return make_unique<TestObject>();
}

void TestObject::Release()
{

}
