#include "Dx11_Graphics.h"
#include <D3DX10math.h>
#include <mmsystem.h>


//////////////////////////////////////////////////////////////////////////
////FLEX 
//#include <NvFlex.h>
//#include <NvFlexExt.h>
//#include <NvFlexDevice.h>
//
//#include "types.h"
//#include "maths.h"
//#include "platform.h"
//#include "mesh.h"
//#include "voxelize.h"
//#include "sdf.h"
//#include "pfm.h"
//#include "tga.h"
//#include "perlin.h"
//#include "convex.h"
//#include "cloth.h"

//////////////////////////////////////////////////////////////////////////

const float SCREEN_DEPTH = 10000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOW_WIDTH = 1024; 
const int SHADOW_HEIGHT = 1024;

bool g_bComeOutFromWater = false;
bool g_bRenderWatertoAir = false;
float g_fInAirDuration = 0.0f;

Dx11_Graphics::Dx11_Graphics()
{	
	m_pDirect3D = new Dx11_Direct3D();
	m_pShader = new Dx11_Shaders();
	m_pModel = new Dx11_Model();
	m_pCamera = new Dx11_Camera();

	m_pFloorModel = new Dx11_Model();
	m_pReflectionShader = new Dx11_Shaders();


	/*m_pRender2Texture = new Dx11_Render2Texture();*/


	m_pCubeModel = new Dx11_Model();
	m_pCubeShader = new Dx11_Shaders();


	m_fTickCount = 0.0f;
	m_bRenderState = false;
	
	m_pText = new Dx11_Text();


	m_pDepthModel = new Dx11_Model();
	m_pDepthShader = new Dx11_Shaders();
	
	m_pSky = new Dx11_Sky();
	m_pSkyShader = new Dx11_Shaders;


	m_pLightSource = new Dx11_LightSource;
	m_pLightSourceShader = new Dx11_Shaders;

	m_pLightShaftGrid = new Dx11_Model;
	m_pLightShaftShader = new Dx11_Shaders;


	m_pParticleShader = new Dx11_Shaders;
	m_pParticleSystem = new Dx11_ParticleSystem;


	//Initialy WireFrame Mode OFF
	m_bIsWireFrameEnable = false;


	//RENDER TO TEXTURE FOR REFLECTIONS
	/*m_pReflectionSurface = new Dx11_Render2Texture();*/

	m_bSunLightEnable = true;
	m_bLightShaftEnable = false;

	//SHADOW MAP
	m_pShadowMapSurface = new Dx11_Render2Texture();
	m_pShadowMapShader = new Dx11_Shaders;
	m_pDepthMapShader = new Dx11_Shaders;

	
	//SUN
	m_pSun = new Dx11_Model();

	m_pDepthMapModel = new Dx11_Model();
	m_pDepthMapModelShader = new Dx11_Shaders();

	m_bShowDepthMapEnable = false;

	m_pOceanWater = new Dx11_Ocean();
	m_pOceanShader = new Dx11_Shaders();

	//FOG SHADER	
	m_fFogStart = 0.0f;
	m_fFogEnd = 0.0f;


	m_pScreenSurface = new Dx11_Render2Texture();
	m_pOnScreenRendering = new Dx11_OnScreenRendering();


	//ARROW
	m_pArrowMesh = new Dx11_Model();
	m_pArrowShader = new Dx11_Shaders();

}


Dx11_Graphics::~Dx11_Graphics()
{

}


