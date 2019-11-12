
#include "FI_SimLightEx.h"

//////////////////////////////////////////////////////////////////////////
//USED IN SPLIT FUNCTION
#include <string>
#include <sstream>
#include <vector>
//////////////////////////////////////////////////////////////////////////





void FI_LightSector::GetDefaultValuesFromXML(LGT_LITCHR lightCharacteristic, float *signalPeriod, float *duration1, float *darkDuration1, float *duration2, float *darkDuration2)
{

	LightExConfiguration *pConfig = NULL;

	if (m_pLightExConfig.size() > 0)
	{		
		std::map<LGT_LITCHR, LightExConfiguration *>::iterator Itr;
		for (Itr = m_pLightExConfig.begin(); Itr != m_pLightExConfig.end(); Itr++)
		{
			if (lightCharacteristic == Itr->first)
			{
				pConfig = Itr->second;

				*signalPeriod = pConfig->fSignalPeriod;
				*duration1 = pConfig->fPulseDuration;
				*darkDuration1 = pConfig->fDarkPulseDuration;

				*duration2 = pConfig->fPulseDuration2;
				*darkDuration2 = pConfig->fDarkPulseDuration2;
				break;
			}
			
		}
	}
	
}

FI_SimLightEx::FI_SimLightEx(FI_Sim *sim) : m_fMaxPixelSize(256.0f), m_fMinPixelSize(5), m_fFadeFactor(0.0f)
{
	//mSim = sim;
	//objtype = FI_SimObjTypeLight;

	m_bLightState = false;
	m_fIntensity = 1.0f;

	m_pSimObj = NULL;
}


FI_SimLightEx::~FI_SimLightEx()
{
	//delete m_vSectors;
}


FI_LightSector::FI_LightSector()
{

	m_fStartAngle = 0;
	m_fEndAngle = 360;
	m_fVStartAngle = 0.0f;
	m_fVEndAngle = 2 * D3DX_PI;


	//size=2.2f;
	m_fSize = 1.0f;
	m_fRange = 6.0;
	//m_fIntensity = 1.0f;
	
	m_bLightStatus = false;
	m_iCurrentPulseIndex = 0;		//INITIALLY PULSE AT ZERO INDEX IN m_vPulses
	m_CurrentColor = LGT_BLACK;	    //BY DEFAULT PULSE COLOR IS BLACK SO THAT IS SHOULD NOT RENDER UNTIL ITS PROCESS.

	m_fShipLightSectorStartExt = 0;
	m_fShipLightSectorEndExt = 0;

	m_fExtSize = 1.0f;
	m_fResizeFactor = 1.0f;
}


void FI_SectorPulse::SetPulse(LGT_COLOR col, float dur)
{
	m_fDuration = dur;
	m_Color = col;
}


void FI_SimLightEx::AddSector()
{	
	FI_LightSector *pSector = new FI_LightSector();

	//pSector->m_pLightExConfig = mSim->m_mapLightExConfig;
	m_vSectors.push_back(pSector);


	//
}



