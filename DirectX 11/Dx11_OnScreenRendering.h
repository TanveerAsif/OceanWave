#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include "Dx11_Texture.h"

struct stSCREEN_VERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};



class Dx11_OnScreenRendering
{
	ID3D11Buffer	*m_pVertexBuffer, *m_pIndexBuffer;
	int              m_nVertexCount, m_nIndexCount;

	ID3D11VertexShader *m_pVS = NULL;
	ID3D11PixelShader  *m_pPS = NULL, *m_pPS_WaterToAir = NULL;

	ID3D11InputLayout  *m_pInputLayout = NULL;
	ID3D11Buffer       *m_pMatrixBuffer = NULL, *m_pTickBuffer = NULL;


	ID3D11SamplerState	*m_pSamplerState = NULL;

	/*Dx11_Texture*/ID3D11ShaderResourceView	*m_pWaterDropTex = NULL, *m_pDisplacementMapTex = NULL, *m_pWater2AirTex = NULL;


	float				m_fTick = 0.0f;
	bool				m_bWaterToAir = false;
public:
	Dx11_OnScreenRendering();
	~Dx11_OnScreenRendering();

	bool initScreenSpace(ID3D11Device *pDevice);
	void SetWaterToAirState(bool bState);
	void Process(float _fTick);
	void RenderScreenSpace(ID3D11DeviceContext *pDeviceContext, ID3D11ShaderResourceView *pTexture, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat,D3DXVECTOR3 vCameraPosition, float fTick);
};