bool Dx11_Graphics::Initialize(HWND hWnd)
{	

	bool bRetValue = false;
	
	//m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	//m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	m_nScreenWidth = 1500;
	m_nScreenHeight = 900;
	
	if (m_pDirect3D->Initialize(hWnd, m_nScreenWidth, m_nScreenHeight))
	{		

		//m_pCamera->SetPosition(D3DXVECTOR3(-8.0f, -37.0f, -101.0f));
		
		
		ID3D11Device *pDevice = m_pDirect3D->GetDevice();
		ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
		

		////LOAD MODEL FOR LIGHT RENDERING
		bRetValue = m_pModel->Initialize(pDevice, pDeviceContext, L"Data/filight.dds");
		if (!bRetValue)
			return bRetValue;
		bRetValue = m_pShader->InitializeLightShader(pDevice, pDeviceContext, L"Data/MyLightShader.hlsl");
		if (!bRetValue)
			return bRetValue;
		
		
		////LOAD FLOOR		
		bRetValue = m_pFloorModel->LoadFloor(pDevice, L"Data/blue01.dds");
		if (!bRetValue)
			return bRetValue;				
		

		////SETTING LENGHT AND WIDTH OF TEXTURE ON WIHCH REFLECTION IS GOING TO FALL as Floor L=35 and W=40
		//bRetValue = m_pRender2Texture->Initialize(pDevice, m_nScreenWidth, m_nScreenHeight, SCREEN_NEAR, SCREEN_DEPTH);
		//if (!bRetValue)
		//	return bRetValue;


		//CUBE
		bRetValue = m_pCubeModel->LoadCube(pDevice, L"Data/seafloor.dds");
		if (!bRetValue)
			return bRetValue;		
		bRetValue = m_pCubeShader->InitializeFloorShader(pDevice, pDeviceContext, L"Data/MyShaderFloor.hlsl");
		if (!bRetValue)
			return bRetValue;

#ifdef _FI_SIMLIGHTEX_
		AddLight(LGT_FLASHING, LGT_WHITE);
		AddLight(LGT_QUICK_FLASHING, LGT_GREEN);
		AddLight(LGT_QUICK_FLASHING, LGT_RED);
		AddLight(LGT_FIXED, LGT_WHITE);
		AddLight(LGT_ULTRA_QUICK_FLASHING, LGT_YELLOW);
		AddLight(LGT_ISOPHASED, LGT_BLUE);
#endif

		/*m_pCamera->Process();	*/	
		D3DXMATRIX matViewMatrix = m_pCamera->GetViewMatrix();
		bRetValue = m_pText->Initialize(pDevice, pDeviceContext,
										"Data/fontdata.txt", L"Data/font.dds", 
										matViewMatrix,
										m_nScreenWidth, m_nScreenHeight
										);
		if (!bRetValue)
			return bRetValue;

		//Z-BUFFER TESTING
		bRetValue = m_pDepthModel->LoadFloorDepthTest(pDevice);
		if (!bRetValue)
			return bRetValue;
		bRetValue = m_pDepthShader->InitializeDepthBufferShader(pDevice, L"Data/MyDepthShader.hlsl");
		if (!bRetValue)
			return bRetValue;		


		//SKY
		//cloud002.dds
		//sky sunset blue gold clouds.jpg		
		bRetValue = m_pSky->InitSky(pDevice, L"Data/sky.jpg");		
		if (!bRetValue)
			return bRetValue;
		bRetValue = m_pSkyShader->InitializeSkyShader(pDevice, L"Data/MySkyShader.hlsl");
		if (!bRetValue)
			return bRetValue;


		//LIGHT SOURCE
		m_pLightSource->SetDirection(D3DXVECTOR3(-1, -1, 1));		
		m_pLightSource->SetDiffuseLight(D3DXVECTOR4(0.15, 0.15, 0.15, 1.0f));
		m_pLightSource->SetAmbientLight(D3DXVECTOR4(0.8, 0.8, 0.8, 1.0f));
		bRetValue = m_pLightSourceShader->InitializeLightSourceShader(pDevice, L"Data/MyLightSource.hlsl");
		if (!bRetValue)
			return bRetValue;



		//CREATING GRID OF 100 x 100 VERTICES FOR LIGHT SHAFT
		bRetValue = m_pLightShaftGrid->CreateLightShaftGrid(pDevice, 100, L"Data/cloudparticle.dds");
		if (!bRetValue)
			return bRetValue;


		//LIGHT SHAFT SHADER
		bRetValue = m_pLightShaftShader->InitializeLightShaftShader(pDevice, L"Data/MyLightShaftShader.hlsl");
		if (!bRetValue)
			return bRetValue;

				
		//PARTICLE SYSTEM
		bRetValue = m_pParticleSystem->Inittialize(pDevice, L"Data/star.dds");
		if (!bRetValue)
			return bRetValue;

		bRetValue = m_pParticleShader->InitializeParticleSystemShader(pDevice, L"Data/MyParticleShader.hlsl");
		if (!bRetValue)
			return bRetValue;



		////REFLECTION SHADER
		//bRetValue = m_pReflectionShader->InitializeReflectionShader(pDevice, pDeviceContext, L"Data/MyReflectionShader.hlsl");
		//if (!bRetValue)
		//	return bRetValue;

		////RENDER2TEXTURE FOR REFLECTION
		//bRetValue = m_pReflectionSurface->Initialize(pDevice, m_nScreenWidth, m_nScreenHeight, SCREEN_NEAR, SCREEN_DEPTH);
		//if (!bRetValue)
		//	return bRetValue;



		/////////////////////////////////////////////////////////////////////////////////////////
		//SHADOW MAP OF SUN LIGHT
		bRetValue = m_pShadowMapSurface->Initialize(pDevice, SHADOW_WIDTH, SHADOW_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);
		if (!bRetValue)
			return bRetValue;
				
		bRetValue = m_pDepthMapShader->InitializeDepthMapShader(pDevice, L"Data/MyDepthMapShader.hlsl");
		if (!bRetValue)
			return bRetValue;

		bRetValue = m_pShadowMapShader->InitializeShadowMapShader(pDevice, L"Data/MyShadowShader.hlsl");
		if (!bRetValue)
			return bRetValue;
		/////////////////////////////////////////////////////////////////////////////////////////



		/////////////////////////////////////////////////////////////////////////////////////////
		//SUN
		m_pSun->SetPosition(D3DXVECTOR3(0.0f, 10.0f, -0.5f));
		m_pSun->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		bRetValue = m_pSun->LoadSun(pDevice, L"Data/sun.dds", D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		if (!bRetValue)
			return bRetValue;

		/////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////
		//DRAW FOUR VERTICES TO HOLD DEPTH MAP
		bRetValue = m_pDepthMapModel->LoadDepthMapModel(pDevice);
		if (!bRetValue)
			return bRetValue;

		bRetValue = m_pDepthMapModelShader->InitializeFloorShader(pDevice, pDeviceContext, L"Data/MyDepthMapModelShader.hlsl");
		if (!bRetValue)
			return bRetValue;
		/////////////////////////////////////////////////////////////////////////////////////////




		/////////////////////////////////////////////////////////////////////////////////////////
		//ocean.png 
		//ocean2.dds
		//wave1.bmp
		//surface_normals.dds
		bRetValue = m_pOceanWater->InitializeOcean(pDevice, L"Data/ocean2.dds");
		if (!bRetValue)
			return bRetValue;

		bRetValue = m_pOceanShader->InitializeOceanShader(pDevice, L"Data/MyOceanShader.hlsl");
		if (!bRetValue)
			return bRetValue;

		/////////////////////////////////////////////////////////////////////////////////////////



		/////////////////////////////////////////////////////////////////////////////////////////
		//OnScreenEffects
		bRetValue = m_pOnScreenRendering->initScreenSpace(pDevice);
		if (!bRetValue)
			return bRetValue;
		
		bRetValue = m_pScreenSurface->Initialize(pDevice, 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);
		if (!bRetValue)
			return bRetValue;
		
		bRetValue = m_pArrowMesh->LoadArrowModel(pDevice);
		if (!bRetValue)
			return bRetValue;

		bRetValue = m_pArrowShader->InitializeArrowShader(pDevice, L"Data/MyArrowShader.hlsl");
		if (!bRetValue)
			return bRetValue;

	}
	else
		MessageBox(hWnd, L"COULD NOT INITIALIZE Direct3D", L"error", MB_ICONEXCLAMATION | MB_OK);
	
	

		//////////////////////////////////////////////////////////////////////////////////
		//////////FLEX 
		////////NvFlexLibrary* library = NvFlexInit();
		////////NvFlexSolver* solver = NvFlexCreateSolver(library);

		////////NvFlexBuffer* particleBuffer = NvlexAllocBuffer(library, n, sizeof(Vec4), eNvFlexBufferHost);
		////////NvFlexBuffer* velocityBuffer = NvlexAllocBuffer(library, n, sizeof(Vec4), eNvFlexBufferHost);
		////////NvFlexBuffer* phaseBuffer = NvlexAllocBuffer(library, n, sizeof(int), eNvFlexBufferHost);

		////////while (!done)
		////////{
		////////	// map buffers for reading / writing
		////////	float4* particles = (float4*)NvlexMap(particles, eNvFlexMapWait);
		////////	float3* velocities = (float3*)NvFlexMap(velocities, eNvFlexMapWait);
		////////	int* phases = (int*)NvFlexMap(phases, eNvFlexMapWait);

		////////	// spawn (user method)
		////////	SpawnParticles(particles, velocities, phases);

		////////	// render (user method)
		////////	RenderParticles(particles, velocities, phases);

		////////	// unmap buffers
		////////	NvFlexUnmap(particleBuffer);
		////////	NvFlexUnmap(velocityBuffer);
		////////	NvFlexUnmap(phaseBuffer);

		////////	// write to device (async)
		////////	NvFlexSetParticles(particleBuffer, n);
		////////	NvFlexSetVelocities(velocityBuffer, n);
		////////	NvFlexSetPhases(phaseBuffer, n);

		////////	// tick
		////////	NvFlexUpdateSolver(solver, dt, 1, false);

		////////	// read back (async)
		////////	NvFlexGetParticles(particleBuffer, n);
		////////	NvFlexGetVelocities(velocityBuffer, n);
		////////	NvFlexGetPhases(phaseBuffer, n);
		////////}

		////////NvFlexFreeBuffer(particleBuffer);
		////////NvFlexFreeBuffer(velocityBuffer);
		////////NvFlexFreeBuffer(phaseBuffer);

		////////NvFlexDestroySolver(solver);
		////////NvFlexShutdown(library);
		//////////////////////////////////////////////////////////////////////////
		
	return bRetValue;
}

void Dx11_Graphics::Process(float _fTick)
{
	/*
	m_pModel->Process();
	m_pShader->Process();	
	*/	
	m_pCubeModel->ProcessCube(_fTick);
	m_pParticleSystem->Prcoess(m_pDirect3D->GetDeviceContext(), _fTick);
	m_pOceanWater->Process(_fTick);

	D3DXVECTOR3 vCameraPosition = m_pCamera->GetPosition();
	if (vCameraPosition.y == 0)
	{
		 m_pOnScreenRendering->Process(_fTick);
	}

	if (vCameraPosition.y < 0)
	{
		g_bComeOutFromWater = true;
		g_bRenderWatertoAir = false;
		g_fInAirDuration = 0.0f;			
	}
		
	if (g_bComeOutFromWater)
	{
		if (vCameraPosition.y >= 1)
		{
			g_bRenderWatertoAir = true;			
			g_bComeOutFromWater = false;
		}
	}

	if (g_bRenderWatertoAir)
	{
		g_fInAirDuration += _fTick;
		if (g_fInAirDuration < 2.0f)
		{
			m_pOnScreenRendering->SetWaterToAirState(true);			
		}		
		else
		{		
			g_bRenderWatertoAir = false;									
			m_pOnScreenRendering->SetWaterToAirState(false);
		}		
	}
	

}

void Dx11_Graphics::Render(float _fTick)
{	
	//RenderToTexture(_fTick);
	//RenderReflectionSurface(_fTick);
	
	//RenderShadowMapOfSunLight(_fTick);

	D3DXVECTOR3 vCameraPosition = m_pCamera->GetPosition();
	if (vCameraPosition.y == 0)
	{
		RenderWaterEffectsOnScreen(_fTick);
	}
	else
	{
		RenderScene(_fTick);
	}
	//RenderScene(_fTick);


	
	
	
}

void Dx11_Graphics::Shutdown()
{

	if (m_pOceanWater)
	{
		m_pOceanWater->Shutdown();
		delete m_pOceanWater;
		m_pOceanWater = NULL;
	}
	
	if (m_pDepthMapModel)
	{
		m_pDepthMapModel->Shutdown();
		delete m_pDepthMapModel;
		m_pDepthMapModel = NULL;
	}

	if (m_pDepthMapModelShader)
	{
		m_pDepthMapModelShader->Shutdown();
		delete m_pDepthMapModelShader;
		m_pDepthMapModelShader = NULL;
	}



	if (m_pSun)
	{
		m_pSun->Shutdown();
		delete m_pSun;
		m_pSun = 0;
	}


	/////////////////////////////////////////////////////
	//SHADOW MAP OF SUN LIGHT
	if (m_pShadowMapSurface)
	{
		m_pShadowMapSurface->Shutdown();
		delete m_pShadowMapSurface;
		m_pShadowMapSurface = 0;
	}

	if (m_pShadowMapShader)
	{
		m_pShadowMapShader->Shutdown();
		delete m_pShadowMapShader;
		m_pShadowMapShader = 0;
	}
	/////////////////////////////////////////////////////



	/////////////////////////////////////////////////////
	//SHADOW MAP OF REFLECTION SURFACE
	/*if (m_pReflectionSurface)
	{
		m_pReflectionSurface->Shutdown();
		delete m_pReflectionSurface;
		m_pReflectionSurface = 0;
	}*/
	/////////////////////////////////////////////////////




	/////////////////////////////////////////////////////
	//PARTICLE 
	if (m_pParticleShader)
	{
		m_pParticleShader->Shutdown();
		delete m_pParticleShader;
		m_pParticleShader = 0;
	}

	if (m_pParticleSystem)
	{
		m_pParticleSystem->Shutdown();
		delete m_pParticleSystem;
		m_pParticleSystem = 0;
	}
	/////////////////////////////////////////////////////

	if (m_pLightShaftShader)
	{
		m_pLightShaftShader->Shutdown();
		delete m_pLightShaftShader;
		m_pLightShaftShader = 0;
	}

	if (m_pLightShaftGrid)
	{
		m_pLightShaftGrid->Shutdown();
		delete m_pLightShaftGrid;
		m_pLightShaftGrid = 0;
	}

	if (m_pLightSource)
	{
		//ONLY TWO VARIABLES, NO DYNAMIC ALLOATED MEMORIES
		delete m_pLightSource;		
		m_pLightSource = 0;
	}


	if (m_pLightSourceShader)
	{
		m_pLightSourceShader->Shutdown();
		delete m_pLightSourceShader;
		m_pLightSourceShader = 0;
	}


	if (m_pSkyShader)
	{
		m_pSkyShader->Shutdown();
		delete m_pSkyShader;
		m_pSkyShader = 0;
	}

	if (m_pSky)
	{
		m_pSky->Shutdown();
		delete m_pSky;
		m_pSky = 0;
	}

	/*if (m_pRender2Texture)
	{
	m_pRender2Texture->Shutdown();
	delete m_pRender2Texture;
	m_pRender2Texture = 0;
	}*/



	if (m_pReflectionShader)
	{
		m_pReflectionShader->Shutdown();
		delete m_pReflectionShader;
		m_pReflectionShader = 0;
	}


	if (m_pShader)
	{
		m_pShader->Shutdown();
		delete m_pShader;
		m_pShader = 0;
	}


	if (m_pFloorModel)
	{
		m_pFloorModel->Shutdown();
		delete m_pFloorModel;
		m_pFloorModel = 0;
	}



	if (m_pModel)
	{
		m_pModel->Shutdown();
		delete m_pModel;
		m_pModel = 0;
	}


	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = 0;
	}

	if (m_pDirect3D)
	{
		m_pDirect3D->Shutdown();
		delete  m_pDirect3D;
		m_pDirect3D = 0;
	}


}

