#pragma once

namespace Engine
{
	//	For.Anim
	typedef struct tagKeyFrame
	{
		_float3		vScale;			//	이 키 프레임때 이 뼈가 표현해야하는 Scale
		_float4		vRotation;		//	이 키 프레임때 이 뼈가 표현해야하는 Rotation(쿼터니언?)
		_float3		vPosition;		//	이 키 프레임때 이 뼈가 표현해야하는 Position
		_double		Time;			//	이 키 프레임이 등장해야하는 시간 Time
	}KEYFRAME;

	//	For.MeshMaterial
	typedef struct tagMeshMaterialDesc
	{
		//	assimp로 불러올수 있는 텍스쳐의 최대갯수 : 18개(AI_TEXTURE_TYPE_MAX]
		//	이 값만큼 그냥 배열로 표현하고 있음
		class CTexture*	pTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIALDESC;

	//	For.Light
	//	빛을 위한 구조체
	typedef struct tagLightDesc
	{
		//	POINT : 정광원, 감쇄값
		//	DIRECTIONAL : 현실에서는 존재할 수 없는 개념
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eLightType = TYPE_END;
		XMFLOAT4	vDiffuse;		//	광원의 기본적인 색상
		XMFLOAT4	vAmbient;		//	환경 광원에 의한 색상 정보. 최종 곱하기, 최대한 실세계와 비슷하게 만들어주기 위한 추가적인 데이터
		XMFLOAT4	vSpecular;		//	정반사 광원에 의핸 색상. 하이라이트 색상을 지정. 이 픽셀이 만약에 내 시선에 들어왔다면(스펙큘러의 세기)를 1로 두고, 아니면 0으로 둠
		XMFLOAT4	vDirection;		//	방향벡터
		XMFLOAT4	vPosition;
	}LIGHTDESC;

	//	For.MeshModel
	//	메쉬를 로드하기위한 정보로써 필요한 구조체
	typedef struct tagVertex_Model
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertex_Model_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	//	For.AnimModel
	typedef struct tagVertex_Animation_Model
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;		//	실제 이 정보가 어떤 뼈들에게 영향을 받는지
		XMFLOAT4			vBlendWeight;		//	뼈에게 몇퍼센트의 영향을 받고 있느냐
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertex_AnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;

	//	For.PointInstance
	typedef struct tagVertex_Point
	{
		XMFLOAT3			vPosition;
		float				fPSize;
	}VTXPOINT;

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

	//	For.CubeTexture
	typedef struct tagVertex_CubeTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUV;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Cube_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;

	//	For.NormalTexture
	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Texture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX_DECLARATION;




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