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