void Dx11_Graphics::RenderReflectionSurface(float _fTick)
{
	////ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
	////
	////m_pReflectionSurface->SetRenderTarget(pDeviceContext);
	////m_pReflectionSurface->BeginScene(pDeviceContext);


	////D3DXMATRIX worldMat, projMat, reflectionMatrix;	
	////
	////worldMat = m_pDirect3D->GetWorldMatrix();
	////D3DXMatrixRotationY(&worldMat,  m_pCubeModel->GetHeading());	
	////reflectionMatrix = m_pCamera->GetReflectionMatrix(D3DXVECTOR3(0.1, -8.0, -1.0), D3DXVECTOR3(0.1f, 1.0f, 0.0f));
	////projMat = m_pReflectionSurface->GetProjectionMatrix();// m_pDirect3D->GetPerspectiveProjectionMatrix();
	////
	////m_pCubeModel->RenderCube(m_pDirect3D->GetDeviceContext());
	////m_pCubeShader->RenderShader(m_pDirect3D->GetDeviceContext(), m_pCubeModel->GetCubeIndexCount(), worldMat, reflectionMatrix, projMat, m_pCubeModel->GetTextureSRView());
	////
	////m_pDirect3D->SetBackBufferRenderTarget();
}

void Dx11_Graphics::RenderShadowMapOfSunLight(float _fTick)
{
	
	ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();

	m_pShadowMapSurface->SetRenderTarget(pDeviceContext);
	m_pShadowMapSurface->BeginScene(pDeviceContext);

	D3DXMATRIX worldMat, sunViewMatrix, sunProjMatrix, matScalingMatrix, matRotationMatrix;

	worldMat = m_pDirect3D->GetWorldMatrix();
	D3DXMatrixIdentity(&matRotationMatrix);
	D3DXMatrixRotationY(&matRotationMatrix, -1 * m_pCubeModel->GetHeading());	
	worldMat = matRotationMatrix * worldMat;	

	//PUTTING SUN IN 4th-QUADRANT
	D3DXVECTOR3 vSunDir = D3DXVECTOR3(0, 0, 0) - m_pSun->GetPosition();
	D3DXVec3Normalize(&vSunDir, &vSunDir);
	sunViewMatrix = m_pCamera->GetViewMatrix(m_pSun->GetPosition(), D3DXVECTOR3(0, 0, 0));
	sunProjMatrix = m_pShadowMapSurface->GetOrthProjectionMatrix();

	m_pCubeModel->RenderCube(m_pDirect3D->GetDeviceContext());
	m_pDepthMapShader->RenderCubeShader(m_pDirect3D->GetDeviceContext(), m_pCubeModel->GetCubeIndexCount(), worldMat, sunViewMatrix, sunProjMatrix, m_pCubeModel->GetTextureSRView());


	m_pDirect3D->SetBackBufferRenderTarget();

}


