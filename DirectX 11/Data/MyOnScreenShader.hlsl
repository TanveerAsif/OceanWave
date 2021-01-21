
cbuffer MatrixBuffer :register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 vCamPositon;
};


struct InputType
{
	float3 position : POSITION;	
	float2 tex : TEXCOORD0;	
	
};

struct OutputType
{
	float4 position : SV_POSITION;	
	float2 tex : TEXCOORD0;	

};


cbuffer cTimer : register(b1)
{	
	float fTime;
	float fDelta;
	float2 padding;
};


OutputType VS(InputType input)
{
	OutputType output;

	
	//output.position = mul(float4(input.position, 1), world);
	//output.position = mul(output.position, view);
	//output.position = mul(output.position, projection);	
	
	output.position = float4(1 - 2 * input.tex.x , 1 - 2 * input.tex.y , 0.0f, 1.0f);
	output.tex = input.tex;

	return output;
}


Texture2D screenTexture : register(t0);
Texture2D waterDropletTexture : register(t1);
Texture2D displacementTexureMap : register(t2);
Texture2D waterToAirTexure : register(t3);
SamplerState textureSampler : register(s0);;


float4 PS(OutputType input) : SV_TARGET
{
	
	float4 screenPixel, screenDispPixel, waterDropletPixel, dispPixel, finalPixel;
	float2 txCoord = input.tex;	
	//txCoord.y = input.tex.y + fDelta;
	input.tex.y -= fTime*0.5;		
	waterDropletPixel = waterDropletTexture.Sample(textureSampler, input.tex);	
	/*
	float grayScaleValue = (waterDropletPixel.r + waterDropletPixel.g + waterDropletPixel.b)/3.0f;
	float4 grayScalePixel = float4(grayScaleValue, grayScaleValue, grayScaleValue, waterDropletPixel.a);
	grayScaleValue = saturate(grayScaleValue);
	finalPixel = lerp(screenPixel, grayScalePixel, grayScaleValue);
	*/
	
	//txCoord.x += waterDropletPixel.x;
	txCoord.y -= waterDropletPixel*0.25;
	txCoord.y = saturate(txCoord.y) + 0.004;
	screenDispPixel = screenTexture.Sample(textureSampler, txCoord);
	dispPixel = displacementTexureMap.Sample(textureSampler, txCoord);
	finalPixel = lerp(screenDispPixel, dispPixel, 0.1);
	

	return finalPixel;
}


float4 PS_WaterToAir(OutputType input) : SV_TARGET
{

	float4 finalPixel, water2AirPixel, screenDispPixel;
	float4 VapourColor = float4(0.4951, 0.4952, 0.4953, 0.28);
	float2 txCoord = input.tex;	
	txCoord.y -= fTime*0.65;
	
	water2AirPixel = waterToAirTexure.Sample(textureSampler, 2 * txCoord);

	screenDispPixel = screenTexture.Sample(textureSampler, input.tex); 
	finalPixel = lerp(screenDispPixel, water2AirPixel, 0.2);
	
	return finalPixel;

}
