#pragma once
#include "Dx11_Direct3D.h"
#include "Dx11_Camera.h"
#include "Dx11_Model.h"
#include "Dx11_Shaders.h"
#include "Dx11_Render2Texture.h"
#include "FI_SimLightEx.h"
#include "Dx11_Font.h"
#include "Dx11_Sky.h"
#include "Dx11_LightSource.h"
#include "Dx11_ParticleSystem.h"
#include "Dx11_Ocean.h"


class Dx11_Graphics
{


private:
	Dx11_Direct3D   *m_pDirect3D;
	Dx11_Camera		*m_pCamera;
	



	Dx11_Model		*m_pModel, *m_pFloorModel, *m_pCubeModel, *m_pDepthModel, *m_pLightShaftGrid, *m_pSun, *m_pDepthMapModel;
	Dx11_Shaders	*m_pShader, *m_pReflectionShader, *m_pCubeShader, *m_pDepthShader, *m_pLightShaftShader, *m_pShadowMapShader, *m_pDepthMapShader, *m_pDepthMapModelShader;
	
	Dx11_LightSource	*m_pLightSource;
	Dx11_Shaders		*m_pLightSourceShader;

	Dx11_Render2Texture /**m_pRender2Texture,*/ *m_pReflectionSurface, *m_pShadowMapSurface;
	

	std::vector<FI_SimLightEx *>m_vLights;
	UINT m_nScreenWidth, m_nScreenHeight;

	float m_fTickCount;
	bool m_bRenderState;
	
	Dx11_Text      *m_pText;
	
	Dx11_Sky		*m_pSky;
	Dx11_Shaders	*m_pSkyShader;


	Dx11_ParticleSystem		*m_pParticleSystem;
	Dx11_Shaders			*m_pParticleShader;

	bool					m_bIsWireFrameEnable;
	bool					m_bSunLightEnable;
	bool					m_bLightShaftEnable;
	bool					m_bShowDepthMapEnable;

	Dx11_Ocean		*m_pOceanWater;
	Dx11_Shaders	*m_pOceanShader;

private:

	void RenderShadowMapOfSunLight(float _fTick);
	void RenderReflectionSurface(float _fTick);
	void RenderToTexture(float _fTick);
	void RenderScene(float _fTick);

	void AddLight(LGT_LITCHR TypeOfLight, LGT_COLOR ColorOfLight);

public:
	Dx11_Graphics();
	~Dx11_Graphics();
	bool Initialize(HWND hWnd);
	void Process(float _fTick);
	void Render(float _fTick);

	void Shutdown();
	void UpdateCamera(char action);
	void ChangeSunDirection(char action);
	void SunLightOnOff();
	void LightShaftOnOff();
	void ShowWireFrame();

	void ShowDepthMap();

};

