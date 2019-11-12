#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include "Dx11_Texture.h"


struct stParticleType
{
	D3DXVECTOR3 vPosition;
	D3DXCOLOR   color;
	float		velocity;
	bool		life;
};

struct  stPARTICLE_VERTEX
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR2 uv;
	D3DXVECTOR4 diffuse;
};

class Dx11_ParticleSystem
{
	
	D3DXVECTOR3 m_vDeviation;
	float       m_fParticleVelocity, m_fParticleVelocityVariation;
	float		m_fParticleSize;
	float		m_fParticleEmitionRate;	
	
	unsigned int		m_nMaxParticles;
	unsigned int		m_nCurrentParticleCount;

	float				m_fTimeDuration;

	stParticleType	*m_pParticleList;
	Dx11_Texture	*m_pTexture;

	ID3D11Buffer	*m_pVB, *m_pIB;
	unsigned int	m_nVertexCount, m_nIndexCount;

	stPARTICLE_VERTEX *m_pVertices;

private:
	bool InitializeBuffer(ID3D11Device	*pDevice);
	void EmitParticles(float fTick);
	void UpdateParticles(float fTick);
	void KillParticles();

	void UpdateBuffer(ID3D11DeviceContext *pDeviceContext);

public:
	Dx11_ParticleSystem();
	~Dx11_ParticleSystem();

	bool Inittialize(ID3D11Device	*pDevice, WCHAR *pTextureFile);
	void Shutdown();

	void Prcoess(ID3D11DeviceContext *pDeviceContext, float _fTick);
	void Render(ID3D11DeviceContext *pDeviceContext);

	ID3D11ShaderResourceView *GetTexture(){ return m_pTexture->GetTexture(); };
	int GetIndexCount(){ return m_nIndexCount; }
};

