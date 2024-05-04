#pragma once

//	���� �ߴ°� �� ���������°�
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <typeinfo>

#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <DirectXCollision.h>

//	�ݶ��̴�
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

//	��Ʈ
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>			//	��ũ���� ��°Ű��� ���
#include <wincodec.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace DirectX;
using namespace Assimp;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <process.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <io.h>
#include <time.h>
using namespace std;

//	�޸� ������ üũ�Ϸ��� #define USE_IMGUI ������ �ּ�ó���ϱ�
#ifdef _DEBUG
#define USE_IMGUI
#endif

//	USE_IMGUI �� define �Ǿ����� ���� ������ �޸� ���� üũ
#if !defined(USE_IMGUI)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif // _DEBUG
#endif // DBG_NEW
#endif // !defined(USE_IMGUI)

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

using namespace Engine;