void Dx11_Graphics::RenderToTexture(float _fTick)
{
	////////D3DXMATRIX worldMat, projMat, reflectionMatrix;
	////////D3DXMATRIX matTranslationMatrix, matScalingMatrix;


	////////ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
	////////
	////////float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//////////NOW RENDER TO TEXTURE
	//////////m_pDirect3D->SetDepthBufferOFF();
	////////m_pRender2Texture->SetRenderTarget(pDeviceContext);
	////////m_pRender2Texture->BeginScene(pDeviceContext);
	////////
	////////

	//////////
	//////////FI_SimLightEx *pLight = NULL;
	//////////std::vector<FI_SimLightEx *>::iterator Itr;

	//////////float nCounter = 0;
	//////////for (Itr = m_vLights.begin(); Itr != m_vLights.end(); Itr++)
	//////////{
	//////////	pLight = (FI_SimLightEx *)(*Itr);
	//////////	if (pLight->GetLightState())
	//////////	{
	//////////		nCounter += 1.5;
	//////////		std::vector<FI_LightSector *>::iterator ItrSector;
	//////////		for (ItrSector = pLight->m_vSectors.begin(); ItrSector != pLight->m_vSectors.end(); ItrSector++)
	//////////		{
	//////////			FI_LightSector *pSector = (*ItrSector);
	//////////			if (pSector)
	//////////			{
	//////////				if (pSector->GetSectorLightStatus())
	//////////				{
	//////////					D3DXMatrixIdentity(&worldMat);
	//////////					D3DXMatrixIdentity(&matTranslationMatrix);
	//////////					D3DXMatrixTranslation(&matTranslationMatrix, nCounter, 0.0f, 0.0f);
	//////////					worldMat = worldMat* matTranslationMatrix;

	//////////					m_pCamera->RenderReflection(D3DXVECTOR3(nCounter+10.0, 0.0f, 2.0f), -1.2f);
	//////////					reflectionMatrix = m_pCamera->GetReflectionViewMatrix();

	//////////					m_pModel->Render(m_pDirect3D->GetDevice(), m_pDirect3D->GetDeviceContext(), pSector->GetSectorLightColor(), pSector->GetSectorLightSize());
	//////////					m_pShader->RenderShader(m_pDirect3D->GetDeviceContext(), m_pModel->GetIndexCount(), worldMat, reflectionMatrix, prespectiveProj, m_pModel->GetTextureSRView());
	//////////				}
	//////////			}

	//////////		}
	//////////	}
	//////////}
	//////////

	////////worldMat = m_pDirect3D->GetWorldMatrix();	
	////////reflectionMatrix = m_pCamera->GetReflectionMatrix(D3DXVECTOR3(0, 1.5*m_pSky->GetRadius() + 5, 0), m_pLightSource->GetLightDir());	
	////////projMat = m_pDirect3D->GetPerspectiveProjectionMatrix();
	////////
	////////m_pSky->Render(pDeviceContext);
	////////m_pSkyShader->RenderShader(pDeviceContext, m_pSky->GetIndexCount(), worldMat, reflectionMatrix, projMat, m_pSky->GetTexture());
	////////

	//////////m_pDirect3D->SetDepthBufferON();
	////////m_pDirect3D->SetBackBufferRenderTarget();
}



