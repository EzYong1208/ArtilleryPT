#include "TestObject.h"
#include "Renderer.h"

TestObject::TestObject()
{
	_RenderObject->SetCoord_Size(0.f, 0.f, 300.f, 300.f);
	_RenderObject->SetTexture("Prototype_Component_Texture_Default");
	_RenderObject->SetRenderType(CRenderer::RENDER_PRIORITY);
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
