#pragma once

namespace Engine
{
	//	For.Anim
	typedef struct tagKeyFrame
	{
		_float3		vScale;			//	�� Ű �����Ӷ� �� ���� ǥ���ؾ��ϴ� Scale
		_float4		vRotation;		//	�� Ű �����Ӷ� �� ���� ǥ���ؾ��ϴ� Rotation(���ʹϾ�?)
		_float3		vPosition;		//	�� Ű �����Ӷ� �� ���� ǥ���ؾ��ϴ� Position
		_double		Time;			//	�� Ű �������� �����ؾ��ϴ� �ð� Time
	}KEYFRAME;

	//	For.MeshMaterial
	typedef struct tagMeshMaterialDesc
	{
		//	assimp�� �ҷ��ü� �ִ� �ؽ����� �ִ밹�� : 18��(AI_TEXTURE_TYPE_MAX]
		//	�� ����ŭ �׳� �迭�� ǥ���ϰ� ����
		class CTexture*	pTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIALDESC;

	//	For.Light
	//	���� ���� ����ü
	typedef struct tagLightDesc
	{
		//	POINT : ������, ���Ⱚ
		//	DIRECTIONAL : ���ǿ����� ������ �� ���� ����
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eLightType = TYPE_END;
		XMFLOAT4	vDiffuse;		//	������ �⺻���� ����
		XMFLOAT4	vAmbient;		//	ȯ�� ������ ���� ���� ����. ���� ���ϱ�, �ִ��� �Ǽ���� ����ϰ� ������ֱ� ���� �߰����� ������
		XMFLOAT4	vSpecular;		//	���ݻ� ������ ���� ����. ���̶���Ʈ ������ ����. �� �ȼ��� ���࿡ �� �ü��� ���Դٸ�(����ŧ���� ����)�� 1�� �ΰ�, �ƴϸ� 0���� ��
		XMFLOAT4	vDirection;		//	���⺤��
		XMFLOAT4	vPosition;
	}LIGHTDESC;

	//	For.MeshModel
	//	�޽��� �ε��ϱ����� �����ν� �ʿ��� ����ü
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
		XMUINT4				vBlendIndex;		//	���� �� ������ � ���鿡�� ������ �޴���
		XMFLOAT4			vBlendWeight;		//	������ ���ۼ�Ʈ�� ������ �ް� �ִ���
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
	���̽�, ������ -> �� �̶�� ����
	dx�� �ﰢ��
	�ﰢ�� 2���� �ٿ��� �簢���� ����ǵ� �׷��� ���� 6���� �ʿ�
	������ Ŀ���͸���¡ �Ѵ�
	������ �迭�� ���� -> �ټ��� ������ ����, �ʿ��� ������ ä���
	*/

	/*	
	�����ȿ� �߰��� �� �ִ� ���(������), �������� ���� ���� �����͸� �߰��ϴ� ���� ���� ����
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

	//	����Ǵ� ������ ����ü ����
	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;		//	��ġ
		XMFLOAT2			vTexUV;			//	�ؽ��ĸ� �����ϱ� ���� ����
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
		unsigned long	_0, _1, _2;			//	�ﰢ���� �׸��Ŷ� �ε��� 3��
	}FACEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;			//	�ﰢ���� �׸��Ŷ� �ε��� 3��
	}FACEINDICES16;
}