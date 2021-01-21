#include "Dx11_Camera.h"


Dx11_Camera::Dx11_Camera()
{

	//m_fPosX = m_fPosY = m_fPosZ = 0.0f;	
	m_fYaw = m_fPitch = m_fRoll = 0.0f;
	//m_fPitch = -1.0f;

	m_vUp = D3DXVECTOR3(0, 1, 0);
	//m_vPos = D3DXVECTOR3(0, 5, -30);
	//m_vPos = D3DXVECTOR3(-1, 2, -14);
	m_vPos = D3DXVECTOR3(0, 2, -10);
	m_vFrwdDir = D3DXVECTOR3(0, 0, 1);
	
	D3DXVECTOR3 vDir;
	vDir.x = cosf(m_fPitch)*sinf(m_fYaw);
	vDir.y = sinf(m_fPitch);
	vDir.z = cosf(m_fPitch)*cosf(m_fYaw);

	D3DXVECTOR2 vFrwdDir = D3DXVECTOR2(vDir.x, vDir.z);
	D3DXVec2Normalize(&vFrwdDir, &vFrwdDir);
	m_vFrwdDir = D3DXVECTOR3(vFrwdDir.x, 0.0f, vFrwdDir.y);


	D3DXVECTOR3 vTarget;
	D3DXVec3Add(&vTarget, &m_vPos, &m_vFrwdDir);
	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &vTarget, &m_vUp);


}


Dx11_Camera::~Dx11_Camera()
{
}



//void Dx11_Camera::Process()
//{
//1
//	
//	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
//	
//
//
//	D3DXMATRIX rotMatrix;
//	D3DXMatrixRotationYawPitchRoll(&rotMatrix, m_fYaw, m_fPitch, m_fRoll);
//	D3DXVec3TransformCoord(&up, &up, &rotMatrix);
//	D3DXVec3TransformCoord(&m_vFrwdDir, &m_vFrwdDir, &rotMatrix);
//
//	/*D3DXVECTOR3 Frwd;
//	Frwd.x = cosf(m_fPitch)*sinf(m_fYaw);
//	Frwd.y = sinf(m_fPitch);
//	Frwd.z = cosf(m_fPitch)*cosf(m_fYaw);
//
//	D3DXVECTOR2 tempFrwd = D3DXVECTOR2(Frwd.x, Frwd.z);
//	D3DXVec2Normalize(&tempFrwd, &tempFrwd);
//	m_vFrwdDir = D3DXVECTOR3(tempFrwd.x, 0, tempFrwd.y);*/
//
//
//	m_vFrwdDir = m_vFrwdDir + m_vPos;
//	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &up, &m_vFrwdDir);
//}





void Dx11_Camera::UpdateCamera(char in)
{
	float speed = 0.1f;
	float amt = 10 * speed;
	float angle = 100 * speed;


	switch (in)
	{
	case VK_LEFT:
	{
		if (GetAsyncKeyState(VK_SHIFT))
		{
			m_fYaw -= 0.05f;
		}
		else
		{
			D3DXVECTOR3 left = GetLeft();
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, left, 10 * amt);			//CHANGING POS								
			}
			else
			{
				Move(&m_vPos, left, amt);
			}

		}
	}
	break;

	case VK_RIGHT:
	{
		if (GetAsyncKeyState(VK_SHIFT))
		{
			m_fYaw += 0.05f;
		}
		else
		{
			D3DXVECTOR3 ryt = GetRight();

			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, ryt, 10 * amt); 
			}
			else
			{
				Move(&m_vPos, ryt, amt);			//CHANGING POS								
			}

		}

	}
	break;

	case VK_UP:
	{
		if (GetAsyncKeyState(VK_CONTROL))
		{
			Move(&m_vPos, m_vFrwdDir, 5*amt);
		}
		else
		{
			Move(&m_vPos, m_vFrwdDir, amt);
		}
		
	}
	break;

	case VK_DOWN:
	{				
		if (GetAsyncKeyState(VK_CONTROL))
		{
			Move(&m_vPos, m_vFrwdDir, -5*amt);
		}
		else
		{
			Move(&m_vPos, m_vFrwdDir, -amt);
		}
	}
	break;

	case VK_PRIOR: //Up			
		Move(&m_vPos, m_vUp, amt);
		break;

	case VK_NEXT: //Down			
		Move(&m_vPos, m_vUp, -amt);
		break;

	case 'R':
		
		break;

	case 'T':
		
		if (m_fPitch < 6.28)
			m_fPitch += 0.05f;
		else
			m_fPitch = 0.0f;

		break;

	case WM_MOUSEHWHEEL:
		m_fPitch -= 0.05f;

		break;
	}

	D3DXVECTOR3 vDir;
	vDir.x = cosf(m_fPitch)*sinf(m_fYaw);
	vDir.y = sinf(m_fPitch);
	vDir.z = cosf(m_fPitch)*cosf(m_fYaw);

	D3DXVECTOR2 vFrwdDir = D3DXVECTOR2(vDir.x, vDir.z);
	D3DXVec2Normalize(&vFrwdDir, &vFrwdDir);
	m_vFrwdDir = D3DXVECTOR3(vFrwdDir.x, 0.0f, vFrwdDir.y);


	D3DXVECTOR3 vTarget;
	D3DXVec3Add(&vTarget, &m_vPos, &m_vFrwdDir);
	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &vTarget, &m_vUp);

}


void Dx11_Camera::Move(D3DXVECTOR3 *pos, D3DXVECTOR3 dir, int amount)
{
	D3DXVec3Scale(&dir, &dir, amount);
	D3DXVec3Add(pos, pos, &dir);
}

D3DXVECTOR3 Dx11_Camera::GetLeft()
{
	D3DXVECTOR3 left;
	D3DXVec3Cross(&left, &m_vFrwdDir, &m_vUp);
	D3DXVec3Normalize(&left, &left);
	return left;
}


D3DXVECTOR3 Dx11_Camera::GetRight()
{
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &m_vUp, &m_vFrwdDir);
	D3DXVec3Normalize(&right, &right);
	return right;
}


D3DXMATRIX Dx11_Camera::GetViewMatrix(D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraDir)
{
	//REFLECTION FROM 	
	//D3DXVec3Normalize(&vCameraDir, &vCameraDir);

	D3DXMATRIX matReflection;
	D3DXMatrixLookAtLH(&matReflection, &vCameraPos, &vCameraDir, &m_vUp);
	
	return matReflection;
}

D3DXMATRIX Dx11_Camera::GetViewMatrix(float posx, float posy, float posz, float dirx, float diry, float dirz)
{
	D3DXVECTOR3 vCameraPos(posx, posy, posz);
	D3DXVECTOR3 vCameraDir(dirx, diry, dirz);

	D3DXMATRIX matReflection;
	D3DXMatrixLookAtLH(&matReflection, &vCameraPos, &vCameraDir, &m_vUp);

	return matReflection;
}


