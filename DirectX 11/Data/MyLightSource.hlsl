

cbuffer MatrixBuffer: register(b0)
{
	matrix world;
	matrix view;	
	matrix projection;
	float3 vCamPositon;
};

cbuffer FogParameter : register(b1)
{
	float fFogStart;
	float fFogEnd;
	float2 fFogPadding;
};

cbuffer LightBuffer : register(b2)
{
	float4 ambientLight;
	float4 diffuseLight;
	float3 lightDir;
	float  padding;		// TO MAKE MULTIPLE OF 16
};


struct InputType
{
	float4 position : POSITION;		
	float2 tex 		: TEXCOORD0;	
	float3 normal 	: NORMAL;
	
};

struct OutputType
{
	float4 position : SV_POSITION;		
	float2 tex 		: TEXCOORD0;	
	float3 normal 	: NORMAL;
	float  fFogFactor : FOG;
	float  fCamYPos : TEXCOORD1;
};


OutputType VS(InputType input)
{
	OutputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
		
	output.tex = input.tex;
	
	output.normal = mul(input.normal, (float3x3) world);
	output.normal = normalize(output.normal);


	//adding fog effect
	float4 vertexInCamPos = mul(input.position, world);
	vertexInCamPos = mul(vertexInCamPos, view);
	float fogStart = fFogStart, fogEnd = fFogEnd;	

	if (vCamPositon.y < 0.0f)
	{
		fogStart = 1.0f;
		fogEnd = 10.0f;
		output.fFogFactor = saturate((fogEnd - vertexInCamPos.z) / (fogEnd - fogStart));
	}
	else
		output.fFogFactor = 1.0f;

	output.fCamYPos = vCamPositon.y;

	return output;
}


Texture2D modelTexture : register(t0);;
SamplerState textureSampler : register(s0);;

float4 PS(OutputType input) : SV_TARGET
{
	float3 dir;
	float4 textureColor, PixelColor, lightColor;
	float  lightIntensity;
	float4 fogColor = float4(30 / 255.0f, 89 / 255.0f, 187 / 255.0f, 1.0f);

	textureColor = modelTexture.Sample(textureSampler, input.tex);
	
	dir = lightDir;
	lightIntensity = saturate(dot(input.normal, dir));
	
	lightColor = ambientLight;
	if (input.fCamYPos > 0)
	{
		if (lightIntensity > 0)
		{
			//Effective Light Color : Ambient + Diffuse				
			lightColor *= diffuseLight *lightIntensity;
		}
		
	}
	else
	{
		lightColor *= fogColor;
	}
	lightColor = saturate(lightColor);	

	//PixelColor = textureColor*lightIntensity*diffuseLight;
	PixelColor = lightColor*textureColor;
	
	//float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f); //Grey	
	//float4 fogColor = float4(201 / 255.0f, 220 / 255.0f, 241 / 255.0f, 1.0f); //Grey	
	
	float4 finalColor = input.fFogFactor * PixelColor + (1.0f - input.fFogFactor) * fogColor;
	
	return finalColor;
}

