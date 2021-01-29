/*****************************************************************
*
* The copyright to this file herein is the property of
* ARI, Hauzkhas delhi. This file may be used
* and/or copied only with the written permission of
* ARI, Hauzkhas delhi or in accordance with the
* terms and conditions stipulated in the agreement/contract
* under which this file has been supplied.
*
******************************************************************/

////////////////////////////////////////////////////////////////////
//
//	file		: FI_SimLightEx.cpp
//	author		: Tanveer Asif
//	date		: 20 April 2016

//  description : Main purpose of this class is, to fix Light Characteristic behavior. 
//                Light has sectors and sectors have pulses, so their separate supporting class also present.
//	
////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>
#include <string>
#include <ctime>

#include <map>
#include "FI_SimObj.h"





class FI_LightSector;
class FI_SectorPulse;



enum LGT_COLOR
{
	//INDEXING AS S-57.com
	LGT_WHITE = 1,
	LGT_BLACK =2,
	LGT_RED =3,
	LGT_GREEN =4,
	LGT_BLUE =5,
	LGT_YELLOW = 6,
	LGT_GREY =7,
	LGT_BROWN =8,
	LGT_AMBER =9,


	LGT_VIOLET = 10,
	LGT_ORANGE = 11,
	LGT_MAGENTA =12,
	LGT_PINK = 13,

	//LOAD ON SHIP
	LGT_BLUE_STEERING =14,

};

enum LGT_LITCHR
{
	//INDEXING AS S-57.com
	LGT_FIXED = 1,
	LGT_FLASHING = 2,
	LGT_LONG_FLASHING = 3,
	LGT_QUICK_FLASHING = 4,
	LGT_VERY_QUICK_FLASHING = 5,
	LGT_ULTRA_QUICK_FLASHING = 6,

	LGT_ISOPHASED = 7,
	
	LGT_OCCULTING = 8,

	LGT_INTERRUPTED_QUICK_FLASHING = 9,
	LGT_INTERRUPTED_VERY_QUICK_FLASHING = 10,
	LGT_INTERRUPTED_ULTRA_QUICK_FLASHING = 11,

	LGT_MORSE = 12,

	LGT_FIXED_AND_FLASHING = 13,
	LGT_FLASHING_AND_LONG_FLASHING = 14,
	LGT_OCCULTING_AND_FLASHING = 15,
	LGT_FIXED_AND_LONG_FLASHING = 16,
	LGT_OCCULTING_AND_ALTERNATING = 17,
	LGT_LONG_FLASH_ALTERNATING = 18,
	LGT_FLASH_ALTERNATING = 19,

	//20-24 REMOVED

	LGT_QUICK_FLASH_PLUS_LONG_FLASH       = 25,
	LGT_VERY_QUICK_FLASH_PLUS_LONG_FLASH  = 26,
	LGT_ULTRA_QUICK_FLASH_PLUS_LONG_FLASH = 27,

	LGT_ALTERNATING                       = 28,
	LGT_FIXED_AND_ALTERNATING_FLASHING    = 29,

};


struct LightExConfiguration 
{
	//LGT_LITCHR enTypeOfLight;
	float      fSignalPeriod;
	float      fPulseDuration;
	float	   fDarkPulseDuration;

	//USE IN DUAL CHARACTERISTIC LIGHT
	float      fPulseDuration2;
	float	   fDarkPulseDuration2;

};

typedef std::map<LGT_LITCHR, LightExConfiguration *> FI_LIGHT_EX_CONFIG;


class FI_SimLightEx : public FI_SimObj
{
	
	public:

		FI_SimLightEx(FI_Sim *sim);
		~FI_SimLightEx();

		void AddSector();
		void Process(float ticks);
		int  Render();
		void Release();
		
		std::vector<FI_LightSector *> m_vSectors;
		

		//void SetOwnerShip(FI_SimShip *pShip){ m_pOwnerShip = pShip;};	
		void SetLightState(bool val){ m_bLightState = val; };
		bool GetLightState(){return m_bLightState;};

		float m_fMaxPixelSize, m_fMinPixelSize;
		


		void SetFadeFactor(float _fFadeFactor)
		{
			m_fFadeFactor = _fFadeFactor;
			}
		
		float GetFadeFactor()
		{
			return m_fFadeFactor;
		}

		float GetIntensity(){ return m_fIntensity; };

		void SetIntensity(float fVal);

		void SetAttachedObject(FI_SimObj* pSimObj)
		{
			m_pSimObj = pSimObj;
		}

		FI_SimObj *GetAttachedObject(){ return m_pSimObj;}

		void ResetLightSector();
	private:
	   //FI_SimShip *m_pOwnerShip;

