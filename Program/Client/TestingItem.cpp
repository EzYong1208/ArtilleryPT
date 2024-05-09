#include "TestingItem.h"
#include "Renderer.h"

TestingItem::TestingItem()
{
	_RenderObject->SetCoord_Size(130.f, 130.f, 240.f, 135.f);
	_RenderObject->SetTexture("Prototype_Component_Texture_Background");
	_RenderObject->SetRenderType(CRenderer::RENDER_PRIORITY);
}

TestingItem::~TestingItem()
{
	_RenderObject->Free();
}

void TestingItem::NativeConstruct()
{
}

void TestingItem::Tick(_double deltaTime)
{
}

void TestingItem::LateTick(AGameObject* pRenderObject)
{
	__super::LateTick(pRenderObject);
}