void Dx11_Graphics::RenderScene(float _fTick)
{
	if (m_pDirect3D->BeginScene())
	{
		m_fTickCount += _fTick;

		m_pDirect3D->EnableBlendState();
		ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();

		D3DXMATRIX viewMat, worldMat, projectionMat, reflectionMat, orthoMat, sunViewMatrix, sunProjMatrix;
		D3DXMATRIX matTranslationMatrix, matScalingMatrix, matRotationMatrix, transformationMat;

		worldMat = m_pDirect3D->GetWorldMatrix();
		viewMat = m_pCamera->GetViewMatrix();		
		projectionMat = m_pDirect3D->GetPerspectiveProjectionMatrix();
		
		D3DXVECTOR3 vCameraPosition = m_pCamera->GetPosition();
		
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//RENDER SKY : SET SKY At DISTANCE OF 250 FROM ORIGIN				
		m_pDirect3D->EnableBlendState2();		
		worldMat = m_pDirect3D->GetWorldMatrix();		
		D3DXMATRIX matRotBy90, matRotBy90OnZAxis;
		D3DXMatrixRotationX(&matRotBy90, (90 * D3DX_PI) / 180);
		D3DXMatrixRotationZ(&matRotBy90OnZAxis, (90 * D3DX_PI) / 180);
		D3DXMatrixTranslation(&matTranslationMatrix, 0.0f, m_pSky->GetRadius() - 515.0f, 0.0f);
		worldMat =  matTranslationMatrix * worldMat;
		m_pSky->Render(pDeviceContext);
		m_pSkyShader->RenderSkyShader(pDeviceContext, m_pSky->GetIndexCount(), worldMat, viewMat, projectionMat, m_pSky->GetTexture(), m_fFogStart, m_fFogEnd, vCameraPosition);
		

		//RENDER SUN		
		if (m_bSunLightEnable == true)
		{
			/*
			worldMat = m_pDirect3D->GetWorldMatrix();				
			
			D3DXVECTOR3 vSun = m_pSun->GetPosition();			
			D3DXMatrixTranslation(&matTranslationMatrix, vSun.x, vSun.y, vSun.z);

			D3DXMATRIX matRotBy90;
			D3DXMatrixRotationX(&matRotBy90, (90 * D3DX_PI) / 180);			
			worldMat = matRotBy90 * matTranslationMatrix * worldMat;
			m_pSun->RenderSun(pDeviceContext);
			m_pSkyShader->RenderSkyShader(pDeviceContext, m_pSun->GetIndexCount(), worldMat, viewMat, projectionMat, m_pSun->GetTextureSRView());
			*/
		}
		m_pDirect3D->DisableBlendState();
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		

		//RENDER CUBE  ON ORIGIN(0,0,0)
		worldMat = m_pDirect3D->GetWorldMatrix();		
		D3DXMatrixIdentity(&matRotationMatrix);
		D3DXMatrixRotationY(&matRotationMatrix, m_pCubeModel->GetHeading());		
		worldMat = matRotationMatrix*worldMat;

		m_pCubeModel->RenderCube(m_pDirect3D->GetDeviceContext());
		if (m_bSunLightEnable == false)
		{
			m_pCubeShader->RenderCubeShader(m_pDirect3D->GetDeviceContext(), m_pCubeModel->GetCubeIndexCount(), worldMat, viewMat, projectionMat, m_pCubeModel->GetTextureSRView());
		}
		else
		{			
			ID3D11ShaderResourceView* pTexture = m_pCubeModel->GetTextureSRView();
			/*if (vCameraPosition.y <= 0)
				pTexture = m_pScreenSurface->GetShaderResourceView();*/			
			D3DXVECTOR3 vSunDir = D3DXVECTOR3(0, 0, 0) - m_pSun->GetPosition();
			D3DXVec3Normalize(&vSunDir, &vSunDir);
			m_pLightSourceShader->RenderShader(pDeviceContext, m_pCubeModel->GetCubeIndexCount(), worldMat, viewMat, projectionMat,
				pTexture,
				m_pLightSource->GetDiffuseLight(), m_pLightSource->GetAmbientLight(), vSunDir, m_fFogStart, m_fFogEnd, vCameraPosition);
		}
		
		
		
		
		////////RENDER FLOOR	@(0.0, -5.0, 0.0)		
		////////CAMERA @(0.0, -3.0, 0.0)  
		//////worldMat = m_pDirect3D->GetWorldMatrix();			
		//////D3DXMatrixTranslation(&matTranslationMatrix, 0.0f, -5.0f, 0.0f);		
		///////*D3DXMatrixScaling(&matScalingMatrix, 10.0f, 0.0f, 10.0f);*/
		//////worldMat = /*matScalingMatrix **/ matTranslationMatrix * worldMat;

		//////m_pFloorModel->RenderFloor(pDeviceContext);		
		//////if (m_bSunLightEnable == false)
		//////{
		//////	//REFLECTION IF NO SUN LIGHT
		//////	reflectionMat = m_pCamera->GetViewMatrix(D3DXVECTOR3(0.1, -8.0, -1.0), D3DXVECTOR3(0, 1, 0));

		//////	m_pReflectionShader->RenderShader(pDeviceContext, m_pFloorModel->GetFloorIndexCount(), worldMat, viewMat, projectionMat,				
		//////		m_pFloorModel->GetTextureSRView(), 
		//////		m_pReflectionSurface->GetShaderResourceView(),
		//////		reflectionMat
		//////		);
		//////}
		//////else
		//////{
		//////	////THIS SHADER JUST RENDER TEXTURE ON SURFACE			
		//////	D3DXVECTOR3 vSunDir = D3DXVECTOR3(0, 0, 0) - m_pSun->GetPosition();
		//////	D3DXVec3Normalize(&vSunDir, &vSunDir);
		//////	sunViewMatrix = m_pCamera->GetViewMatrix(m_pSun->GetPosition(), vSunDir);
		//////	sunProjMatrix = m_pShadowMapSurface->GetOrthProjectionMatrix();

		//////	m_pShadowMapShader->RenderShadowMap(pDeviceContext, m_pFloorModel->GetFloorIndexCount(), worldMat, viewMat, projectionMat
		//////		, m_pShadowMapSurface->GetShaderResourceView(), m_pFloorModel->GetTextureSRView(), sunViewMatrix, sunProjMatrix
		//////		, m_pLightSource->GetAmbientLight(), m_pLightSource->GetDiffuseLight(), m_pSun->GetPosition());
		//////	
		//////}		
		



		if (m_bShowDepthMapEnable)
		{
			//DEPTH MAP MODEL
			worldMat = m_pDirect3D->GetWorldMatrix();			
			D3DXMatrixTranslation(&matTranslationMatrix, 0, 5, 10);			
			D3DXMatrixScaling(&matScalingMatrix, 5.0f, 1.0f, 5.0f);
			worldMat = matScalingMatrix * matTranslationMatrix * worldMat;
			viewMat = m_pCamera->GetViewMatrix();
			//orthoMat = m_pDirect3D->GetOrthogonalProjectionMatrix();
			projectionMat = m_pDirect3D->GetPerspectiveProjectionMatrix();
			m_pDepthMapModel->RenderDepthMapModel(pDeviceContext);
			m_pDepthMapModelShader->RenderDepthMapModelShader(pDeviceContext, m_pDepthMapModel->GetIndexCount(), worldMat, viewMat, /*orthoMat*/projectionMat, m_pScreenSurface->GetShaderResourceView());
			
		}
		



		//if (m_bSunLightEnable && m_bLightShaftEnable)
		//{
		//	//RENDER LIGHT SHAFT GRID
		//	//LIGHT SHAFT HAS 100x100 Points so shift x = -50 and z = -50 to set on origin		
		//	m_pDirect3D->SetDepthBufferOFF();
		//	m_pDirect3D->EnableBlendState();
		//	worldMat = m_pDirect3D->GetWorldMatrix();		
		//	//D3DXMatrixTranslation(&matTranslationMatrix, -50.0f, 50.0f, -50.0f);	//SKY Tip FROM ORIGIN(0,0,0) is 50
		//	D3DXMatrixTranslation(&matTranslationMatrix, -50.0f, -50.0f, -50.0f);	//SHIFTING BELOW 50
		//	worldMat = matTranslationMatrix*worldMat;

		//	m_pLightShaftGrid->RenderLightShaftGrid(pDeviceContext);
		//	m_pLightShaftShader->RenderShader(pDeviceContext, m_pLightShaftGrid->GetIndexCount(), worldMat, viewMat, projectionMat, m_pRender2Texture->GetShaderResourceView());

		//	if (m_fTickCount > 1)
		//	{
		//		m_fTickCount = 0;
		//		m_bRenderState = (m_bRenderState == false) ? true : false;
		//	}


		//	/*if (m_bRenderState)
		//		m_pLightShaftShader->RenderShader(pDeviceContext, m_pLightShaftGrid->GetIndexCount(), worldMat, viewMat, projectionMat, m_pLightShaftGrid->GetTextureSRView());
		//		else
		//		m_pLightShaftShader->RenderShader(pDeviceContext, m_pLightShaftGrid->GetIndexCount(), worldMat, viewMat, projectionMat);*/
		//	m_pDirect3D->DisableBlendState();
		//	m_pDirect3D->SetDepthBufferON();
		//}
		
		D3DXVECTOR3 vSunDir = D3DXVECTOR3(0, 0, 0) - m_pSun->GetPosition();
		D3DXVec3Normalize(&vSunDir, &vSunDir);
		
		//RENDER WATER
		worldMat = m_pDirect3D->GetWorldMatrix();
		D3DXMatrixTranslation(&matTranslationMatrix, -75.0f * 2.5f, 0.0f, -75.0f* 2.5f);
		worldMat = matTranslationMatrix * worldMat;
		viewMat = m_pCamera->GetViewMatrix();
		m_pOceanWater->Render(pDeviceContext);
		m_pOceanShader->RenderOceanShader(pDeviceContext, m_pOceanWater->GetIndexCount(), worldMat, viewMat, projectionMat, m_pOceanWater->GetTexture(), m_pOceanWater->GetTime(), m_pLightSource->GetAmbientLight(), vSunDir, m_fFogStart, m_fFogEnd, vCameraPosition);



		if (0)
		{
			//RENDER PARTICLES
			m_pDirect3D->EnableBlendState2();
			worldMat = m_pDirect3D->GetWorldMatrix();
			D3DXMatrixTranslation(&matTranslationMatrix, 0.0f, 50.0f, -5.0f);	//Tip OF SKY AT HEIGHT 50
			worldMat = matTranslationMatrix * worldMat;
			m_pParticleSystem->Render(pDeviceContext);
			m_pParticleShader->RenderParticleShader(pDeviceContext, m_pParticleSystem->GetIndexCount(), worldMat, viewMat, projectionMat, m_pParticleSystem->GetTexture());
		}


		/*
		//RENDER DEPTH BUFFER	
		m_pDirect3D->EnableBlendState();
		worldMat = m_pDirect3D->GetWorldMatrix();		
		D3DXMatrixIdentity(&matTranslationMatrix); 
		D3DXMatrixIdentity(&matScalingMatrix);		
		D3DXMatrixTranslation(&matTranslationMatrix, 0.0f, 5.0f, 0.0f);
		D3DXMatrixScaling(&matScalingMatrix, 10.0f, 1.0f, 10.0f);
		worldMat = matScalingMatrix * matTranslationMatrix * worldMat;
		m_pDepthModel->RenderDepth(pDeviceContext);
		m_pDepthShader->RenderShader(pDeviceContext, m_pDepthModel->GetIndexCount(), worldMat, viewMat, projectionMat);
		m_pDirect3D->DisableBlendState();
		*/

		
		if (vCameraPosition.y > 0 && 0)
		{
			m_pDirect3D->EnableBlendState();
			worldMat = m_pDirect3D->GetWorldMatrix();
			D3DXVECTOR3 vCamDir = m_pCamera->GetDirection();
			D3DXVec3Normalize(&vCamDir, &vCamDir);
			D3DXVECTOR3 vNewArrowPos = vCameraPosition + (vCamDir * 4.0);
			D3DXVECTOR3 vCubePostion = D3DXVECTOR3(0, vCameraPosition.y, 0);		 //Cube is Fixed at origin	at Camera Level
			D3DXVECTOR3 vDir2Cube = vCubePostion - vNewArrowPos;
			D3DXVec3Normalize(&vDir2Cube, &vDir2Cube);
			D3DXVECTOR3 vZeeDir = D3DXVECTOR3(0, 0, 1);
			float fArrowHeading = acos(D3DXVec3Dot(&vZeeDir, &vDir2Cube)); //Angle bw Dir2Cube and CameraDir
			
			D3DXVECTOR3 vRotAxis;
			D3DXVec3Cross(&vRotAxis, &vZeeDir, &vDir2Cube);
			D3DXVec3Normalize(&vRotAxis, &vRotAxis);
			//fArrowHeading = atan2(vDir2Cube.x, vDir2Cube.z);

			
			D3DXMatrixTranslation(&matTranslationMatrix, vNewArrowPos.x, vNewArrowPos.y - 0.5, vNewArrowPos.z); //Adding in Y  so that little higher from camera level			
			D3DXMatrixRotationAxis(&matRotationMatrix, &vRotAxis, fArrowHeading);			
			transformationMat = matRotationMatrix * matTranslationMatrix * worldMat;
			m_pArrowMesh->RenderArrowModel(pDeviceContext);
			m_pArrowShader->RenderArrowShader(pDeviceContext, m_pArrowMesh->GetIndexCount(), transformationMat, viewMat, projectionMat, m_pArrowMesh->GetTextureSRView());
			m_pDirect3D->DisableBlendState();
		}

		//RENDER TEXT
		/*m_pDirect3D->SetDepthBufferOFF();
		m_pDirect3D->EnableBlendState();
		worldMat = m_pDirect3D->GetWorldMatrix();
		orthoMat = m_pDirect3D->GetOrthogonalProjectionMatrix();

		char strCameraPos[100];
		D3DXVECTOR3 vPos = m_pCamera->GetPosition(), vSunPos = m_pSun->GetPosition();
		sprintf_s(strCameraPos, "CameraPos = (%.0f,%0.0f,%0.0f)", vPos.x, vPos.y, vPos.z);
		m_pText->Render(pDeviceContext, strCameraPos, worldMat, orthoMat);

		m_pDirect3D->DisableBlendState();
		m_pDirect3D->SetDepthBufferON();*/
		
		m_pDirect3D->EndScene();
	}

}