	   //STATE OF WORLD LIGHT IS ALWAYS TRUE
	   //STATE OF SHIP LIGHTS SET FROM NETWORK	   
	   //IF STATE IS OFF LIGHT SHOULD NOT BE PROCESS
	   //fibool   m_bLightState;
		bool   m_bLightState;
	   
	   float	m_fFadeFactor;
	   float	 m_fIntensity;
	   FI_SimObj* m_pSimObj;

};






class FI_LightSector
{
	public:
		FI_LightSector();
		~FI_LightSector();
		
		void AddPulse(char *pColor, char *pLgtChr, float sigper, char *pSigSeq=0, char *pSigGrp=0);
		
		//FOR LOADING LIGHT INFO FROM OLD FIW 
		void AddPulseByPattern(char *pColor, char *pPattern, float duration);


		void SetSectorAngle(float start, float end, float vstart = 0, float vend=2.0*D3DX_PI);		
		void SetSectorExtensions(float s, float e){ m_fShipLightSectorStartExt = s; m_fShipLightSectorEndExt = e;};

		void SetSectorHeight(float height);
		void SetSectorSize(float size);
		void SetSectorRange(float range);
		//void SetSectorIntensity(float intensity);
		void SetSectorLightStatus(bool state);

		
		//void Process(float fTick, float framePos[3], float lgtHeading, FI_Frame *pFrame);
		void Process(float fTick, D3DXVECTOR3 framePos, FI_Frame *pFrame);
	
		FI_SectorPulse* GetPulse(int iPulseIndex);		

		bool GetSectorLightStatus(){return m_bLightStatus;};
		float GetSectorLightSize(){ return m_fSize; };
		D3DXCOLOR GetSectorLightColor(){ return GetColorByIndex(m_CurrentColor); };
		float GetSectorLightRange(){ return m_fRange; };

		FI_LIGHT_EX_CONFIG m_pLightExConfig;		
		LGT_COLOR GetColorIndexByValue(char *pColorName);
		void InsertPulse(FI_SectorPulse *pPulse);

		float GetSizeExt(){return m_fResizeFactor;};
		void ResetSectorPulse();

	private:
		float    m_fStartAngle, m_fEndAngle, m_fVStartAngle, m_fVEndAngle;
		float    m_fShipLightSectorStartExt;
		float    m_fShipLightSectorEndExt;

		float    m_fSize;
		float	 m_fRange;						//UNIT : NM
		//float	 m_fIntensity;
		float    m_fHeight;
		bool   m_bLightStatus;
		
		int		   m_iCurrentPulseIndex;
		float      m_fCurrentPulseTimeElapsed;
		LGT_COLOR  m_CurrentColor;
		std::vector<FI_SectorPulse *> m_vPulses;

		float     m_fExtSize, m_fResizeFactor;
		
		
		D3DXCOLOR		  GetColorByIndex(LGT_COLOR colId);
		std::vector<LGT_COLOR> GenerateColor(const std::string &s, char delim);		
		void			  StringToken(const std::string &s, char delim, std::vector <int>&vSigGrp);
		std::vector<int>		  GetSignalGroup(char *pSigGrp);
		void			  GetDualSignalGroup(char *pSigGrp, char* &pSignalGroup1, char* &pSignalGroup2);
		void			  GetDefaultValuesFromXML(LGT_LITCHR chr, float *signalPeriod, float *duration1, float *darkDuration1, float *duration2, float *darkDuration2);
		float             GetRemainingTimeToSetOnLastPulse();
		float             GetMorseCodeRemainingTime(float signalPeriod);

		void InitializePulse(float dura, float eclipseTime, float sigper, char *pSigGrp, std::vector<LGT_COLOR> vCol);
		void InitializeMorseCodePulse(char code, std::vector<LGT_COLOR> vCol, float sigper, float fLightOnDuration);		
		void InitializeFlashingPulse(float dura, float eclipseTime, float sigper, char *pSigGrp, std::vector<LGT_COLOR> vCol);				

		void InitializeDualCharacteristicLight(float fDuration1, float fEclipse1, float fDuration2, float fEclipse2												
												, char *pSignalGroup1, char *pSignalGroup2
												, float fSignalPeriod
												, std::vector<LGT_COLOR> vCol);
};



class  FI_SectorPulse
{

	public:		
		FI_SectorPulse();
		~FI_SectorPulse();

		void SetPulse(LGT_COLOR col, float dur);

		LGT_COLOR GetColor(){ return  m_Color; };
		float GetDuration(){ return  m_fDuration; };

	private:
		LGT_COLOR  m_Color;
		float 	   m_fDuration;				//UNIT : Sec
	
};


