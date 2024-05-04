#pragma once

//	V : Vertex, I : Index, Buffer : �޸� ����
//	�پ��� �� ������ Ŭ�������� �θ� �ȴ�

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
	ID3D11Buffer*				m_pVB = nullptr;		//	���� ����
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubResourceData;
	_uint						m_iNumVertices = 0;		//	������ ����
	_uint						m_iNumVertexBuffers = 0;	//	���ؽ� ���� � �����Ұ���(�Ϲ����� ��Ʈ 1��)
	_float3*					m_pVerticesPos = nullptr;	//	������ ��ġ

protected:
	ID3D11Buffer*				m_pIB = nullptr;		//	�ε��� ����
	D3D11_BUFFER_DESC			m_IBDesc;				//	�ؿ� �ּ� ����ü ����
	D3D11_SUBRESOURCE_DATA		m_IBSubResourceData;	//	����ü �ȿ� ������ ������ ������(*pSysMem) �ϰ� ���� �Ҵ��ϰ����ϴ� ���ؽ� �迭�� ä������ϴ� ���� �־ �� ������ �ִ� ���� ���� �����ؼ� ���ؽ� ���ۿ� �Ҵ����ٰ�
	_uint						m_iNumPrimitive = 0;	//	�׸� ������ ��
	_uint						m_iNumIndicesPerPrimitive = 0;	//	���� �ϳ��� ��� �ε����� ����ϴ��� �˷�����ҰŰ����� ������ ���� 
	DXGI_FORMAT					m_eIndexFormat;			//	�ε��� ����
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	//	�ڽĿ��� ���� ������ �θ𿡼� �� ���� ���� ����ڴ�
protected:
	HRESULT						Create_VertexBuffer();
	HRESULT						Create_IndexBuffer();

public:
	virtual CComponent*			Clone(void* pArg) = 0;
	virtual void				Free() override;
};

END

//	�� Ŭ������ �� �ʿ��� ���ΰ�
//	�̹����� �ö󰡱� ���� �ٴ�(��) <- �� ���� �׸���
//	�̹��� ������� ũ�� �߿����� �ʰ� �� �̹����� �ø� ���� ũ�⿡ �����Ұ�
//	�𵨱���) ����(���ؽ�)�� ���, ������ ��ġ�� ����, � ���·� ��ġ�ϴ��Ŀ� ���� ũ�� ���� ��ġ�� ������


//typedef struct D3D11_BUFFER_DESC
//{
//	UINT ByteWidth;				//	�� ����Ʈ�� �Ҵ��� ����
//	D3D11_USAGE Usage;			//	���ؽ������� �뵵(����, ���� ��ɷ� �������)
//	UINT BindFlags;				//	�޸� ����� � �뵵�� ��ġ�� ���ε� �Ұ�����(�ε��� ���۳� ���ؽ� ���۳� ���ϰ� ��)
//	UINT CPUAccessFlags;
//	UINT MiscFlags;				//	CPUAccessFlag �� MiscFlag �� Usage�� ���� ���� �޶��� << �ڽĿ��� ���� ä���ִ� �۾��� ��
//	UINT StructureByteStride;
//} 	D3D11_BUFFER_DESC;


//typedef
//enum D3D11_USAGE
//{
//	D3D11_USAGE_DEFAULT = 0,		
//	D3D11_USAGE_IMMUTABLE = 1,	//	(cpu ���ٵ� �Ұ�, gpu ���ٵ� �Ұ�, �ѹ� �Ҵ��� ���� ���� �ٲ��� �ʴ� ���)  << ������ �ӵ��� ���� ������ �ӵ��� ������ �� ����, ����
//	D3D11_USAGE_DYNAMIC = 2,	//	��������
//	D3D11_USAGE_STAGING = 3		//	���̴����� ���� ���۸� ���� Ŭ���̾�Ʈ�� �����Ҽ� �ְԲ� ���������� �ϴµ� ������Ʈ�� ���̴��� �־ ���� �Ⱦ�
//} 	D3D11_USAGE;
//	CPU ���� �����Ѱ� Lock, UnLock �� �����Ѱ