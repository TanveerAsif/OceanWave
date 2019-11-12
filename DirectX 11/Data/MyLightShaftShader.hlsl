cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};


struct InputType
{
	float4 position : POSITION;		
	float4 col	: COLOR;
	float2 tex 		: TEXCOORD0;	
	float3 normal 	: NORMAL;
	
};

struct OutputType
{
	float4 position : SV_POSITION;		
	float4 col	: COLOR;
	float2 tex 		: TEXCOORD0;	
	float3 normal 	: NORMAL;
};


cbuffer RandomValue
{
	float randValue;
	float3 paddingValue;
}


Texture2D modelTexture;
SamplerState textureSampler;



OutputType VS(InputType input)
{
	OutputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position.y = modelTexture.SampleLevel(textureSampler, input.tex, 0).r * -100.0 ;
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	

	output.normal = mul(input.normal, (float3x3) world);
	output.normal = normalize(output.normal);

	////float4 startPos = float4(output.position.x, output.position.y, output.position.z, output.position.w);
	////float3 dir = float3(1.0, 0.0, 0.0);
	////float angle = dot(output.normal, dir);		//Cos90
	/////*if (angle == 0)
	////{
	////	float2 noise = (frac(sin(dot(input.tex, float2(12.9898, 78.233)*2.0)) * 43758.5453));

	////	float testVal = randValue;
	////	if (testVal > 25 || testVal < 0)
	////		testVal = 10.5;
	////	output.position.y -= 125 + abs(noise.x + noise.y) * testVal;
	////}*/

	////float4 targetPos = float4(output.position.x, output.position.y, output.position.z, output.position.w);
	////targetPos = lerp(targetPos, startPos, 0.1);

	////output.position = targetPos;

	output.tex = input.tex;
	output.col = input.col;

	return output;
}





float4 PS(OutputType input) : SV_TARGET
{
	float4 colorTexture, FinalColor;	
	colorTexture = modelTexture.Sample(textureSampler, input.tex);
	

	FinalColor = float4(colorTexture.r, colorTexture.g, colorTexture.b*0.1, colorTexture.r * 0.075);

	//colorTexture.a = 0.075;



	float3 dir = float3(1.0, 0.0, 0.0);
	float angle = dot(input.normal, dir);	
	//if (angle == 0)
	//	colorTexture.a = 0.15;//Transparent
	//else
	//	colorTexture.a = 0;
	
	
	//FinalColor = colorTexture * input.col;
	//FinalColor.a = saturate(FinalColor.a *0.075);//Transparent	
	
	//FinalColor = float4(1, 1, 1, 0.075);
	//FinalColor = float4(1, 1, 1, 0.075);
	
	return FinalColor;
}

