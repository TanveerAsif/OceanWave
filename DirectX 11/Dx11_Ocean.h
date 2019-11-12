#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include "Dx11_Texture.h"

struct  stOceanVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 col;
	D3DXVECTOR3 normal;		//ADD NORMAL FOR SUN LIGHT EFFECT 
};


class Dx11_Ocean
{
	Dx11_Texture	*m_pTexture;
	ID3D11Buffer	*m_pVB, *m_pIB;
	int				m_nIndexCount;

	float			m_fTime;
public:
	Dx11_Ocean();
	~Dx11_Ocean();

	bool InitializeOcean(ID3D11Device *pDevice, WCHAR *pTextureFile);
	void Shutdown();
	void Process(float _fTick);
	void Render(ID3D11DeviceContext *pDeviceContext);
	int GetIndexCount(){ return m_nIndexCount; }

	ID3D11ShaderResourceView* GetTexture(){ return m_pTexture->GetTexture(); }
	float GetTime(){ return m_fTime; }
};

