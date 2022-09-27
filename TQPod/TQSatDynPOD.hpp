#pragma once
#include"structDef.hpp"
#include"SatdynBasic.hpp"
#include<map>
#include"TimePosVelFile.hpp"
#include"MathAlgorithm.hpp"
#include"TQUXBObsFile.hpp"
#include"RuningInfoFile.hpp"

// Copyright 2018, SUN YAT-SEN UNIVERSITY TianQin Research Center at ZhuHai
using namespace NUDTTK;
using namespace NUDTTK::Math;
using namespace NUDTTK::Geodyn;

namespace NUDTTK
{
	namespace TQPod
	{

        struct TQUXB_StaDatum
		{
			int                      id_Station;      // ��վ��
			string                   name;            // ��վ��
			POS3D                    pos_ITRF;        // ��վ��ITRF����µ�λ��
			map<UTC, TQUXBObsLine>   obsList;         // �����Ϣ�б�
			bool                     on_EstZeroDelay; // 
			double                   zeroDelay_0;     // ��վ��������ӳ�
			double                   zeroDelay_Est;   // Est
			int                      id_ZeroDelay;    //

            //TWRObsErrFile          obsErrFile;

			TQUXB_StaDatum()
			{
				on_EstZeroDelay = false;
				zeroDelay_0     = 0.0;
				zeroDelay_Est   = 0.0;
				id_ZeroDelay    = -1;
			}
		};
        
		struct TQUXBPOD_DEF
		{
			bool                     on_TropDelay;         // �Ƿ��Ƕ������ӳ�ЧӦ
			bool                     on_SolidTides;        // �Ƿ��ǹ��峱ЧӦ
			bool                     on_OceanTides;        // �Ƿ��Ǻ���ЧӦ
			bool                     on_GraRelativity;     // �Ƿ��ǹ��������ЧӦ
			bool                     on_SatPCO;            // ������λ�������
			bool                     on_ObsEditedInfo;     
			
			double                   on_DeleteData;        // �Ƿ�ɾ������ʱ�������
			double                   min_Elevation;        // ��С�߶Ƚ���ֵ
			double                   max_ocEdit;           // �в������ֵ
			double                   ratio_ocEdit;         // �в�༭����       
			int                      min_ArcPointCount;    // �۲����ݴ�����Сֵ
			int                      max_OrbIteration;     // ����Ľ��������ֵ
			bool                     on_EstZeroDelay;       
			double                   satZeroDelay_0;       // ��������ӳ�
			double                   satZeroDelay_Est;

			TYPE_SOLARPRESSURE_MODEL solarPressure_Model;        // ̫����ѹģ��
			double                   period_SolarPressure;       // ̫����ѹ����
			double                   period_EmpiricalAcc;        // ���������ٶ�����

			bool                     on_SRP9_D0;               // �Ƿ���ƹ�ѹD0����
			bool                     on_SRP9_DC1;              // �Ƿ���ƹ�ѹDC1����
			bool                     on_SRP9_DS1;              // �Ƿ���ƹ�ѹDS1����	
			bool                     on_SRP9_Y0;               // �Ƿ���ƹ�ѹY0����
			bool                     on_SRP9_YC1;              // �Ƿ���ƹ�ѹYC1����
			bool                     on_SRP9_YS1;              // �Ƿ���ƹ�ѹYS1����
			bool                     on_SRP9_B0;               // �Ƿ���ƹ�ѹB0����
			bool                     on_SRP9_BC1;              // �Ƿ���ƹ�ѹBC1����
			bool                     on_SRP9_BS1;              // �Ƿ���ƹ�ѹBS1����

			string                   nameDynPodFitFile;

            //bool                     on_DeleteManeuver;    // �Ƿ�ɾ������ǰ������   
			//bool                     on_WeightManeuver;    // �Ƿ�ʹ�û����ڼ��Ȩ
			//double                   weightManeuver;
			//double                   span_InitDynDatumCoarsePos; // ����ȷ�����   
			//double                   span_DeleteManeuver;        // ɾ������ǰ��ʱ����

			 TQUXBPOD_DEF()
			{
				on_TropDelay               = true;
				on_SolidTides              = true;
				on_OceanTides              = true;
				on_GraRelativity           = true;
				on_SatPCO                  = true;
				on_ObsEditedInfo           = false;
				on_DeleteData              = false;
				min_Elevation              = 10;
				max_ocEdit                 = 70.0;
				ratio_ocEdit               = 3.0;
				min_ArcPointCount          = 20;
				max_OrbIteration           = 10;
			
				period_SolarPressure       = 3600.0 * 24.0;
				period_EmpiricalAcc        = 3600.0 * 24.0;
				solarPressure_Model        = TYPE_SOLARPRESSURE_9PARA; 
				on_EstZeroDelay            = false;
				satZeroDelay_0             = 0.0;
				satZeroDelay_Est           = 0.0;
			
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

				//on_DeleteManeuver          = false;
				//on_WeightManeuver          = false;
				//span_InitDynDatumCoarsePos = 300.0; // 5����
				//span_DeleteManeuver        = 0.5 * 3600.0; // ��Сʱ
				//span_DeleteManeuver        = 0.5 * 3600.0; // ��Сʱ
			}
		};

		class TQSatDynPOD : public SatdynBasic
		{
		public:
			TQSatDynPOD(void);
		public:
			~TQSatDynPOD(void);
		public:
			bool    adamsCowell_ac(UTC t0, UTC t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 60.0, int q = 11);			
		    bool    dynamicTQPOD_pos(vector<TimePosVel> obsOrbitList, SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
			bool    mainTQPOD(SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
		private:
			bool    getEphemeris(UTC t, TimePosVel& tqOrb, int nLagrange = 9);
			bool    getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& tqOrb, Matrix& tqRtPartial, double threshold = 1.0E-07);
		private:
			vector<TimePosVel>           m_acOrbitList;           // ���ڲ�ֵ�������
			vector<Matrix>               m_acRtPartialList;       // ���ڲ�ֵƫ��������
		public:
			string                       m_strTQPODPath;
			TQUXBPOD_DEF                 m_tqUXBDefine;
			POS3D                        m_pcoAnt;                // ����ƫ����Ϣ
			map<string, TQUXB_StaDatum>  m_staDatumList;          // ��վ�۲���Ϣ
			TimePosVelFile               m_orbJ2000File_0;
			TimePosVelFile               m_orbECEFFile_0;
		};
	}
}