void FI_LightSector::AddPulse(char *pColor, char *pLgtChr, float sigper, char *pSigSeq, char *pSigGrp)
{	

	FI_SectorPulse *pPulse = NULL;	



	//GENERATING COLOR ARRAY
	std::vector<LGT_COLOR> vCol = GenerateColor(pColor,'_');


	LGT_LITCHR lightCHR = (LGT_LITCHR)atoi(pLgtChr);


	float fDefaultPulseDuration = 0, fDefaultSignalPeriod = 0, fDefaultDarkPulseDuration = 0, fDefaultPulseDuration2 = 0, fDefaultDarkPulseDuration2 = 0;
	GetDefaultValuesFromXML(lightCHR, &fDefaultSignalPeriod, &fDefaultPulseDuration, &fDefaultDarkPulseDuration, &fDefaultPulseDuration2, &fDefaultDarkPulseDuration2);
	m_pLightExConfig.clear();
	
	
	//SET LIGHT CHARACTERISTIC
	switch (atoi(pLgtChr))
	{
	
		default:
		//case LGT_FIXED:
			{
				//Adding White Light
				pPulse = new FI_SectorPulse();				
				pPulse->SetPulse(vCol[0], sigper);
				m_vPulses.push_back(pPulse);
			}
			break;


		case LGT_FLASH_ALTERNATING:
		case LGT_FLASHING:
		{								 
			float dura = 0.7, dark = 0.5f;			
			if (sigper == 0)			
				sigper = fDefaultSignalPeriod;
				
			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializeFlashingPulse(dura, dark, sigper, pSigGrp, vCol);

		}
		break;

		case LGT_LONG_FLASH_ALTERNATING:
		case LGT_LONG_FLASHING:
		{
			float dura = 2.5f, dark = 1.0f;
			if (sigper == 0)			
				sigper = fDefaultSignalPeriod;			
			

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;


		case LGT_QUICK_FLASHING:
		{			
			float dura = 0.4f, dark = 0.4f;
			
			if (sigper == 0)		
				sigper = fDefaultSignalPeriod;		

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;


			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;
		

		case LGT_VERY_QUICK_FLASHING:
		{
			float dura = 0.2f, dark = 0.2f;
			
			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;

		
		case LGT_ULTRA_QUICK_FLASHING:
		{
			float dura = 0.15f, dark = 0.15f;

			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;

		case LGT_ISOPHASED:
		{
			if (sigper == 0)
			{
				//sigper = 10.0f;										//Default 10Sec	
				sigper = fDefaultSignalPeriod;				
			}
				
			
			//Adding White Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[0], sigper/2);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], sigper/2);
			m_vPulses.push_back(pPulse);

		}
		break;


		case LGT_OCCULTING_AND_ALTERNATING:
		case LGT_OCCULTING:
			{
				if (sigper == 0)
				{				
					sigper = fDefaultSignalPeriod;
				}

				std::vector<int> vSigGrp = GetSignalGroup(pSigGrp);
				
				int nPulses = 0, PulseCount=0;										//No Of Pulses : DECIDE BY SIGNAL GROUP
				for (int indx = 0; indx < vSigGrp.size(); indx++)
				{
					nPulses += vSigGrp[indx];
				}
				
				float dura = sigper;
				for (int grpId = 0; grpId < vSigGrp.size(); grpId++)
				{

					for (int pulseId = 0; pulseId < vSigGrp[grpId]; pulseId++)
					{
						
						float interval;
						PulseCount++;
						if (PulseCount < nPulses)
						{
							interval = dura*0.66;
							dura = dura*0.34;										//remaining
						}
						else
							interval = dura;				
 

						//Adding White Light
						pPulse = new FI_SectorPulse();
						pPulse->SetPulse(vCol[0], interval*0.66);
						m_vPulses.push_back(pPulse);

						//Adding Black Light
						pPulse = new FI_SectorPulse();
						pPulse->SetPulse(vCol[1], interval*0.34);
						m_vPulses.push_back(pPulse);
					}
					
				}

				

			}
			break;

		case LGT_INTERRUPTED_QUICK_FLASHING:
		{
			float dura = 0.4f, dark = 0.6f;

			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;

		
		case LGT_INTERRUPTED_VERY_QUICK_FLASHING:
		{
			float dura = 0.2f, dark = 0.3f;

			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;



		case LGT_INTERRUPTED_ULTRA_QUICK_FLASHING:
		{
			float dura = 0.15f, dark = 0.2f;

			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration;

			InitializePulse(dura, dark, sigper, pSigGrp, vCol);
		}
		break;
		

		case LGT_MORSE:
			{
				float duration = 1.0f;

				if(fDefaultPulseDuration > 0)
					duration = fDefaultPulseDuration;
				
				if (sigper == 0)
					sigper = fDefaultSignalPeriod;

				//NO OF CASE MUCH MORE SO SHIFTED INTO ANOTHRE FUNCTION
				char code = pSigGrp[1];
				InitializeMorseCodePulse(code, vCol, sigper, duration);

			}
			break;

		
		case LGT_FIXED_AND_ALTERNATING_FLASHING:
		case LGT_FIXED_AND_FLASHING:
		{
			float dura = 0.7, dark = 0.5f;
			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura = fDefaultPulseDuration2;

			if (fDefaultDarkPulseDuration > 0)
				dark = fDefaultDarkPulseDuration2;


			char *pSigGrp1 = NULL, *pSigGrp2 = NULL;
			GetDualSignalGroup(pSigGrp, pSigGrp1, pSigGrp2);
			InitializeFlashingPulse(dura, dark, sigper, pSigGrp2, vCol);

			float fRemains = sigper - GetRemainingTimeToSetOnLastPulse();
			if (fRemains < 0)
				fRemains = fDefaultPulseDuration;

			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[0], fRemains);

			
		}
		break;

		case LGT_FLASHING_AND_LONG_FLASHING:
		{
			float dura1 = 0.7, dark1 = 0.5f;
			float dura2 = 2.5f, dark2 = 1.0f;


			if (sigper == 0)
				sigper = fDefaultSignalPeriod;

			if (fDefaultPulseDuration > 0)
				dura1 = fDefaultPulseDuration;

			if (fDefaultDarkPulseDuration > 0)
				dark1 = fDefaultDarkPulseDuration;

			if (fDefaultPulseDuration2 > 0)
				dura2 = fDefaultPulseDuration2;

			if (fDefaultDarkPulseDuration2 > 0)
				dark2 = fDefaultDarkPulseDuration2;

			char *pSigGrp1 = NULL, *pSigGrp2 = NULL;
			GetDualSignalGroup(pSigGrp, pSigGrp1, pSigGrp2);
			InitializeDualCharacteristicLight(dura1, dark1, dura2, dark2, pSigGrp1, pSigGrp2, sigper, vCol);

		}
		break;

		/*
		LGT_OCCULTING_AND_FLASHING = 15,
			*/

		//20-24 REMOVED


		case LGT_QUICK_FLASH_PLUS_LONG_FLASH:
			{
				if (sigper == 0)
					sigper = fDefaultSignalPeriod;
					
				float dura1 = 0.4f, dark1 = 0.4f;
				float dura2 = 2.5f, dark2 = 1.0f;


				//SETTING DEFAULT VALUE
				if (fDefaultPulseDuration > 0)
					dura1 = fDefaultPulseDuration;

				if (fDefaultDarkPulseDuration > 0)
					dark1 = fDefaultDarkPulseDuration;

				if (fDefaultPulseDuration2 > 0)
					dura2 = fDefaultPulseDuration2;

				if (fDefaultDarkPulseDuration2 > 0)
					dark2 = fDefaultDarkPulseDuration2;

				char *pSigGrp1 = NULL, *pSigGrp2 = NULL;
				GetDualSignalGroup(pSigGrp, pSigGrp1, pSigGrp2);
				InitializeDualCharacteristicLight(dura1, dark1, dura2, dark2, pSigGrp1, pSigGrp2, sigper, vCol);

			}
			break;


		case LGT_VERY_QUICK_FLASH_PLUS_LONG_FLASH:
			{		

				if (sigper == 0)
					sigper = fDefaultSignalPeriod;

				float dura1 = 0.2f, dark1 = 0.2f;
				float dura2 = 2.5f, dark2 = 1.0f;


				//SETTING DEFAULT VALUE
				if (fDefaultPulseDuration > 0)
					dura1 = fDefaultPulseDuration;

				if (fDefaultDarkPulseDuration > 0)
					dark1 = fDefaultDarkPulseDuration;

				if (fDefaultPulseDuration2 > 0)
					dura2 = fDefaultPulseDuration2;

				if (fDefaultDarkPulseDuration2 > 0)
					dark2 = fDefaultDarkPulseDuration2;

				char *pSigGrp1 = NULL, *pSigGrp2 = NULL;
				GetDualSignalGroup(pSigGrp, pSigGrp1, pSigGrp2);
				InitializeDualCharacteristicLight(dura1, dark1, dura2, dark2, pSigGrp1, pSigGrp2, sigper, vCol);
				
			}
			break;

		case LGT_ULTRA_QUICK_FLASH_PLUS_LONG_FLASH:
			{

				if (sigper == 0)
					sigper = fDefaultSignalPeriod;

				float dura1 = 0.15f, dark1 = 0.15f;
				float dura2 = 2.5f, dark2 = 1.0f;


				//SETTING DEFAULT VALUE
				if (fDefaultPulseDuration > 0)
					dura1 = fDefaultPulseDuration;

				if (fDefaultDarkPulseDuration > 0)
					dark1 = fDefaultDarkPulseDuration;

				if (fDefaultPulseDuration2 > 0)
					dura2 = fDefaultPulseDuration2;

				if (fDefaultDarkPulseDuration2 > 0)
					dark2 = fDefaultDarkPulseDuration2;

				char *pSigGrp1 = NULL, *pSigGrp2 = NULL;
				GetDualSignalGroup(pSigGrp, pSigGrp1, pSigGrp2);
				InitializeDualCharacteristicLight(dura1, dark1, dura2, dark2, pSigGrp1, pSigGrp2, sigper, vCol);

			}
			break;

		case LGT_ALTERNATING:
			{
				if (sigper == 0)
				{					
					sigper = fDefaultSignalPeriod;					
				}
					

				int nColor = vCol.size();				
				for (int pulseIndx = 0; pulseIndx < nColor; pulseIndx++)
				{
					pPulse = new FI_SectorPulse();
					pPulse->SetPulse(vCol[pulseIndx], sigper/nColor);
					m_vPulses.push_back(pPulse);
				}
			
			}
			break;
	}


}






void FI_SimLightEx::Process(float ticks)
{	
	D3DXVECTOR3 vFramePos;
	/*if (m_pFrame)			
		m_pFrame->GetPosition(mSim->GetScene(), &vFramePos);*/
	
	

	std::vector<FI_LightSector *>::iterator iter;
	FI_LightSector *pSector = NULL;
	for (iter = m_vSectors.begin(); iter != m_vSectors.end(); iter++)
	{			
		//Processing Sector Light
		pSector = (*iter);
		if (m_bLightState && pSector)
		{
			pSector->Process(ticks, vFramePos, m_pFrame);

			////IF A SECTOR LIGHT STATUS IS TRUE, MEANS THIS SECTOR IS UNDER VIEW ANGLE AND NO OTHER SECTOR WILL BE VISIBLE
			////SO NO NEED TO PROCESS OTHERS SECTORS.
			//if (pSector->GetSectorLightStatus())
			//	break;
		}

	}

}


//////////////////////////////////////////////////////////////////////////
//TEST FOUR CONDITION
//1- IS LIGHT UNDER VISIBLITY RANGE
//2- IS LIGHT IN VISIBLE HORIZONTAL SECTOR REGION
//3- IS LIGHT IN VISIBLE VERTICAL SECTOR REGION
//4- COMPUTE CURRENT PULSE COLOR
//////////////////////////////////////////////////////////////////////////
void FI_LightSector::Process(float fTicks, D3DXVECTOR3 framePos, FI_Frame *pFrame)
{

	
	
	//D3DXVECTOR3 vCameraPos;// = g_vEyePt;	
	//D3DXVECTOR3 vDistanceFromCamera;
	//D3DXVec3Subtract(&vDistanceFromCamera, &vCameraPos, &framePos);	
	//float fDistanceFromCamera = D3DXVec3Length(&vDistanceFromCamera);
	//fDistanceFromCamera /= 1852.3;

	////IS LIGHT SOURCE UNDER RANGE OF EYE POSITION
	//m_bLightStatus = (fDistanceFromCamera <= m_fRange) ? true : false;
	//if (!m_bLightStatus) return;    //LIGHT IS OUT OF RANGE
	


	////IS EYE POSITION FALLS HORIZONTAL IN SECTORS ANGLES		
	//float oheading = atan2(vCameraPos.x - framePos[0], vCameraPos.z - framePos[2]);
	//oheading = fmod(oheading + 10.0*FI_Metric::pi, 2 * FI_Metric::pi);

	//D3DXVECTOR3 vLgtNrm(0, 0, 1);
	//FIMATRIX *mat = pFrame->GetWorldTransform(false);
	//D3DXVec3TransformCoord(&vLgtNrm, &vLgtNrm, mat);
	//vLgtNrm = vLgtNrm - framePos;
	//D3DXVec3Normalize(&vLgtNrm, &vLgtNrm);
	//float fCurrentLgtPitch = acos(D3DXVec3Dot(&vLgtNrm, &D3DXVECTOR3(vLgtNrm.x, 0, vLgtNrm.z)));
	//if (!_finite(fCurrentLgtPitch))
	//	fCurrentLgtPitch = 0.0f;
	//float fCurrentLgtHdg = atan2(vLgtNrm.x, vLgtNrm.z);

	//
	//D3DXVECTOR3 vLDir = D3DXVECTOR3(vCameraPos) - framePos;
	//D3DXVec3Normalize(&vLDir, &vLDir);
	//D3DXVECTOR3 vLH(vLDir.x, 0, vLDir.z);
	//D3DXVec3Normalize(&vLH, &vLH);
	//float opitch = acos(D3DXVec3Dot(&vLDir, &vLH));
	//if (_finite(opitch))
	//	opitch = fmodf(opitch + 10.0f * D3DX_PI, 2.0*D3DX_PI);
	//else
	//	opitch = 0.0f;


	//
	//float ltheading = fLgtHeading;	
	//ltheading = fmod(ltheading + 4 * FI_Metric::pi, 2 * FI_Metric::pi);//ownership heading

	//m_fStartAngle = fmod(m_fStartAngle + 4 * FI_Metric::pi, 2 * FI_Metric::pi);
	//m_fEndAngle = fmod(m_fEndAngle + 4 * FI_Metric::pi, 2 * FI_Metric::pi);
	//
	//float s1 = m_fStartAngle + ltheading + m_fShipLightSectorStartExt;
	//float s2 = m_fEndAngle + ltheading + m_fShipLightSectorEndExt;

	//s1 = fmod(s1 + 4 * FI_Metric::pi, 2 * FI_Metric::pi);
	//s2 = fmod(s2 + 4 * FI_Metric::pi, 2 * FI_Metric::pi);


	//
	////if(m_fStartAngle + m_fEndAngle + 0.1 < 2*D3DX_PI)
	//{
	//	if (s1 > s2)
	//	{
	//		if ((oheading > s2) && (oheading <s1))
	//			m_bLightStatus = false;				
	//	}
	//	else
	//	{
	//		if (!(oheading>s1 && oheading < s2))
	//			m_bLightStatus = false;
	//	}
	//}
	//if (!m_bLightStatus)
	//	return;
	


	//IS EYE POSITION FALLS IN VERTICAL SECTORS ANGLES
	
	



	//PULSE DURATION DECIDE COLOR OF LIGHT		
	FI_SectorPulse *pPulse = GetPulse(m_iCurrentPulseIndex);	
	if (pPulse && fTicks>0.0f)
	{		
		//IF LIGHT HAS ONLY ONE PULSE, CURRENT PULSE INDEX WILL NOT BE UPDATE
		if (m_vPulses.size() > 1)
		{				
			m_fCurrentPulseTimeElapsed += fTicks;
			if (m_fCurrentPulseTimeElapsed >= pPulse->GetDuration())
			{
				m_iCurrentPulseIndex = (m_iCurrentPulseIndex + 1) % m_vPulses.size();
				m_fCurrentPulseTimeElapsed = 0;
				m_CurrentColor = GetPulse(m_iCurrentPulseIndex)->GetColor();
			}
	
		}
		else
		{
			m_CurrentColor = GetPulse(m_iCurrentPulseIndex)->GetColor();
		}
	}		
	m_bLightStatus = (m_CurrentColor == LGT_BLACK) ? false : true;
	
	
	/*if(m_bLightStatus)
	{
		float es1 = fmod(s1+m_fShipLightSectorStartExt + 4 * FI_Metric::pi, 2 * FI_Metric::pi);
		float es2 = fmod(s2+m_fShipLightSectorEndExt + 4 * FI_Metric::pi, 2 * FI_Metric::pi);

		if(fabs(m_fShipLightSectorStartExt)>0 &&  oheading >es1)
			m_fResizeFactor = m_fExtSize*min(fabs((oheading-s1)/m_fShipLightSectorStartExt),1.0);
		else if(fabs(m_fShipLightSectorEndExt)>0 &&  oheading<es2)
			m_fResizeFactor = m_fExtSize*min(fabs((s2-oheading)/m_fShipLightSectorEndExt),1.0);
		else
			m_fResizeFactor = m_fExtSize;	
	}*/

}


D3DXCOLOR FI_LightSector::GetColorByIndex(LGT_COLOR colId)
{

	D3DXCOLOR color;
	
	
	switch (colId)
	{

		case LGT_WHITE:
			color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			break;

		case LGT_BLACK:
			color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		
		case LGT_RED:
			color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		
		case LGT_GREEN:
			color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			break;

		case LGT_BLUE:
			color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);;
			break;

		case LGT_YELLOW:
			color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
			break;
			
		case LGT_GREY:
			color = D3DXCOLOR(128/255, 128/255, 128/255, 1.0f);
			break;

		case LGT_BROWN:
			color = D3DXCOLOR(128/255, 64/255, 0, 1.0f);
			break;

		case LGT_AMBER:
			color = D3DXCOLOR(0, 255/255, 255/255, 1.0f);
			break;

		case LGT_VIOLET:
			color = D3DXCOLOR(128/255, 128/255, 255/255, 1.0f);
			break;


		case LGT_ORANGE:
			color = D3DXCOLOR(255/255, 128/255, 0, 1.0f);
			break;

		case LGT_MAGENTA:
			color = D3DXCOLOR(255/255, 0, 128/255, 1.0f);
			break;

		case LGT_PINK:
			color = D3DXCOLOR(255/255, 128/255, 128/255, 1.0f);
			break;			 


		case LGT_BLUE_STEERING:
			color = D3DXCOLOR(0, 0, 240/255, 1.0f);
			break;

		default:
			//WHITE
			color = D3DXCOLOR(255/255, 255/255, 255/255, 1.0f);
			break;
	}

	return color;
}


int FI_SimLightEx::Render()
{
	return 0;
}


void FI_SimLightEx::SetIntensity(float fVal)
{
	m_fIntensity = fVal;
}


void FI_SimLightEx::Release()
{
	/*std::vector<FI_LightSector *>::iterator iter;
	for (iter = m_vSectors.begin(); iter != m_vSectors.end(); iter++)
	{
	delete (*iter);
	}*/
}


//FI_LightSector* FI_SimLightEx::GetSector(int iSectorIndex)
//{
//	if (iSectorIndex < m_vSectors.size())
//	{
//		return m_vSectors[iSectorIndex];
//	}
//	
//	return NULL;
//}


FI_SectorPulse* FI_LightSector::GetPulse(int iPulseIndex)
{
	if (iPulseIndex < m_vPulses.size())
	{
		return m_vPulses[iPulseIndex];
	}

	return NULL;
}

FI_SectorPulse::FI_SectorPulse()
{
	m_Color = LGT_BLACK;
	m_fDuration = 1.0f;
}



void FI_LightSector::SetSectorAngle(float start, float end, float vstart, float vend)
{
	
	m_fStartAngle = start;
	m_fEndAngle = end;

	m_fVStartAngle = vstart;
	m_fVEndAngle = vend;
}


void FI_LightSector::SetSectorRange(float range)
{
	m_fRange = range;
}


void FI_LightSector::SetSectorSize(float size)
{
	m_fSize = size;
}


//void FI_LightSector::SetSectorIntensity(float intensity)
//{
//	m_fIntensity = intensity;
//}


void FI_LightSector::SetSectorHeight(float height)
{
	m_fHeight = height;
}

void FI_LightSector::SetSectorLightStatus(bool state)
{
	m_bLightStatus = state;
}

void FI_LightSector::InitializeMorseCodePulse(char code, std::vector<LGT_COLOR> vCol, float sigper, float fLightOnDuration)
{

	FI_SectorPulse *pPulse = NULL;	
	float duration = fLightOnDuration;

	float fRemTime = 0.0f;

	switch (code)
	{
		case 'A':
			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[0], duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[0], 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);
			break;


		case 'B':
			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[0], 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);

			break;

		case 'C':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);

			break;

		case 'D':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);			

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);

			break;

		case 'E':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);			

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);

			break;


		case 'F':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse(vCol[1], GetMorseCodeRemainingTime(sigper));
			m_vPulses.push_back(pPulse);

			break;

		case 'G':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);


			duration = sigper - ((2*((3*duration)+duration)) + (duration));
			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			break;
		
		case 'H':
			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			duration = sigper - (2*(duration+duration) + (duration));
			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			break;

		case 'I':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);


			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			duration = sigper - (3*(duration));
			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			break;

		case 'J':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);



			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);



			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);



			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);


			duration = sigper - (13*(duration));
			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			break;


		case 'K':
			{
				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (9*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;


		case 'L':
			{
				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (9*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;


		case 'M':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (7*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;

		case 'N':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (5*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;

		case 'O':
			{

				if (sigper - (11*(duration)) > 4*duration)
					duration /=2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (11*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;


		case 'P':
			{

				if (sigper - (11*(duration)) > 4*duration)
					duration /=2;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (11*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;


		case 'Q':
			{

				if (sigper - (13*(duration)) > 4*duration)
					duration -= duration/2;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (13*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				
			}
			break;


		case 'R':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (7*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



			}
			break;



		case 'S':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (5*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case 'T':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (3*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;

		case 'U':
			{

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (7*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;


		case 'V':
			{

				if(sigper - (9*(duration)) > 4*duration)
					duration -= duration/2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (9*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;

		case 'W':
			{

				if(sigper - (9*(duration)) > 4*duration)
					duration -= duration/2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);



				duration = sigper - (9*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case 'X':
			{

				if (sigper - (11*(duration)) > 4*duration)
					duration -= duration/2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (11*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case 'Y':
			{

				if (sigper - (13*(duration)) > 4*duration)
					duration -= duration/2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (13*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case 'Z':
			{
				if (sigper - (11*(duration)) > 4*duration)
				duration -= duration/2;

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);


				duration = sigper - (11*(duration));
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;

		


		//ZERO
		case '0':

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), sigper);
			m_vPulses.push_back(pPulse);


			break;


		//ONE
		case '1':
			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), duration); 
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);
			
			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), duration);
			m_vPulses.push_back(pPulse);

			//Adding Color Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), 3*duration);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), sigper);
			m_vPulses.push_back(pPulse);

			break;

		case '2':
			{

				if (sigper - (15*duration) > duration*4)
					duration /= 2;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (15*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;

		case '3':
			{				

				if (sigper - (13*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (13*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;

		case '4':
			{

				if (sigper - (11*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (11*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;


		case '5':
			{
				if (sigper - (9*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (9*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;


		case '6':
			{
				if (sigper - (11*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (11*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


			}
			break;

		case '7':
			{
				if (sigper - (13*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (13*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case '8':
			{
				if (sigper - (15*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (15*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);
			}
			break;


		case '9':
			{
				if (sigper - (17*duration) > duration*4)
					duration /=2 ;


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);



				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);


				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), 3*duration); 
				m_vPulses.push_back(pPulse);

				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

				//Adding Color Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[0]), duration); 
				m_vPulses.push_back(pPulse);

				duration = sigper - (17*duration);
				//Adding Black Light
				pPulse = new FI_SectorPulse();
				pPulse->SetPulse((vCol[1]), duration);
				m_vPulses.push_back(pPulse);

			}
			break;

	}

}


std::vector<LGT_COLOR> FI_LightSector::GenerateColor(const std::string &s, char delim)
{
	std::vector<LGT_COLOR> color;

	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		color.push_back((LGT_COLOR)atoi(item.c_str()));			//ADDING COLOR ID 
		color.push_back(LGT_BLACK);								//ADDING BLACK COLOR
	}

	return color;
}

void FI_LightSector::StringToken(const std::string &s, char delim, std::vector <int>&vSigGrp)
{

	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{		
		vSigGrp.push_back(atoi(item.c_str()));		
	}
	
}

std::vector<int> FI_LightSector::GetSignalGroup(char *pSigGrp)
{
	
	std::vector <int> vSigGrp;
	if (pSigGrp != NULL)
	{		
		int nBracketStart = -1, nBracketEnd = -1;
		for (int nAtChar = 0; nAtChar < strlen(pSigGrp); nAtChar++)
		{
			if (pSigGrp[nAtChar] == '(')
			{
				nBracketEnd = nBracketStart = nAtChar;
				continue;
			}

			if (pSigGrp[nAtChar] != ')')
			{
				nBracketEnd++;
				continue;
			}


			if (nBracketStart < nBracketEnd)
			{
				char *szParse = new char[nBracketEnd - nBracketStart];
				memcpy(szParse, &pSigGrp[nBracketStart + 1], nBracketEnd);
				szParse[nBracketEnd - nBracketStart] = '\0';
				StringToken(szParse, '+', vSigGrp);

				//Reset
				nBracketEnd = nBracketStart = -1;
			}
			else if ((nBracketEnd != -1) && (nBracketStart == nBracketEnd))
			{
				vSigGrp.push_back(1);
			}
		}		
	}
	else		
	{
		//Signal Group When : () 
		vSigGrp.push_back(1);
	}

	//SIGNAL ALWAYS HAS ATLEAST ONE PULSE
	if (vSigGrp.size() == 0)
		vSigGrp.push_back(1);

	return vSigGrp;

}


void FI_LightSector::GetDualSignalGroup(char *pSigGrp, char* &pSignalGroup1, char* &pSignalGroup2)
{
	
	bool bGroup1Found = false, bGroup2Found = false;
	if (pSigGrp != NULL)
	{
		int nBracketStart = -1, nBracketEnd = -1;
		for (int nAtChar = 0; nAtChar < strlen(pSigGrp); nAtChar++)
		{
			if (pSigGrp[nAtChar] == '(')
			{
				nBracketEnd = nBracketStart = nAtChar;
				continue;
			}

			if (pSigGrp[nAtChar] != ')')
			{
				nBracketEnd++;
				continue;
			}			
			

			
			if (nBracketStart < nBracketEnd)
			{
				nBracketEnd++;
				if (bGroup1Found == false)
				{
					pSignalGroup1 = new char[nBracketEnd - nBracketStart + 1];
					memcpy(pSignalGroup1, &pSigGrp[nBracketStart], nBracketEnd + 1);					
					pSignalGroup1[nBracketEnd - nBracketStart + 1]  = '\0';
					

					bGroup1Found = true;
				}
				else if (bGroup2Found == false)
				{
					pSignalGroup2 = new char[nBracketEnd - nBracketStart + 1];
					memcpy(pSignalGroup2, &pSigGrp[nBracketStart], nBracketEnd + 1);
					pSignalGroup2[nBracketEnd - nBracketStart + 1] = '\0';

					bGroup2Found = true;
				}


				//Reset
				nBracketEnd = nBracketStart = -1;
			}
			else if ((nBracketEnd != -1) && (nBracketStart == nBracketEnd))
			{
				//MEANS REACHED AT THE END OF STRING 
				
			}
		}
	}
	

	if (bGroup1Found == false)
	{
		pSignalGroup1 = new char[3];
		strcpy_s(pSignalGroup1, 4,"()");
	}

	if (bGroup2Found == false)
	{
		pSignalGroup2 = new char[3];
		strcpy_s(pSignalGroup2, 4, "()");
	}

}

void FI_LightSector::InitializePulse(float dura, float eclipseTime, float sigper, char *pSigGrp, std::vector<LGT_COLOR> vCol)
{
	FI_SectorPulse *pPulse = NULL;
	std::vector<int> vSigGrp = GetSignalGroup(pSigGrp);

	int nPulses = 0;										//No Of Pulses : DECIDE BY SIGNAL GROUP
	for (int indx = 0; indx < vSigGrp.size(); indx++)
	{
		nPulses += vSigGrp[indx];
	}

	for (int iGrpId = 0; iGrpId < vSigGrp.size(); iGrpId++)
	{
		for (int iPulseId = 0; iPulseId < vSigGrp[iGrpId]; iPulseId++)
		{
			//Adding White Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), dura);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), eclipseTime);
			m_vPulses.push_back(pPulse);

			if (iPulseId == vSigGrp[iGrpId] - 1)
			{
				pPulse = m_vPulses[m_vPulses.size() - 1];
				pPulse->SetPulse(vCol[1], 2 * eclipseTime);
			}

		}

		if (iGrpId == vSigGrp.size() - 1)
		{
			//remaining duration								
			pPulse = m_vPulses[m_vPulses.size() - 1];
			//float fRemainningTime = sigper - (nPulses*(dura + eclipseTime) + eclipseTime);
			float fRemainningTime = sigper - GetRemainingTimeToSetOnLastPulse();
			if (fRemainningTime < 0)
				fRemainningTime = dura + 2 * eclipseTime;
			pPulse->SetPulse(vCol[1], fRemainningTime);
		}
	}
}


void FI_LightSector::AddPulseByPattern(char *pColor, char *pPattern, float duration)
{

	//TELERING SPACE
	int nSize = strlen(pPattern);
	if (pPattern[nSize - 1] == ' ')
		nSize -= 1;	

	if (nSize > 0)
	{
		//Pulse DURATION IN MILI SECOND
		if (duration == 0)
			duration = 1000;			//DEFAULT 1 Sec
		duration = duration / 1000;

		std::vector<LGT_COLOR> vCol;
		//GENERATING COLOR ARRAY
		if (pColor == NULL)
		{
			vCol.push_back(LGT_WHITE);
			vCol.push_back(LGT_BLACK);
		}
		else
			vCol = GenerateColor(pColor, '_');
		

		FI_SectorPulse *pPulse = NULL;
		for (int iPulseId = 0; iPulseId<nSize; iPulseId++)
		{
			pPulse = new FI_SectorPulse();

			char Val = pPattern[iPulseId];
			int  indx = iPulseId + 1, nCount=1;
			while (indx < nSize)
			{
				if (Val == pPattern[indx])
					indx++;
				else
					break;
			}
			
			nCount = indx - iPulseId;

			if (pPattern[iPulseId] == '1')
			{				
				pPulse->SetPulse(vCol[0], nCount*duration);
			}
			else
			{
				pPulse->SetPulse(LGT_BLACK, nCount*duration);
			}
			m_vPulses.push_back(pPulse);

			if (nCount > 1)
			{
				iPulseId = indx -1;
			}
		}

	}
	
}




void FI_LightSector::InitializeFlashingPulse(float dura, float eclipseTime, float sigper, char *pSigGrp, std::vector<LGT_COLOR> vCol)
{
	FI_SectorPulse *pPulse = NULL;
	std::vector<int> vSigGrp = GetSignalGroup(pSigGrp);

	int nPulses = 0;										//No Of Pulses : DECIDE BY SIGNAL GROUP
	for (int indx = 0; indx < vSigGrp.size(); indx++)
	{
		nPulses += vSigGrp[indx];
	}

	for (int iGrpId = 0; iGrpId < vSigGrp.size(); iGrpId++)
	{
		for (int iPulseId = 0; iPulseId < vSigGrp[iGrpId]; iPulseId++)
		{
			//Adding White Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), dura);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), eclipseTime);
			m_vPulses.push_back(pPulse);

			if (iPulseId == vSigGrp[iGrpId] - 1)
			{
				pPulse = m_vPulses[m_vPulses.size() - 1];
				pPulse->SetPulse(vCol[1],  2*eclipseTime);				
			}

		}

		if (iGrpId == vSigGrp.size() - 1)
		{
			//remaining duration								
			pPulse = m_vPulses[m_vPulses.size() - 1];

			
			//float fRemainningTime = sigper - (nPulses*(dura + eclipseTime) + eclipseTime);
			float fRemainningTime = sigper - GetRemainingTimeToSetOnLastPulse();
			if (fRemainningTime < 0)
				fRemainningTime = dura + 2 * eclipseTime;
			pPulse->SetPulse(vCol[1], fRemainningTime);
		}
	}
}


LGT_COLOR FI_LightSector::GetColorIndexByValue(char *pColorName)
{

	if(strcmp(pColorName, "255,255,255")==0)
		return LGT_WHITE;
	else if(strcmp(pColorName, "0,0,0")==0)
		return LGT_BLACK;
	else if(strcmp(pColorName, "255,0,0")==0)
		return LGT_RED;
	else if(strcmp(pColorName, "0,255,0")==0)
		return LGT_GREEN;
	else if(strcmp(pColorName, "0,0,255")==0)
		return  LGT_BLUE;
	else if(strcmp(pColorName, "255,255,0")==0)
		return LGT_YELLOW;
	else if(strcmp(pColorName, "128,128,128")==0)
		return  LGT_GREY;
	else if(strcmp(pColorName, "128,64,0")==0)
		return LGT_BROWN;
	else if(strcmp(pColorName, "0,255,255")==0)
		return LGT_AMBER;
	else if(strcmp(pColorName, "128,128,255")==0)
		return LGT_VIOLET;
	else if(strcmp(pColorName, "255,128,0")==0)
		return LGT_ORANGE;
	else if(strcmp(pColorName, "255,0,128")==0)
		return LGT_MAGENTA;
	else if(strcmp(pColorName, "255,128,128")==0)
		return LGT_PINK;
	else if(strcmp(pColorName, "0,0,240")==0)
		return LGT_BLUE_STEERING;
	else
		return LGT_WHITE;	
	
}




void FI_LightSector::InsertPulse(FI_SectorPulse *pPulse)
{
	m_vPulses.push_back(pPulse);
}



void FI_SimLightEx::ResetLightSector()
{
	for(int nSectorId=0; nSectorId < m_vSectors.size(); nSectorId++)
		delete m_vSectors[nSectorId];

	m_vSectors.clear();
}


FI_LightSector::~FI_LightSector()
{

	for(int nPulseId=0; nPulseId < m_vPulses.size(); nPulseId++)
		delete m_vPulses[nPulseId];

	m_vPulses.clear();
}


FI_SectorPulse::~FI_SectorPulse()
{

}


void FI_LightSector::ResetSectorPulse()
{
	for(int nPulseId=0; nPulseId < m_vPulses.size(); nPulseId++)
		delete m_vPulses[nPulseId];

	m_vPulses.clear();
}




float FI_LightSector::GetRemainingTimeToSetOnLastPulse()
{
	/*vector<FI_SectorPulse *>::iterator itrPulse;
	for(itrPulse = m_vPulses.begin(); itrPulse != m_vPulses.end(); itrPulse++)*/

	float fRemTime = 0.0f;
	for(int pulseId=0; pulseId < m_vPulses.size()-1; pulseId++)
	{
		//fRemTime += (*itrPulse)->GetDuration();		
		fRemTime += m_vPulses[pulseId]->GetDuration();
	}
	return fRemTime;
}


float FI_LightSector::GetMorseCodeRemainingTime(float signalPeriod)
{
	
	float fRemTime = 0.0f;
	if (signalPeriod > 0)
	{
		for (int pulseId = 0; pulseId < m_vPulses.size(); pulseId++)
		{
			fRemTime += m_vPulses[pulseId]->GetDuration();
		}
	}

	fRemTime = signalPeriod - fRemTime;
	if (fRemTime < 0)
		fRemTime = signalPeriod;
		
	
	return fRemTime;

}


void FI_LightSector::InitializeDualCharacteristicLight(float fDuration1, float fEclipse1, float fDuration2, float fEclipse2															
														, char *pSignalGroup1
														, char *pSignalGroup2
														, float fSignalPeriod
														, std::vector<LGT_COLOR> vCol)
{
	
	FI_SectorPulse *pPulse = NULL;
	std::vector<int> vSigGrp1 = GetSignalGroup(pSignalGroup1);

	for (int iGrpId = 0; iGrpId < vSigGrp1.size(); iGrpId++)
	{
		for (int iPulseId = 0; iPulseId < vSigGrp1[iGrpId]; iPulseId++)
		{
			//Adding White Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), fDuration1);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), fEclipse1);
			m_vPulses.push_back(pPulse);

			if (iPulseId == vSigGrp1[iGrpId] - 1)
			{
				pPulse = m_vPulses[m_vPulses.size() - 1];
				pPulse->SetPulse(vCol[1], 2 * fEclipse1);
			}
		}		
	}


	std::vector<int> vSigGrp2 = GetSignalGroup(pSignalGroup2);
	for (int iGrpId = 0; iGrpId < vSigGrp2.size(); iGrpId++)
	{
		for (int iPulseId = 0; iPulseId < vSigGrp2[iGrpId]; iPulseId++)
		{
			//Adding White Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[0]), fDuration2);
			m_vPulses.push_back(pPulse);

			//Adding Black Light
			pPulse = new FI_SectorPulse();
			pPulse->SetPulse((vCol[1]), fEclipse2);
			m_vPulses.push_back(pPulse);

			if (iPulseId == vSigGrp2[iGrpId] - 1)
			{
				pPulse = m_vPulses[m_vPulses.size() - 1];
				pPulse->SetPulse(vCol[1], 2 * fEclipse2);
			}
		}
	}

	//COMPUTE REMAINING TIME EXCEPT FOR LAST PULSE
	float fTotalPulseDuration=0;
	for (int pulseId = 0; pulseId < m_vPulses.size() - 1; pulseId++)
	{
		fTotalPulseDuration += m_vPulses[pulseId]->GetDuration();
	}

	float fRemainingTimeForLastPulse = fSignalPeriod - fTotalPulseDuration;
	if (fRemainingTimeForLastPulse <= 0)
		fRemainingTimeForLastPulse = fEclipse1 + fEclipse2;

	m_vPulses[m_vPulses.size() - 1]->SetPulse(vCol[1], fRemainingTimeForLastPulse);

}