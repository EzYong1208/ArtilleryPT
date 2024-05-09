//	데이터들을 선언할때 상수 버퍼 입니다 라고 알려줘야함
cbuffer	RenderingPipeLine
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

//	부드럽게줘야하면 리니어샘플러, 주기 싫으면 포인트
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

/*ID3D11BlendState << 컴객체, 이 컴객체를 만들어주는 함
구조체에 값을 채우고 컴객체를 이용하고 
앉에 인자값으로 렌더 타겟 블렌드 데스크가 있고 
그 구조체 안에 다양한 인자들이 있는데 그걸 쉐이더에서 값을 채워주면 됨*/

//	알파 블렌딩용
BlendState	AlphaBlending
{
	//	안에 있는 값은 모두 다 배열을 사용할 수 있다.
	//	0번째, 배열의 인덱스 장치가 한번에 장치에 바인드 할 수 있는 렌더타겟이 최대 8개임
	//	0번째 렌더타겟의 BlendEnable을 트루로 하겠다 로 설정할 수 있다
	BlendEnable[0] = true;	//	알파 블렌딩을 한다
	BlendOp = Add;
	SrcBlend = Src_Alpha;	//	알파값을 곱해서 표현
	DestBlend = Inv_Src_Alpha;
};

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

//	D3D11_DEPTH_STENCIL_DESC 구조체의 값으로 설정되고 있다, 이 타입들을 참고로 만들어주면 된다(blenderstate 처럼)
/*
BOOL DepthEnable;	//	깊이 테스트를 킨다, 끈다
D3D11_DEPTH_WRITE_MASK DepthWriteMask;	//	WriteMask 멤버가 2개인 구조체(Zero, All) << zero를 주면 깊이를 기록하기 위한 영역(그릴거냐 아니냐)을 지정해라. 0 << 깊이 기록을 하지 않는다, all << 전체 영역에 기록해라(깊이 기록을 한다)
D3D11_COMPARISON_FUNC DepthFunc;	//	less greate equal 등 여러개가 있다, less 말고는 딱히 변경할게 없다
BOOL StencilEnable;		//	임의의 값을 기록해두고 그 스텐실 버퍼를 통한 비교를 통해 그릴 피셀을 그릴것인지 아닐것인지 설정(용도에 따라서 0에서 255사이의 값을 기록해두고 특정 영역을 그린다 안그린다, 깊이버퍼 좋지? 니가 사용해도 됨)
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/

//	기본적인 상태
DepthStencilState  ZTestAndWriteState
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;	//	작을때만 기록해라
};

//	깊이 테스트도 할 필요가 없다, 깊이 기록을 하지 않는다
DepthStencilState  NonZTestAndWriteState
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};

//	D3D11_RASTERIZER_DESC
/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;		//	앞면을 반시계 방향으로 설정 -> False 로 해야 시계방향으로 설정
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/
//	다른 옵션들은 딱히 안 사용한다

RasterizerState FillMode_Wireframe
{
	FillMode = wireframe;
	CullMode = back;		//	기본 컬링(뒤 부분을 컬링)
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
	CullMode = front;		//	앞면 컬링
	FrontCounterClockwise = false;

};

RasterizerState CullMode_ccw
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;

};
