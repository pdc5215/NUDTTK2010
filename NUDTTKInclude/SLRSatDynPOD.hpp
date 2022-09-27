#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "MathAlgorithm.hpp"
#include "SLROrbitComparison.hpp"
#include "RuningInfoFile.hpp"

//  Copyright 2020, Sun Yat-sen University at GuangZhou
using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::Math;

namespace NUDTTK
{
	namespace SLR
	{
		struct SLRPOD_ObsElement
		{
			unsigned int id;                    // ��վ�������������վַ¼�еı��
			POS3D        staPos_ECEF;           // ��վλ��, ���ڷ������
			POS3D        staPos_J2000;          // ��վλ��, Tsʱ��
			BLH          staBLH;
			UTC          Ts;                    // �����źŷ���ʱ��
			double       obs;                   // ���̼�����ֵ��ԭʼ�۲����ݣ�
			double       wavelength;
			double       temperature;
			double       pressure;
			double       humidity;
			ENU          ecc;

			// �Ӽ�����
			UTC          Tr;                    // �����źŷ���ʱ��
			double       obscorrected_value;    // ���̼�������ֵ
			double       r_mean;                // ������λ�ü���������о����ƽ��ֵ
			TimePosVel   satOrb;
			TimePosVel   satOrb_ECEF;
			Matrix       satRtPartial;
			double       Elevation;
			double       dR_correct_Trop;       // ���������
			double       dR_correct_Relativity; // ����۸���
			double       dR_correct_SatMco;     // �������ĸ��� Mass Center Correct
			double       dR_correct_StaEcc;     // ��վƫ�ĸ���
			double       dR_correct_Tide;       // ��ϫ����

			// ���ܶ�����ز���
			double       weight;
			double       oc; 
			double       rw;  

			SLRPOD_ObsElement()
			{
				dR_correct_Trop       = 0;
				dR_correct_Relativity = 0;
				dR_correct_Tide       = 0;
				dR_correct_StaEcc     = 0;
				dR_correct_SatMco     = 0;

				weight                = 1.0;
				rw                    = 1.0;
			}
		};

		struct SLRPOD_ObsArc
		{
			double rms_oc;
			int count_valid;
			vector<SLRPOD_ObsElement> obsList;

			SLRPOD_ObsArc()
			{
				count_valid = 0;
				rms_oc = 0.0;
			}
		};

		struct SLRPOD_DEF
		{
			bool                     on_TropDelay;               // �Ƿ��Ƕ������ӳ�ЧӦ
			bool                     on_Tides;                   // �Ƿ��ǹ��峱ЧӦ
			bool                     on_GraRelativity;           // �Ƿ��ǹ��������ЧӦ
			bool                     on_SatPCO;                  // ������λ�������
			bool                     on_StaEcc;                  // ��վƫ��
			bool                     on_YawAttitudeModel;        // ��̬ƫ������ģʽ��Ĭ�Ϲر�
			double                   min_Elevation;              // ��С�߶Ƚ���ֵ
			double                   max_ocEdit;                 // �в������ֵ�������޳�һЩ�в��ĵ�
			double                   ratio_ocEdit;               // �в�༭����       
			int                      min_ArcPointCount;          // �۲����ݴ�����Сֵ
			int                      max_OrbIteration;           // ����Ľ��������ֵ
			double                   threshold_max_adjustpos;
			double                   period_SolarPressure;       // ̫����ѹ����
			double                   period_AtmosphereDrag;      // ������������
			double                   period_EmpiricalAcc;        // ���������ٶ�����
			bool                     on_SRP9_D0;                 // �Ƿ���ƹ�ѹD0����
			bool                     on_SRP9_DC1;                // �Ƿ���ƹ�ѹDC1����
			bool                     on_SRP9_DS1;                // �Ƿ���ƹ�ѹDS1����	
			bool                     on_SRP9_Y0;                 // �Ƿ���ƹ�ѹY0����
			bool                     on_SRP9_YC1;                // �Ƿ���ƹ�ѹYC1����
			bool                     on_SRP9_YS1;                // �Ƿ���ƹ�ѹYS1����
			bool                     on_SRP9_B0;                 // �Ƿ���ƹ�ѹB0����
			bool                     on_SRP9_BC1;                // �Ƿ���ƹ�ѹBC1����
			bool                     on_SRP9_BS1;                // �Ƿ���ƹ�ѹBS1����
			string                   nameDynPodFitFile;
			double                   span_InitDynDatumCoarsePos;

			SLRPOD_DEF()
			{
				on_TropDelay               = true;
				on_Tides                   = true;
				on_GraRelativity           = true;
				on_SatPCO                  = true;
				on_StaEcc                  = true;
				on_YawAttitudeModel        = false;
				min_Elevation              = 10;
				max_ocEdit                 = 10.0; 
				ratio_ocEdit               = 4.0;
				min_ArcPointCount          = 0; 
				max_OrbIteration           = 10;
				period_SolarPressure       = 3600.0 * 24.0;
				period_AtmosphereDrag      = 3600.0 * 24.0;
				period_EmpiricalAcc        = 3600.0 * 24.0;
				threshold_max_adjustpos    = 0.02;
				//solarPressure_Model        = TYPE_SOLARPRESSURE_1PARA; 
				on_SRP9_D0                 = true;
				on_SRP9_DC1                = true;
				on_SRP9_DS1                = true;
				on_SRP9_Y0                 = true;
				on_SRP9_YC1                = true;
				on_SRP9_YS1                = true;
				on_SRP9_B0                 = true;
				on_SRP9_BC1                = true;
				on_SRP9_BS1                = true;
				nameDynPodFitFile          = "dynpod.fit";
				span_InitDynDatumCoarsePos = 300;
			}
		};

		class SLRSatDynPOD : public SatdynBasic
		{
		public:
			SLRSatDynPOD(void);
		public:
			~SLRSatDynPOD(void);
		public:
			bool getStaPosvel(UTC t, int id, POS6D& posvel);
			bool getStaEcc(UTC t, int id, ENU& ecc);
		public:
			void    setStepAdamsCowell(double step);
			void    weighting_Elevation(double Elevation, double& weight);
			bool    adamsCowell_ac(UTC t0, UTC t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 10.0, int q = 11);
			bool    initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3, double h = 10.0);
			bool    dynamicPOD_pos(vector<TimePosVel> obsOrbitList,  SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
			bool    mainPOD(string strObsFileName, int nObsFileType, SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
		private:
			bool    getEphemeris(UTC t, TimePosVel& satOrb, Matrix& satRtPartial, int nLagrange = 9);
		private:
			double                           m_stepAdamsCowell; 
			vector<TimePosVel>               m_acOrbitList;            // ���ڲ�ֵ�������
			vector<Matrix>                   m_acRtPartialList;        // ���ڲ�ֵƫ��������

		public:
			SLRPOD_DEF                       m_podDefine;
			string                           m_strPODPath;
			vector<SLRPOD_ObsArc>            m_obsArc;

			// ��������Ԥ������������
			vector<StaEccRecord>             m_staEccList;             // �����վ��ƫ����Ϣ
			vector<StaSscRecord>             m_staSscList;             // �����վ��������Ϣ
			POS3D                            m_mcoLaserRetroReflector; // ���ⷴ��������ƫ��
			StaOceanLoadingDisplacementFile  m_staOldFile;             // ��վ�ĺ����������λ�ļ�( http://www.oso.chalmers.se/~loading/ ��վ�ṩ )
			double                           m_constRangeBias;         // �̶�ƫ��У��, �����ⲿ����, Ĭ��Ϊ 0
			bool                             m_bChecksum;
		};
	}
}
