#pragma once
#include <D3D11.h>
#include <D3DX10math.h>





class Dx11_LightSource
{
	D3DXVECTOR3 m_vLightDir;
	//D3DXVECTOR3 m_vLightPos;			//LIGHT Dir CAN BE COMPUTE BY ITS POSITION(Dir = Obj.Pos - Light.Pos)
	
	
	//D3DXCOLOR   m_Color;				//IN SHADER COMPATIBLITY ISSUE WITH D3DXCOLOR SO CHANGE TO D3DXVECTOR4
	//D3DXVECTOR4	  m_vColor;			//Replace by Ambient and Diffuse Light

	D3DXVECTOR4   m_ambientColor, m_diffuseColor;

public:
	Dx11_LightSource();
	~Dx11_LightSource();

	
	D3DXVECTOR3		GetLightDir(){ return m_vLightDir; }
	void SetDirection(D3DXVECTOR3 dir){ m_vLightDir = dir; }

	/*D3DXVECTOR3		GetLightPos(){ return m_vLightPos; }
	void SetLightPos(D3DXVECTOR3 pos){ m_vLightPos = pos; }*/


	void SetDiffuseLight(D3DXVECTOR4 vCol){ m_diffuseColor = vCol; }
	void SetAmbientLight(D3DXVECTOR4 vCol){ m_ambientColor = vCol; }
	D3DXVECTOR4		GetDiffuseLight(){ return m_diffuseColor; }
	D3DXVECTOR4		GetAmbientLight(){ return m_ambientColor; }
};

