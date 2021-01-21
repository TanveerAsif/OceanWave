

#define PI 3.14159265358979323846

cbuffer MatrixBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;	
	float3 vCamPositon;
};



cbuffer TickeBuffer : register(b1)
{
	float  time;
	float3 padding;
};

cbuffer FogStartEnd : register(b3)
{
	float fFogStart;
	float fFogEnd;
	float2 fFogPadding;
};

cbuffer LightBuffer : register(b2)
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
	float  fFogFactor : FOG;
};


float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}


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
		//float w = 1.5 * (i + 1) / 3.0; // w is frequency		
		float w = 2.5 * (i + 1) / 3.0; // w is frequency		
		//float a = 0.005; // a is amplitude
		float a = 0.005; // a is amplitude
		/*
		K is a wave vector

		*/
		// X = X0 - (K/k)(a*sin(K*X0 - wt);
		// y = a * cos (K*X0 - wt);
		//float h = 0.01 * sin(dot(K, X0) + w*time*3);
		float2 X = X0 - K * a*sin(dot(K, X0) - w * time*2.5);
		float  y = a * cos(dot(K, X0) - w*time*2.5) + (a * 0.5) * cos(dot(K, X0) - w*time*0.5) + (a * 0.025) * cos(dot(K, X0) - w*time*0.025);
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


	//adding fog effect
	float4 vertexInCamPos = mul(input.Pos, world);
	vertexInCamPos = mul(vertexInCamPos, view);
	float fogStart = fFogStart, fogEnd = fFogEnd;
	//if (fogEnd > 0)
	float fFogEffectRegon = 300 + rand_1_05(float2(input.Pos[0], input.Pos[1] * 2));
	if (vCamPositon.y < 0.0f)
	{
		fogEnd = 10.0f;
		fogStart = 1.0f;
		output.fFogFactor = saturate((fogEnd - vertexInCamPos.z) / (fogEnd - fogStart));
	}
	else
		output.fFogFactor = 1.0f;


	float4 vLightColor = (0.2, 0.7, 0.8, 0.7);
	output.Color = ambientLight * saturate(dot(vLightDirection, input.normal)) ;
	//output.Color += float4(0.1, 0.2, 0.5, 1);
	//output.Color /= 1.75;
	return output;
}


Texture2D waterTexture;
SamplerState textureSampler;


