#pragma once

//	V : Vertex, I : Index, Buffer : 메모리 공간
//	다양한 모델 형태의 클래스들의 부모가 된다

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;

public:
	virtual HRESULT				Render(class CShader* pShader, _uint iPassIndex);

protected:
	ID3D11Buffer*				m_pVB = nullptr;		//	정점 버퍼
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubResourceData;
	_uint						m_iNumVertices = 0;		//	정점의 갯수
	_uint						m_iNumVertexBuffers = 0;	//	버텍스 버퍼 몇개 세팅할건지(일반적인 렉트 1개)
	_float3*					m_pVerticesPos = nullptr;	//	정점의 위치

protected:
	ID3D11Buffer*				m_pIB = nullptr;		//	인덱스 버퍼
	D3D11_BUFFER_DESC			m_IBDesc;				//	밑에 주석 구조체 참고
	D3D11_SUBRESOURCE_DATA		m_IBSubResourceData;	//	구조체 안에 포인터 변수가 들어가있음(*pSysMem) 니가 지금 할당하고자하는 버텍스 배열에 채우고자하는 값을 넣어서 그 공간에 있는 값을 고대로 복사해서 버텍스 버퍼에 할당해줄게
	_uint						m_iNumPrimitive = 0;	//	그릴 폴리곤 수
	_uint						m_iNumIndicesPerPrimitive = 0;	//	도형 하나당 몇개의 인덱스를 사용하는지 알려줘야할거같으니 변수로 만듬 
	DXGI_FORMAT					m_eIndexFormat;			//	인덱스 포맷
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	//	자식에서 값을 넣으면 부모에서 그 값을 토대로 만들겠다
protected:
	HRESULT						Create_VertexBuffer();
	HRESULT						Create_IndexBuffer();

public:
	virtual CComponent*			Clone(void* pArg) = 0;
	virtual void				Free() override;
};

END

//	이 클래스가 왜 필요한 것인가
//	이미지가 올라가기 위한 바닥(모델) <- 이 모델을 그릴거
//	이미지 사이즈는 크게 중요하지 않고 이 이미지를 올릴 모델의 크기에 맵핑할거
//	모델구성) 정점(버텍스)을 찍고, 점에다 위치를 세팅, 어떤 형태로 배치하느냐에 따라 크기 형태 위치가 결정됨


//typedef struct D3D11_BUFFER_DESC
//{
//	UINT ByteWidth;				//	몇 바이트를 할당할 건지
//	D3D11_USAGE Usage;			//	버텍스버퍼의 용도(동적, 정적 어떤걸로 만들건지)
//	UINT BindFlags;				//	메모리 덩어리가 어떤 용도로 장치에 바인드 할것인지(인덱스 버퍼냐 버텍스 버퍼냐 정하게 됨)
//	UINT CPUAccessFlags;
//	UINT MiscFlags;				//	CPUAccessFlag 나 MiscFlag 는 Usage에 따라 값이 달라짐 << 자식에서 값을 채워주는 작업을 함
//	UINT StructureByteStride;
//} 	D3D11_BUFFER_DESC;


//typedef
//enum D3D11_USAGE
//{
//	D3D11_USAGE_DEFAULT = 0,		
//	D3D11_USAGE_IMMUTABLE = 1,	//	(cpu 접근도 불가, gpu 접근도 불가, 한번 할당한 값이 절대 바뀌지 않는 경우)  << 렌더링 속도는 가장 최적의 속도로 가져올 수 있음, 정적
//	D3D11_USAGE_DYNAMIC = 2,	//	동적버퍼
//	D3D11_USAGE_STAGING = 3		//	쉐이더에서 직접 버퍼를 만들어서 클라이언트에 저장할수 있게끔 가능해지긴 하는데 지오메트리 쉐이더가 있어서 별로 안씀
//} 	D3D11_USAGE;
//	CPU 쓰기 가능한건 Lock, UnLock 이 가능한거