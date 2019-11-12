////////////////////////////////////////////////////////////////////////////////
// Filename: reflection.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
};


Texture2D floorTexture;
SamplerState SampleType;
Texture2D shadowMapTexture;


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS(VertexInputType input)
{
    PixelInputType output;
	matrix reflectProjectWorld;
	

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

    // Create the reflection projection world matrix.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// Calculate the input position against the reflectProjectWorld matrix.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

	
    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PixelInputType input) : SV_TARGET
{
	float4 floorPixelColor;
    float2 reflectTexCoord;
    float4 reflectionPixelColor;
    float4 finalPixelColor;


	floorPixelColor = floorTexture.Sample(SampleType, input.tex);
    
    // Calculate the projected reflection texture coordinates.	
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
	
	if(reflectTexCoord.x > 1)
		reflectTexCoord.x = 1.0f;
	else if(reflectTexCoord.x < 0)
		reflectTexCoord.x = 0.0f;
		
	if(reflectTexCoord.y > 1)
		reflectTexCoord.y = 1.0f;
	else if(reflectTexCoord.y < 0)
		reflectTexCoord.y = 0.0f;	
		
	
	reflectionPixelColor = shadowMapTexture.Sample(SampleType, reflectTexCoord);

	
	// Do a linear interpolation between the two textures for a blend effect.
	finalPixelColor = lerp(floorPixelColor, reflectionPixelColor, 0.2f);


	return finalPixelColor;
}