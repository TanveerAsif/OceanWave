cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
	float3 vCamPositon;
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
	float  fFogFactor : FOG;
};


cbuffer FogStartEnd
{
	float fFogStart;
	float fFogEnd;
	float2 fFogPadding;
};


float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

OutputType VS(InputType input)
{
	OutputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.color = input.color;
	output.tex = input.tex;

	//adding fog effect
	float4 vertexInCamPos = mul(input.position, world);
	vertexInCamPos = mul(vertexInCamPos, view);
	float fogStart = fFogStart, fogEnd = fFogEnd;
	float fFogEffectsRegion = 5.5f + rand_1_05(float2(input.position[0], input.position[1] * 2))*0.1;
	if(vCamPositon.y <  0.0f)
		output.fFogFactor = saturate((fogEnd - vertexInCamPos.z) / (fogEnd - fogStart));
	else
		output.fFogFactor = 1.0f;

	return output;
}


Texture2D skyTexture;
SamplerState textureSampler;

float4 PS(OutputType input) : SV_TARGET
{
	float4 pixelColor;

	//BlackPixel = float4(0.0f, 0.0f, 0.0f, 1.0f);	
	float2 revXY = float2(1.0 - input.tex.x, 1.0 - input.tex.y);
	float4 skyColor = skyTexture.Sample( textureSampler, revXY);
	//return skyColor;

	//pixelColor = skyColor;
	pixelColor = lerp(input.color, skyColor, 0.8);		
	//if(input.fFogFactor == 1.0f)
		pixelColor = skyColor;
	//else
		//pixelColor = input.color;
		
	/*if (pixelColor == BlackPixel)
	{
		pixelColor = input.color;
	}
	*/
	/*return old_color;*/
	//float4 fogColor = float4(201/255.0f, 220 / 255.0f, 241 / 255.0f, 1.0f); //Grey	
	float4 fogColor = float4(30 / 255.0f, 89 / 255.0f, 187 / 255.0f, 1.0f);
	float4 finalColour = input.fFogFactor * pixelColor + (1 - input.fFogFactor) * fogColor;
		
	//if(input.fFogFactor != 1.0f)
		//finalColour = skyColor;	
		
	return finalColour;
	
}

