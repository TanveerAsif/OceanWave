#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include "Dx11_Texture.h"

struct st_SKY_VERTEX
{
	D3DXVECTOR3 pos;
	D3DXCOLOR diffuse;
	D3DXVECTOR2 tex;

};


class Dx11_Sky
{
	ID3D11Buffer	*m_pVertexBuffer, *m_pIndexBuffer;
	int              m_nVertexCount, m_nIndexCount;

	Dx11_Texture	*m_pTexture;

	int				 m_nResTheta, m_nResPhi;
	int				 m_nRadius;
public:
	Dx11_Sky();
	~Dx11_Sky();

	bool InitSky(ID3D11Device *pDevice, WCHAR *pTextureCloud);
	void Render(ID3D11DeviceContext *pDeviceContext);
	void Process(D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraFrwdDir);
	void Shutdown();
	int  GetIndexCount(){ return m_nIndexCount; }
	ID3D11ShaderResourceView *GetTexture(){ return m_pTexture->GetTexture(); }


	int GetRadius(){ return m_nRadius; }
	void SetRadius(int radValue){ m_nRadius = radValue; }
};