float4 PS(OutputType input) : SV_TARGET
{	
	float4 old_color = input.Color;
	old_color[2] += (rand_1_05(float2(input.position[2], input.position[2] * 2)))*0.1;
	float4 color = input.Color;
	float3 text = waterTexture.Sample(textureSampler, float2(input.tex[0], input.tex[1]));
	color[0] = text[0]; color[1] = text[1]; color[2] = text[2];

	old_color = (old_color + color) / 2.0;


	/*return old_color;*/
	//float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f); //Grey	
	//float4 fogColor = float4(201 / 255.0f, 220 / 255.0f, 241 / 255.0f, 1.0f); 	
	float4 fogColor = float4(30 / 255.0f, 89 / 255.0f, 187 / 255.0f, 1.0f);
	float4 waterColour = float4(0, text[1] * text[2], text[2], 1.0f);	
	
	float4 finalColour = input.fFogFactor * waterColour + (1.0f - input.fFogFactor) * fogColor;	
	if (input.fFogFactor  < 1.0f)
	{
		finalColour = lerp(finalColour, waterColour, 0.1*input.fFogFactor);
	}	

	return finalColour;
	
	

		
	float fWindDir = 0.523598;  //30Deg -> ..rad
	float fWindForce = 1.0f;

	float3 wind = float3(cos(fWindDir), sin(fWindDir), 0);



	float kangle = fWindDir + text[2] *(float) PI / 12.0f;
	float3   kdir = float3(cos(kangle), sin(kangle), 0.0f);


	float  k = (0.5f + 0.5f*text[0]) / pow(2.0f, 0.2f * (1.0f - input.tex) + 7.0f * input.tex);


	float  w = sqrt(981.0f * k);


	float A = (fWindForce)*0.008f*(text[2]) / k;

	// make sure no waves are producing loops !
	if (k*abs(A) > 1.0f) {
		A = (A > 0.0f ? 1.0f : -1.0f)*1.0f / k;
	}

	//////ret.b = 3.0*k*kdir[0]; // kx
	//////ret.a = 3.0*k*kdir[1]; // ky

	//////ret.x = A;
	//////ret.y = w;
	//return float4(A, w, 3.0*k*kdir[0], 3.0*k*kdir[1]);

	//////float3	watercolor = { 0.04, 0.08, 0.3 };
	//////float2 TcWindShift = float2(sin(fWindDir - PI / 2), cos(fWindDir - PI / 2));

	//////float fViewDistance = 1.0f//length(In.vViewVec);


	//////float3 suncolor = float3(input.Color[0], input.Color[1], input.Color[2]);

	//////float4 sky_clr = float4(0.0f, 0.0f, 0.7f, 1.0f);// texCUBE(S_EnvMap, Reflect.xyz);	

	//////float lux_trsl = 0.2f;//sky_clr.w;
	//////float lux = lux_trsl*lux_trsl;
	//////lux = lux*lux;

	//////float fLocalWindForce = saturate(fWindForce);
	////////port diffuse map///////////////////////////
	//////float3 SeaColor = watercolor;
	//////float fPort = 0;		
	////////fPort = tex2D(S_PortDiffMap, In.NrmCoord_PM.zw).r;
	//////SeaColor = saturate(float3(watercolor.r*0.3, watercolor.g + 0.5, watercolor.b*0.7));
	//////SeaColor = lerp(watercolor, SeaColor, saturate(fPort*(1.0 - fLocalWindForce)));
	//////////
	///////////////////////////////////////////////////

	//////////// faked water transluence effectD
	//////////float3 sea_clr_top = SeaColor;

	//////////float3 bottom_clr = SeaColor*0.55;

	//////////float interp = /*In.Transluence*/(nI.y + 1.0f)*(0.2 + 0.8*lux_trsl);
	//////////float3 sea_clr = (lerp(bottom_clr, sea_clr_top, interp))*suncolor.r;

	//////////float2 refl_tc = In.tPosRefl.xy - In.tPosRefl.z*Ng.xz*1.5;

	//////////float3 Rsun = normalize(2 * dot(Ng, sun_vec) * Ng - sun_vec);
	//////////float Specular = shadow>0.5 ? pow(saturate(dot(Rsun, nI)), sun_shininess)*sun_strength : 0.0f;

	//////////float4 local_refl = tex2D(reflmap, refl_tc);
	//////////float4 local_refr = tex2D(refrmap, In.tPosRefr.xy - In.tPosRefr.z*Ng.xz*0.2);
	//////////float fSunFac = 1.0 - local_refl.a;
	//////////local_refr.a = saturate((1.0 - fLocalWindForce*2.0f)*local_refr.a*g_fRefractionFactor);
	//////////sea_clr = lerp(sea_clr, local_refr.rgb, local_refr.a);

	//////////local_refl.a *= saturate(max(0.75, 1.0 - suncolor.r))*g_fReflectionFactor;

	//////////sky_clr.rgb = lerp(sky_clr.rgb, local_refl.rgb, local_refl.a);//saturate(local_refl.a*(1.0-fLocalWindForce)));

	//////////float4 ret = float4(
	//////////	(reflectivity * sky_clr + (1 - reflectivity) * sea_clr + 0.8*lux.xxx)
	//////////	, 1);
	////////////return ret;
	/////////////////////////////////////////////////

	////////////////surface textures////////////////////
	//////////////////sea foam///////////////////
	//////////float fTexSpeed = 2.0;//max(fWindForce*2.0f, 1.0f);
	//////////float4 foam = tex2D(S_seafoam, In.FoamTc.xy*2.23 + TcWindShift*Time / 150 * fTexSpeed);
	//////////float f1 = 0;//(tex2D(S_normalmap_mp,In.FoamTc.xy*0.5+TcWindShift*Time/75*fTexSpeed).a);
	//////////			 //f1 *= (tex2D(S_normalmap_mp,In.FoamTc.xy*5.0+float2(1.0,0.75)*TcWindShift*Time/50*fTexSpeed).a);

	//////////			 //f1 = saturate(fLocalWindForce*0.25+f1*f1+In.WorldPos.y*abs(dot(Ng,float3(-TcWindShift.x,0,-TcWindShift.y)))) * foam.a;
	//////////			 //f1 = saturate(fLocalWindForce*0.25+f1*f1+abs(dot(Ng*2.0,3.0*float3(-TcWindShift.x,0,-TcWindShift.y)))) * foam.a;
	//////////f1 = saturate(fLocalWindForce*0.15 + abs(dot(2.0*Ng, 3.0*float3(-TcWindShift.x, 0, -TcWindShift.y))))* foam.a;
	////////////////////////////////////////////////////  

	//////////ret.rgb = lerp(ret.rgb, 0.8, saturate(In.FoamTc.z*f1));


	//////////float4 oilspill = tex2D(oilspillmap, In.Dof_OS.yz)*suncolor.r;
	//////////ret.rgb = lerp(ret.rgb, oilspill.rgb, oilspill.a*0.75);


	//////////float3  fWindFoam = tex2D(S_SurfaceDiffuseMap, In.FoamTc.xy + TcWindShift*Time / 80)*suncolor;//*saturate(200.0/fViewDistance);
	//////////fWindFoam *= tex2D(S_SurfaceDiffuseMap, In.FoamTc.xy*0.3 + TcWindShift*Time / 220)*suncolor;//*saturate(200.0/fViewDistance);
	//////////ret.rgb += saturate((0.14 + fLocalWindForce*0.5)*fWindFoam)*0.6;


	////////////fMistDistance = 20000;
	//////////float f = 1;
	//////////if (fog_end<35000)
	//////////	f = hetro_fog(fViewDistance, In.WorldPos);
	//////////float4 fWake = tex2Dproj(samplerWakeMap, In.vWM_TC);
	//////////ret.rgb = saturate(lerp(ret.rgb, fWake.rgb*sun_color.r, fWake.a/**(1.0-fLocalWindForce)*/)) + sun_color*Specular*fSunFac*2.0;
	//////////ret.rgb = lerp(fog_color, ret.rgb*shadow, f);
	
	/*return SeaColor;*/
	
	
}

