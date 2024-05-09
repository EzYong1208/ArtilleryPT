#include "Shader_Defines.hpp"

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UI(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//if (Out.vColor.rgb = 0.f)
	//	discard;

	return Out;
}

//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;	

technique11		DefaultTechnique
{
	//	하나 패스에서 사용했으면 다른 패스에서도 다 사용해야한다
	//	대충 pass Rect SetBlendState() 로 했으면 다른 pass 에서도 다 안에서 SetBlendState() 로 사용해야함
	pass Rect
	{
		//	렌더 스테이트
		//	세가지를 모두 세팅하고 다 호출해서 사용
		//	쉐이더 말고 클라이언트에서 잡고 사용해도 되기는 하는데 상황이 좀 번거로움, 쉐이더에서 하는게 좀 편하다고 하심

		//	알파 블렌딩 방식으로 SetBlendState(알파 블렌딩방식, BlendFactor(지금은 그냥 0000),
		//	Mask 값 rgba에 각각 몇퍼센트나 블렌드 할것이냐, rgb의 특정 부위만 블레드를 하고 싶다 하면 mask를 통해서...일반적으로는 잘 안쓰니깐 fffffff로 싹 밀어버림ㅇㅇ)
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		//SetRasterizerState();
		//SetDepthStencilState();

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass BlackUI
	{
		//	렌더 스테이트
		//	세가지를 모두 세팅하고 다 호출해서 사용
		//	쉐이더 말고 클라이언트에서 잡고 사용해도 되기는 하는데 상황이 좀 번거로움, 쉐이더에서 하는게 좀 편하다고 하심

		//	알파 블렌딩 방식으로 SetBlendState(알파 블렌딩방식, BlendFactor(지금은 그냥 0000),
		//	Mask 값 rgba에 각각 몇퍼센트나 블렌드 할것이냐, rgb의 특정 부위만 블레드를 하고 싶다 하면 mask를 통해서...일반적으로는 잘 안쓰니깐 fffffff로 싹 밀어버림ㅇㅇ)
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		//SetRasterizerState();
		//SetDepthStencilState();

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UI();
	}
}