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
	//	�ϳ� �н����� ��������� �ٸ� �н������� �� ����ؾ��Ѵ�
	//	���� pass Rect SetBlendState() �� ������ �ٸ� pass ������ �� �ȿ��� SetBlendState() �� ����ؾ���
	pass Rect
	{
		//	���� ������Ʈ
		//	�������� ��� �����ϰ� �� ȣ���ؼ� ���
		//	���̴� ���� Ŭ���̾�Ʈ���� ��� ����ص� �Ǳ�� �ϴµ� ��Ȳ�� �� ���ŷο�, ���̴����� �ϴ°� �� ���ϴٰ� �Ͻ�

		//	���� ���� ������� SetBlendState(���� �������, BlendFactor(������ �׳� 0000),
		//	Mask �� rgba�� ���� ���ۼ�Ʈ�� ���� �Ұ��̳�, rgb�� Ư�� ������ ���带 �ϰ� �ʹ� �ϸ� mask�� ���ؼ�...�Ϲ������δ� �� �Ⱦ��ϱ� fffffff�� �� �о��������)
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
		//	���� ������Ʈ
		//	�������� ��� �����ϰ� �� ȣ���ؼ� ���
		//	���̴� ���� Ŭ���̾�Ʈ���� ��� ����ص� �Ǳ�� �ϴµ� ��Ȳ�� �� ���ŷο�, ���̴����� �ϴ°� �� ���ϴٰ� �Ͻ�

		//	���� ���� ������� SetBlendState(���� �������, BlendFactor(������ �׳� 0000),
		//	Mask �� rgba�� ���� ���ۼ�Ʈ�� ���� �Ұ��̳�, rgb�� Ư�� ������ ���带 �ϰ� �ʹ� �ϸ� mask�� ���ؼ�...�Ϲ������δ� �� �Ⱦ��ϱ� fffffff�� �� �о��������)
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