void Dx11_Graphics::RenderWaterEffectsOnScreen(float _fTick)
{
	ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();

	m_pScreenSurface->SetRenderTarget(pDeviceContext);	
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	//BLACK
	m_pScreenSurface->BeginScene(pDeviceContext, clearColor);

	
	D3DXMATRIX viewMat, worldMat, projectionMat;
	D3DXMATRIX matTranslationMatrix, matScalingMatrix, matRotationMatrix;

	worldMat = m_pDirect3D->GetWorldMatrix();	
	D3DXVECTOR3 vCamPos = m_pCamera->GetPosition();
	D3DXVECTOR3 vCamDir = m_pCamera->GetDirection();
	vCamPos.y = 1.0f;													//Placing Camera just above the water 
	viewMat = m_pCamera->GetViewMatrix(vCamPos.x, vCamPos.y, vCamPos.z, vCamDir.x, vCamDir.y, vCamDir.z);
	projectionMat = m_pScreenSurface->GetProjectionMatrix();// m_pDirect3D->GetPerspectiveProjectionMatrix();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//RENDER SKY : SET SKY At DISTANCE OF 250 FROM ORIGIN					
	worldMat = m_pDirect3D->GetWorldMatrix();	
	D3DXMatrixTranslation(&matTranslationMatrix, 0.0f, m_pSky->GetRadius() - 515.0f, 0.0f);
	worldMat = matTranslationMatrix * worldMat;
	m_pSky->Render(pDeviceContext);
	m_pSkyShader->RenderSkyShader(pDeviceContext, m_pSky->GetIndexCount(), worldMat, viewMat, projectionMat, m_pSky->GetTexture(), m_fFogStart, m_fFogEnd, vCamPos);


	//RENDER CUBE  ON ORIGIN(0,0,0)
	worldMat = m_pDirect3D->GetWorldMatrix();
	D3DXMatrixIdentity(&matRotationMatrix);
	D3DXMatrixRotationY(&matRotationMatrix, m_pCubeModel->GetHeading());
	worldMat = matRotationMatrix*worldMat;	
	D3DXVECTOR3 vSunDir = D3DXVECTOR3(0, 0, 0) - m_pSun->GetPosition();
	m_pCubeModel->RenderCube(m_pDirect3D->GetDeviceContext());
	D3DXVec3Normalize(&vSunDir, &vSunDir);
	m_pLightSourceShader->RenderShader(pDeviceContext, m_pCubeModel->GetCubeIndexCount(), worldMat, viewMat, projectionMat,
		m_pCubeModel->GetTextureSRView(),
		m_pLightSource->GetDiffuseLight(), m_pLightSource->GetAmbientLight(), vSunDir, m_fFogStart, m_fFogEnd, vCamPos);

	
	//RENDER WATER
	worldMat = m_pDirect3D->GetWorldMatrix();
	D3DXMatrixTranslation(&matTranslationMatrix, -75.0f * 2.5f, 0.0f, -75.0f* 2.5f);
	worldMat = matTranslationMatrix * worldMat;	
	m_pOceanWater->Render(pDeviceContext);
	m_pOceanShader->RenderOceanShader(pDeviceContext, m_pOceanWater->GetIndexCount(), worldMat, viewMat, projectionMat, m_pOceanWater->GetTexture(), m_pOceanWater->GetTime(), m_pLightSource->GetAmbientLight(), vSunDir, m_fFogStart, m_fFogEnd, vCamPos);

	m_pDirect3D->SetBackBufferRenderTarget();

	//RENDER ON SCREEN
	if (m_pDirect3D->BeginScene())
	{
		ID3D11Device *pDevice = m_pDirect3D->GetDevice();		
		D3DXVECTOR3 vCameraPosition = m_pCamera->GetPosition();
		
		worldMat = m_pDirect3D->GetWorldMatrix();				
		viewMat = m_pCamera->GetViewMatrix();
		projectionMat = m_pDirect3D->GetPerspectiveProjectionMatrix();
		m_pOnScreenRendering->RenderScreenSpace(pDeviceContext, m_pScreenSurface->GetShaderResourceView(), worldMat, viewMat, projectionMat, vCameraPosition, _fTick);
		

		m_pDirect3D->EndScene();
	}
}


