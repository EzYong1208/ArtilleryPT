//	�����͵��� �����Ҷ� ��� ���� �Դϴ� ��� �˷������
cbuffer	RenderingPipeLine
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

//	�ε巴������ϸ� ���Ͼ���÷�, �ֱ� ������ ����Ʈ
sampler PointSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler DefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

/*ID3D11BlendState << �İ�ü, �� �İ�ü�� ������ִ� ��
����ü�� ���� ä��� �İ�ü�� �̿��ϰ� 
�ɿ� ���ڰ����� ���� Ÿ�� ���� ����ũ�� �ְ� 
�� ����ü �ȿ� �پ��� ���ڵ��� �ִµ� �װ� ���̴����� ���� ä���ָ� ��*/

//	���� ������
BlendState	AlphaBlending
{
	//	�ȿ� �ִ� ���� ��� �� �迭�� ����� �� �ִ�.
	//	0��°, �迭�� �ε��� ��ġ�� �ѹ��� ��ġ�� ���ε� �� �� �ִ� ����Ÿ���� �ִ� 8����
	//	0��° ����Ÿ���� BlendEnable�� Ʈ��� �ϰڴ� �� ������ �� �ִ�
	BlendEnable[0] = true;	//	���� ������ �Ѵ�
	BlendOp = Add;
	SrcBlend = Src_Alpha;	//	���İ��� ���ؼ� ǥ��
	DestBlend = Inv_Src_Alpha;
};

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

//	D3D11_DEPTH_STENCIL_DESC ����ü�� ������ �����ǰ� �ִ�, �� Ÿ�Ե��� ����� ������ָ� �ȴ�(blenderstate ó��)
/*
BOOL DepthEnable;	//	���� �׽�Ʈ�� Ų��, ����
D3D11_DEPTH_WRITE_MASK DepthWriteMask;	//	WriteMask ����� 2���� ����ü(Zero, All) << zero�� �ָ� ���̸� ����ϱ� ���� ����(�׸��ų� �ƴϳ�)�� �����ض�. 0 << ���� ����� ���� �ʴ´�, all << ��ü ������ ����ض�(���� ����� �Ѵ�)
D3D11_COMPARISON_FUNC DepthFunc;	//	less greate equal �� �������� �ִ�, less ����� ���� �����Ұ� ����
BOOL StencilEnable;		//	������ ���� ����صΰ� �� ���ٽ� ���۸� ���� �񱳸� ���� �׸� �Ǽ��� �׸������� �ƴҰ����� ����(�뵵�� ���� 0���� 255������ ���� ����صΰ� Ư�� ������ �׸��� �ȱ׸���, ���̹��� ����? �ϰ� ����ص� ��)
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/

//	�⺻���� ����
DepthStencilState  ZTestAndWriteState
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;	//	�������� ����ض�
};

//	���� �׽�Ʈ�� �� �ʿ䰡 ����, ���� ����� ���� �ʴ´�
DepthStencilState  NonZTestAndWriteState
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};

//	D3D11_RASTERIZER_DESC
/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;		//	�ո��� �ݽð� �������� ���� -> False �� �ؾ� �ð�������� ����
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/
//	�ٸ� �ɼǵ��� ���� �� ����Ѵ�

RasterizerState FillMode_Wireframe
{
	FillMode = wireframe;
	CullMode = back;		//	�⺻ �ø�(�� �κ��� �ø�)
	FrontCounterClockwise = false;
};
RasterizerState FillMode_Solid
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState CullMode_cw
{
	FillMode = Solid;
	CullMode = front;		//	�ո� �ø�
	FrontCounterClockwise = false;

};

RasterizerState CullMode_ccw
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;

};
