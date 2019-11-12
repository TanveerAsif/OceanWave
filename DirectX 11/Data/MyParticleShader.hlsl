cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};


struct VertexInputType
{
	float4 position : POSITION;	
	float2 tex : TEXCOORD0;
	float4 color : COLOR;	
	
	
};

struct PixelInputType
{
	float4 position : SV_POSITION;		
	float2 tex : TEXCOORD0;	
	float4 color : COLOR;
};


PixelInputType VS(VertexInputType input)
{
	PixelInputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.color = input.color;
	output.tex = input.tex;
	return output;
}


Texture2D particleTexture;
SamplerState textureSampler;

float4 PS(PixelInputType input) : SV_TARGET
{
	float4 finalColor;
	float4 textureColor;
	//finalColor = float4(1.0f, 1.0f, 0.0f, 1.0f);//WHITE	
	
	
	textureColor = particleTexture.Sample(textureSampler, input.tex);
	
	//finalColor.rgb = textureColor.rgb * input.color.rgb;
	finalColor = textureColor * input.color;
	
		
	return finalColor;	
}