#ifdef _FI_SIMLIGHTEX_
void Dx11_Graphics::AddLight(LGT_LITCHR TypeOfLight, LGT_COLOR ColorOfLight)
{

	FI_LightSector *pSector = NULL;
	FI_SectorPulse *pPulse = NULL;

	FI_SimLightEx *pLight = new FI_SimLightEx(NULL);

	pLight->SetLightState(true);
	pLight->AddSector();
	pSector = pLight->m_vSectors[0];

	if (pSector)
	{
		char pCol[256] = "", pLgtChr[256] = "";	float fSigPer = 1.0f;

		pSector->SetSectorRange(5);
		pSector->SetSectorAngle(0, 360);

		_itoa_s(ColorOfLight, pCol, 10);
		_itoa_s(TypeOfLight, pLgtChr, 10);

		pSector->AddPulse(pCol, pLgtChr, fSigPer);
	}

	m_vLights.push_back(pLight);
}
#endif

void Dx11_Graphics::UpdateCamera(char action)
{
	m_pCamera->UpdateCamera(action);
}

void Dx11_Graphics::SunLightOnOff()
{
	m_bSunLightEnable = (m_bSunLightEnable) ? false: true;
}

void Dx11_Graphics::LightShaftOnOff()
{	
	m_bLightShaftEnable = (m_bLightShaftEnable) ? false : true;
}
void Dx11_Graphics::ChangeSunDirection(char action)
{
	D3DXVECTOR3 vSunPos = m_pSun->GetPosition();
	D3DXVECTOR3 vSunRot = m_pSun->GetRotation();
	float fAngle = (5 * D3DX_PI) / 180;

	D3DXVECTOR3 newPos;
	if (action == 'M' || action == 'N')
	{
		if (action == 'M')
		{
			fAngle = fAngle * 1;
			vSunRot.z += fAngle;	//ROTATION IN XY PLANE : 
		}
		else if (action == 'N')
		{
			fAngle = fAngle * (-1);
			vSunRot.z += fAngle;	//ROTATION IN XY PLANE : 
		}
		
		newPos.x = vSunPos.x*cosf(fAngle) - vSunPos.y*sinf(fAngle);
		newPos.y = vSunPos.x*sinf(fAngle) + vSunPos.y*cosf(fAngle);
		newPos.z = vSunPos.z;
	}
	else if(action == 'H' || action == 'J')
	{
		if (action == 'H')
		{
			fAngle = fAngle * 1;			
		}
		else if (action == 'J')
		{
			fAngle = fAngle * (-1);			
		}

		newPos.x = vSunPos.x*cosf(fAngle) - vSunPos.z*sinf(fAngle);		
		newPos.z = vSunPos.x*sinf(fAngle) + vSunPos.z*cosf(fAngle);
		newPos.y = vSunPos.y;

	}


	m_pSun->SetPosition(newPos);
	m_pSun->SetRotation(D3DXVECTOR3(vSunRot.x, vSunRot.y, vSunRot.z));

}


void Dx11_Graphics::ShowWireFrame()
{	
	if (m_pDirect3D)
	{
		if (m_bIsWireFrameEnable == false)
			m_pDirect3D->SetRasterWireframe();
		else
			m_pDirect3D->SetRasterSolid();
		
		m_bIsWireFrameEnable = (m_bIsWireFrameEnable == false) ? true : false;		
	}
}

void Dx11_Graphics::ShowDepthMap()
{
	m_bShowDepthMapEnable = (m_bShowDepthMapEnable == false) ? true : false;
}

void Dx11_Graphics::SetFogParameter()
{
	m_fFogStart = 0.0f;
	m_fFogEnd += 10.0f;

	if (GetAsyncKeyState(VK_SHIFT))
	{
		//Reset FOG Parameter
		//m_fFogEnd  = m_fFogStart = 0.0f;		
	}
}

