

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;	
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;

};


cbuffer LightColorBuffer
{
	float4 ambientLight;
	float4 diffuseLight;	
};

cbuffer LightPosBuffer
{	
	float3 lightPos;
	float  padding;		// TO MAKE MULTIPLE OF 16
};



struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;	
};

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	
	float4 lpos : TEXCOORD1;            //vertex with regard to light view
	float3 lightDir : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowMap_VS(VertexInputType input)
{
	float4 worldPosition;
    PixelInputType output;
    
    input.position.w = 1.0f;

	
	output.pos = mul(input.position, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);
    
	//output.pos = mul(input.position, mul(world, mul(viewMatrix, projectionMatrix));

	output.lpos = mul(input.position, worldMatrix);
	output.lpos = mul(output.lpos, lightViewMatrix);
	output.lpos = mul(output.lpos, lightProjectionMatrix);
    
	//output.lpos = mul(input.position, mul(world, mul(lightViewMatrix, projectionMatrix));
	output.tex = input.tex;
	//output.normal = input.normal;
	output.normal = mul(input.normal, (float3x3)worldMatrix);	
	output.normal = normalize(output.normal);

	worldPosition = mul(input.position, worldMatrix);
	output.lightDir = lightPos.xyz - worldPosition.xyz;
	output.lightDir = normalize(output.lightDir);

    return output;
}


Texture2D shaderTexture : register(t0);
Texture2D depthTexture : register(t1);

SamplerState SampleWarpType : register(s0);
SamplerState SampleClampType : register(s1);




float4 ShadowMap_PS(PixelInputType input) : SV_TARGET
{
	float4 pixelColor, lightColor, textureColor, shadowColor;
	float depthValue, ligthDepthValue, lightIntensity;
	
	input.lpos.xyz /= input.lpos.w;	

	input.lpos.x = (input.lpos.x / 2) + 0.5;
	input.lpos.y = (input.lpos.y / 2) + 0.5;
		
	lightColor = ambientLight;	
	if ((saturate(input.lpos.x) == input.lpos.x) && (saturate(input.lpos.y) == input.lpos.y))                   //if x and y lies in 0-1 : else x <-1 and x > 1 will be in ambient light
	{		
		depthValue = (depthTexture.Sample(SampleClampType, input.lpos.xy).r);		
		ligthDepthValue = input.lpos.z;
		
		if (ligthDepthValue > depthValue)
		{
			//lightIntensity = dot(input.normal, input.lightDir);
			//if (lightIntensity > 0)
			{
				//Effective Light Color : Ambient + Diffuse				
				lightColor = diffuseLight; //*lightIntensity;				
				lightColor = saturate(lightColor);
			}
		}		
	}			
	
	textureColor = shaderTexture.Sample(SampleWarpType, input.tex);
	pixelColor = lightColor * textureColor;
	
	//shadowColor = depthTexture.Sample(SampleWarpType, input.lpos.xy);
	return pixelColor;
	
	
}