cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
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
	return output;
}

cbuffer lightBuffer
{
	float4 ambientLight;
	float4 diffuseLight;
	float3 lightDir;
	float  padding;		// TO MAKE MULTIPLE OF 16
};

Texture2D modelTexture;
SamplerState textureSampler;

float4 PS(OutputType input) : SV_TARGET
{
	float3 dir;
	float4 textureColor, PixelColor, lightColor;
	float  lightIntensity;
	

	textureColor = modelTexture.Sample(textureSampler, input.tex);
	
	dir = lightDir;
	lightIntensity = saturate(dot(input.normal, dir));
	
	lightColor = ambientLight;
	if (lightIntensity > 0)
	{
		//Effective Light Color : Ambient + Diffuse				
		lightColor *= diffuseLight *lightIntensity;
	}
	lightColor = saturate(lightColor);	

	//PixelColor = textureColor*lightIntensity*diffuseLight;
	PixelColor = lightColor*textureColor;
	
	return PixelColor;
}

