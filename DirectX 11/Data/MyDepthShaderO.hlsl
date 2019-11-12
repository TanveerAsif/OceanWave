/*
	output.depthPosition = mul(output.depthPosition, world);
	output.depthPosition = mul(output.depthPosition, view);
	*/

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};


struct InputType
{
	float4 position : POSITION;	
};

struct OutputType
{
	float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};


OutputType VS(InputType input)
{
	OutputType output;
	input.position.w =1;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	
	output.depthPosition = input.position;
	

	return output;
}


float4 PS(OutputType input) : SV_TARGET
{
	float depthValue = input.depthPosition.z / input.depthPosition.w;
	
	float4 color;
	if(depthValue < 0.1)
		color = float4(1.0f, 0.0f, 0.0f, 1.0f);//RED
	else if(depthValue < 0.9)
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);//GREEN
	else if(depthValue <= 1.0)
		color = float4(1.0f, 1.0f, 0.0f, 1.0f);//YELLOW
	else
		color = float4(1.0f, 1.0f, 1.0f, 1.0f);//WHITE
		

	return color;
}

