#include "Dx11_OnScreenRendering.h"
#include <fstream>
#include <D3DX11async.h>
#include "Dx11_Shaders.h"  //TO USE STRUCTURE DEF

float g_delta = 0.0f;
Dx11_OnScreenRendering::Dx11_OnScreenRendering()
{
	//m_pWaterDropTex = new Dx11_Texture();
	//m_pDisplacementMapTex = new Dx11_Texture();
	m_bWaterToAir = false;
}


Dx11_OnScreenRendering::~Dx11_OnScreenRendering()
{
}

bool Dx11_OnScreenRendering::initScreenSpace(ID3D11Device * pDevice)
{
	m_nVertexCount = 4;
	m_nIndexCount = 6;

	D3DXVECTOR3 pos;
	D3DXCOLOR diffuse;
	D3DXVECTOR2 tex;

	stSCREEN_VERTEX *pScreenSpace = new stSCREEN_VERTEX[m_nVertexCount];
	pScreenSpace[0] = { D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) };
	pScreenSpace[1] = { D3DXVECTOR3(1.0f, 1.0f, 0.0f),D3DXVECTOR2(1.0f, 0.0f) };
	pScreenSpace[2] = { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f) };
	pScreenSpace[3] = { D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f) };

	unsigned int *pIndices = new unsigned int[m_nIndexCount];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 2;
	pIndices[4] = 1;
	pIndices[5] = 3;



	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stSCREEN_VERTEX)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pScreenSpace;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	delete[] pScreenSpace;
	delete[] pIndices;
	pScreenSpace = 0;
	pIndices = 0;


	//INITShader
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer, *pPSBuffer_Water2Air;

	hr = D3DX11CompileFromFile(L"Data\\MyOnScreenShader.hlsl", NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "Data/ShaderCompileError.txt", "w");
		if (fp)
		{
			for (int i = 0; i < bufferSize; i++)
			{
				fwrite(&pCompileError[i], sizeof(pCompileError[i]), 1, fp);
			}
			fclose(fp);
		}
		return false;
	}

	hr = D3DX11CompileFromFile(L"Data\\MyOnScreenShader.hlsl", NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = D3DX11CompileFromFile(L"Data\\MyOnScreenShader.hlsl", NULL, NULL, "PS_WaterToAir", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer_Water2Air, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer_Water2Air->GetBufferPointer(), pPSBuffer_Water2Air->GetBufferSize(), NULL, &m_pPS_WaterToAir);
	if (hr != S_OK)
		return false;

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};	
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SET IN EACH RENDER : SHIFTED IN RENDER FUNCTION
	/*pDeviceContext->IASetInputLayout(m_pInputLayout);*/		

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	//TickBuffer
	D3D11_BUFFER_DESC tickBufferDesc;
	tickBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tickBufferDesc.ByteWidth = sizeof(stOceanTimeBufferType);
	tickBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tickBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tickBufferDesc.MiscFlags = 0;
	tickBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&tickBufferDesc, NULL, &m_pTickBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_NEVER; //
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;

	Dx11_Texture *pTexWaterDrop = new Dx11_Texture();
	if (pTexWaterDrop->Initiazlize(pDevice, L"Data/UnderwaterPP/WaterDroplets.dds"))
	{
		m_pWaterDropTex = pTexWaterDrop->GetTexture();
	}	

	Dx11_Texture *pTexWaterDisplacementMap = new Dx11_Texture();
	if (pTexWaterDisplacementMap->Initiazlize(pDevice, L"Data/UnderwaterPP/underwater_disp_map.dds"))
	{
		m_pDisplacementMapTex = pTexWaterDisplacementMap->GetTexture();
	}

	Dx11_Texture *pTexWater2AirMap = new Dx11_Texture();
	if (pTexWater2AirMap->Initiazlize(pDevice, L"Data/UnderwaterPP/waterair.dds"))
	{
		m_pWater2AirTex = pTexWater2AirMap->GetTexture();
	}
	
	
	
	return true;
}

void Dx11_OnScreenRendering::SetWaterToAirState(bool bState)
{
	m_bWaterToAir = bState;
}


void Dx11_OnScreenRendering::Process(float _fTick)
{
	m_fTick += _fTick;

}

void Dx11_OnScreenRendering::RenderScreenSpace(ID3D11DeviceContext * pDeviceContext, ID3D11ShaderResourceView *pScreenTexture,
															D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat,
															D3DXVECTOR3 vCameraPosition, float fTick)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stSCREEN_VERTEX);
		UINT offset = 0;
		pDeviceContext->IASetInputLayout(m_pInputLayout);		

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);		
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		

		D3DXMatrixTranspose(&worldMat, &worldMat);
		D3DXMatrixTranspose(&viewMat, &viewMat);
		D3DXMatrixTranspose(&projectionMat, &projectionMat);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return;
		stMatrixBufferType *dataPtr;
		dataPtr = (stMatrixBufferType*)mappedResource.pData;
		dataPtr->world = worldMat;
		dataPtr->view = viewMat;
		dataPtr->projection = projectionMat;
		dataPtr->vCameraPosition = vCameraPosition;
		pDeviceContext->Unmap(m_pMatrixBuffer, 0);

		hr = pDeviceContext->Map(m_pTickBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return;
		stOceanTimeBufferType *dataTimePtr;
		dataTimePtr = (stOceanTimeBufferType *)mappedResource.pData;
		dataTimePtr->fTime = m_fTick;
		g_delta += 0.001f;
		if (g_delta > 1.0f)
			g_delta = 0.0f;
		dataTimePtr->fDelta = g_delta;
		pDeviceContext->Unmap(m_pTickBuffer, 0);

		//ID3D11ShaderResourceView *pWaterDropTex = m_pWaterDropTex->GetTexture();

		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);		
		pDeviceContext->PSSetShaderResources(0, 1, &pScreenTexture);
		pDeviceContext->PSSetShaderResources(1, 1, &m_pWaterDropTex);
		pDeviceContext->PSSetShaderResources(2, 1, &m_pDisplacementMapTex);
		pDeviceContext->PSSetConstantBuffers(1, 1, &m_pTickBuffer);
		

		pDeviceContext->VSSetShader(m_pVS, NULL, 0);
		if (m_bWaterToAir)
		{
			pDeviceContext->PSSetShader(m_pPS_WaterToAir, NULL, 0);
			pDeviceContext->PSSetShaderResources(3, 1, &m_pWater2AirTex);
		}
		else
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
		
		pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
		
		pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}
}
