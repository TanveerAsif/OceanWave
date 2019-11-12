#pragma once
#include <D3D11.h>
#include <D3DX11tex.h>

class Dx11_Texture
{

private:
	ID3D11ShaderResourceView *m_pShaderResourceView;

public:
	Dx11_Texture();
	~Dx11_Texture();

	bool Initiazlize(ID3D11Device *pDevice, WCHAR *pTextureFile);
	void Shutdown();

	ID3D11ShaderResourceView *GetTexture(){ return m_pShaderResourceView; };
};

