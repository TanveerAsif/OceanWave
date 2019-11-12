cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;	
};


cbuffer TickeBuffer
{
	float  time;
	float3 padding;
};


cbuffer LightBuffer
{
	float4 ambientLight;
	float4 diffuseLight;
	float3 vLightDirection;
	float  padd;		// TO MAKE MULTIPLE OF 16
};

struct InputType
{
	float4 Pos : POSITION;		
	float3 normal : NORMAL;	
	float4 Col : COLOR0;
};

struct OutputType
{
	float4 position : SV_POSITION;	
	float4 wPos    : TEXCOORD0;
	float2 tex : TEXCOORD1;	
	float4 normal : NORMAL;	
	float4 Color : COLOR0;
};


OutputType VS(InputType input)
{
	OutputType output;
	input.Pos.w =1;

	output.tex = float2(input.Pos[0] * 22.0 / 2816.0, input.Pos[2] * 16.5 / 2112.0);

	float s_X = 0;
	float s_Y = 0;
	float s_Z = 0;
	float2 X0 = float2(input.Pos[0], input.Pos[2]);
	float height = 0;
	for (int i = 0; i < 3; i++)
	{
		float2 K = float2(1, i);
		float w = 1.5 * (i + 1) / 3.0; // w is frequency		
		float w2 = 0.5 * (i + 1) / 5.0; // w is frequency		
		float a = 0.005; // a is amplitude
		float a2 = 0.0001; // a is amplitude
		/*
		K is a wave vector

		*/
		// X = X0 - (K/k)(a*sin(K*X0 - wt);
		// y = a * cos (K*X0 - wt);
		//float h = 0.01 * sin(dot(K, X0) + w*time*3);
		float2 X = X0 - K * a*sin(dot(K, X0) - w * time*2.5) - K * a2 *sin(dot(K, X0) - w2 * time*2.5);
			float  y = a * cos(dot(K, X0) - w*time*2.5) + a2 * cos(0.5 * (dot(K, X0) - w*time*2.5));
		s_X += X[0] / 64 - 0.5f;
		s_Y += X[1] / 64 - 0.5f;
		s_Z += y;
	}
	input.Pos[0] = s_X;
	input.Pos[2] = s_Y;
	input.Pos[1] = s_Z;
		
	input.Pos *= 7.5f;
	input.Pos[3] /= 7.5f;
	
	//float y, a = 1.0, w = 0.5, k = 0.1;	
	//y = a * sin(k - w * time);
	//input.Pos.y = y;
	
	output.position = mul(input.Pos, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.normal = mul(float4(input.normal, 1.0f), world);
	output.wPos = mul(input.Pos, world);
	//output.tex = input.tex;
	//float3 vLightDirection = (-1, 0, 0.25);
	float4 vLightColor = (0.2, 0.7, 0.8, 0.7);
	output.Color = ambientLight * saturate(dot(vLightDirection, input.normal));
	//output.Color += float4(0.1, 0.2, 0.5, 1);
	//output.Color /= 1.75;
	return output;
}


Texture2D waterTexture;
SamplerState textureSampler;

float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
		return abs(noise.x + noise.y) * 0.5;
}

float4 PS(OutputType input) : SV_TARGET
{
	/*float4 pixelColor;
	pixelColor = waterTexture.Sample(textureSampler, input.tex);*/
	float4 watercolor = { 0.125, 0.12, 0.3, 1 };
	/*float4 watercolor = { 9/255, 255/255, 70/255, 1 };

	if (input.wPos.y > -5)
		watercolor = float4(1, 0, 0, 0.5);	


	return watercolor;*/

	
	float4 old_color = input.Color;
	old_color[2] += (rand_1_05(float2(input.position[2], input.position[2] * 2)))*0.1;
	float4 color = input.Color;
	float3 text = waterTexture.Sample(textureSampler, float2(input.tex[0], input.tex[1]));
	color[0] = text[0]; color[1] = text[1]; color[2] = text[2];

	old_color = (old_color + color) / 2.0;


	//return old_color;
	return float4(0.0f, text[1], text[2], 1.0f);
	
}

