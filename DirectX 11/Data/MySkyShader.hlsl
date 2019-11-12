cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};


struct InputType
{
	float4 position : POSITION;	
	float4 color : COLOR;	
	float2 tex : TEXCOORD0;	
	
};

struct OutputType
{
	float4 position : SV_POSITION;	
	float4 color : COLOR;	
	float2 tex : TEXCOORD0;	
};


OutputType VS(InputType input)
{
	OutputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.color = input.color;
	output.tex = input.tex;
	return output;
}


Texture2D skyTexture;
SamplerState textureSampler;

float4 PS(OutputType input) : SV_TARGET
{
	float4 pixelColor;

	//BlackPixel = float4(0.0f, 0.0f, 0.0f, 1.0f);	
	pixelColor = skyTexture.Sample( textureSampler, input.tex);
	
	/*if (pixelColor == BlackPixel)
	{
		pixelColor = input.color;
	}
	*/
		
	return pixelColor;
	
}

