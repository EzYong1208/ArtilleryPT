#pragma once

namespace Engine
{
	typedef struct ENGINE_DLL tagVertex_Point_Matrix_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINTMATRIX_DECLARATION;

	//	For.RectInstance
	typedef struct tagVertex_Matrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertex_Matrix_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMATRIX_DECLARATION;

	//	For.Navigation
	typedef struct tagVertex_Color
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertex_Color_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCOL_DECLARATION;

	/*
	페이스, 폴리곤 -> 면 이라는 개념
	dx는 삼각형
	삼각형 2개를 붙여서 사각형을 만들건데 그러면 점이 6개가 필요
	정점을 커스터마이징 한다
	정점은 배열로 선언 -> 다수의 정점을 선언, 필요한 정보만 채우고
	*/

	/*	
	정점안에 추가될 수 있는 멤버(데이터), 정점에게 쓸데 없는 데이터를 추가하는 것은 좋지 않음
	D3DDECLUSAGE_POSITION = 0,
	D3DDECLUSAGE_BLENDWEIGHT,   // 1
	D3DDECLUSAGE_BLENDINDICES,  // 2
	D3DDECLUSAGE_NORMAL,        // 3
	D3DDECLUSAGE_PSIZE,         // 4
	D3DDECLUSAGE_TEXCOORD,      // 5
	D3DDECLUSAGE_TANGENT,       // 6
	D3DDECLUSAGE_BINORMAL,      // 7
	D3DDECLUSAGE_TESSFACTOR,    // 8
	D3DDECLUSAGE_POSITIONT,     // 9
	D3DDECLUSAGE_COLOR,         // 10
	D3DDECLUSAGE_FOG,           // 11
	D3DDECLUSAGE_DEPTH,         // 12
	D3DDECLUSAGE_SAMPLE,        // 13
	*/

	//	선언되는 순서는 열거체 순서
	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;		//	위치
		XMFLOAT2			vTexUV;			//	텍스쳐를 맵핑하기 위한 정보
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertex_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXTEX_DECLARATION;

	typedef struct tagLineIndices32
	{
		unsigned long	_0, _1;
	}LINEINDICES32;

	typedef struct tagLineIndices16
	{
		unsigned short	_0, _1;
	}LINEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long	_0, _1, _2;			//	삼각형을 그릴거라 인덱스 3개
	}FACEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;			//	삼각형을 그릴거라 인덱스 3개
	}FACEINDICES16;
}