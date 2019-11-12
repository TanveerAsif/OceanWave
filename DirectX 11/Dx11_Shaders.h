#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>


struct stMatrixBufferType
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};


struct stOceanTimeBufferType
{
	float fTime;
	float padding[3];							//TO MAKE MULTIPLE OF 16
};

struct stMatrixBufferShadowMap
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
	D3DXMATRIX lightView;
	D3DXMATRIX lightProjection;
};

//FOR TRANSPARENCY OF TEXTURE
//struct stTransparentBufferType
//{
//	float blendAmount;
//	D3DXVECTOR3 padding;
//};


struct  stLightViewMatrixBufferType
{
	//FOR SHADOW MAP
	D3DXMATRIX matLightViewMatrix;
};


struct  stReflectionBufferType
{
	D3DXMATRIX matReflectionMatrix;
};

struct  stFontColor
{
	//D3DXCOLOR clr;
	D3DXVECTOR4 clr;
};

struct stLightBuffer
{
	D3DXVECTOR4 vLightAmbientColor;				//Ambient Light;
	D3DXVECTOR4 vLightDiffuseColor;				//D3DXCOLOR   d3dLightColor;
	D3DXVECTOR3 vLightDir;	
	float       fPaddingValue;					//TO MAKE MULTIPLE OF 16

};


struct stLightBufferColor
{
	D3DXVECTOR4 vLightAmbientColor;				//Ambient Light;
	D3DXVECTOR4 vLightDiffuseColor;				//D3DXCOLOR   d3dLightColor;	
};


struct stLightBufferPos
{
	D3DXVECTOR3 vLightPos;
	float       fPaddingValue;					//TO MAKE MULTIPLE OF 16
};

struct stRandomValueLightShaft
{
	float randVal;
	float padding[3];							//TO MAKE MULTIPLE OF 16
};



class Dx11_Shaders
{

private:
	ID3D11VertexShader *m_pVS;
	ID3D11PixelShader  *m_pPS;

	ID3D11InputLayout  *m_pInputLayout;
	ID3D11Buffer       *m_pMatrixBuffer;


	ID3D11SamplerState	*m_pSamplerState, *m_pSamplerStateWarp, *m_pSamplerStateClamp;
	ID3D11Buffer		*m_pTransparency;

	ID3D11Buffer        *m_pRefMatrixBuffer;

	ID3D11Buffer		*m_pFontColorBuffer;
	ID3D11Buffer		*m_pLightBuffer, *m_pLightColorBuffer, *m_pLightPosBuffer;

	ID3D11Buffer		*m_pRandomLightShaft;

	ID3D11Buffer        *m_pLightViewMatrixBuffer;//SHADOW MAP

	ID3D11Buffer		*m_pOceanTickBuffer, *m_pSunInfoBuffer;
public:
	Dx11_Shaders();
	~Dx11_Shaders();


	bool InitializeLightShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *ShaderFilename);
	bool InitializeFloorShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *ShaderFilename);
	bool InitializeReflectionShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *ShaderFilename);
	bool InitializeFontShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *ShaderFilename);
	bool InitializeDepthBufferShader(ID3D11Device *pDevice, WCHAR *ShaderFilename);
	bool InitializeSkyShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeLightSourceShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeLightShaftShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeParticleSystemShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeShadowMapShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeDepthMapShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);
	bool InitializeOceanShader(ID3D11Device *pDevice, WCHAR *pShaderFilename);


	void Shutdown();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV);
	bool SetSkyShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV);
	bool SetCubeShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV);
	bool SetDepthMapModelShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV);
	bool SetShadowMapParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pDepthTextureRV, ID3D11ShaderResourceView *pTextureRV, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
		D3DXVECTOR4 ambientLight, D3DXVECTOR4 diffuseLight, D3DXVECTOR3 dirLight);
	bool SetParticleShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV, D3DXVECTOR4 color);
	
	//LIGHT SHADER
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV, D3DXVECTOR4 diffuseLight, D3DXVECTOR4 ambientLight, D3DXVECTOR3 dir);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX
							, ID3D11ShaderResourceView *pTextureRV
							, ID3D11ShaderResourceView *pReflectionTextureRV	
							, D3DXMATRIX refViewMatrix);
	bool SetOceanShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *pTextureRV, float _time, D3DXVECTOR4 Sun_AmbientLight, D3DXVECTOR3 sunDir);
	
	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat);
	void RenderSkyShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV);
	void RenderCubeShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV);
	void RenderDepthMapModelShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV);
	void RenderParticleShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV);
	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV, D3DXVECTOR4 color);
	
	//LIGHT SOURCE EFFECT
	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, 
					  ID3D11ShaderResourceView *pTextureRV,
					  D3DXVECTOR4 diffuseLight, D3DXVECTOR4 ambientLight, D3DXVECTOR3 dir);

	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTexture, ID3D11ShaderResourceView *pReflectionTexture, D3DXMATRIX reflectionMatrix);

	//SHADOW MAP 
	void RenderShadowMap(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pDepthTextureRV, ID3D11ShaderResourceView *pTextureRV, D3DXMATRIX sunViewMat, D3DXMATRIX sunProjectionMat,
		D3DXVECTOR4 ambientLight, D3DXVECTOR4 diffuseLight, D3DXVECTOR3 dirLight);
	void RenderDepthMap(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat);
	void RenderOceanShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV, float _time, D3DXVECTOR4 Sun_AmbientLight, D3DXVECTOR3 sunDir);

};

