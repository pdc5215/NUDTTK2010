#pragma once
#include "TQUXBObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "TimeCoordConvert.hpp"
#include "RuningInfoFile.hpp"
#include <time.h>

using namespace NUDTTK;
using namespace NUDTTK::Math;

// Copyright 2018, SUN YAT-SEN UNIVERSITY TianQin Research Center at ZhuHai
namespace NUDTTK
{
	namespace TQPod
	{
		// ��վ��Ϣ
		struct TQStaInfo
		{
			string       name;         // ����
			BLH          pos_blh;       // ��վ�ľ�γ��
			POS3D        pos;          // ��վλ����Ϣ
			ENU          pcoAnt;       // ����ƫ����Ϣ
			double       StaZeroDelay; // ��վϵͳƫ��

			double       mu;           // ��ʱ��� ���Դ��� �������
			double       sigma;


			TQStaInfo()
			{
				StaZeroDelay  = 0.0;
				mu            = 0.0;
				sigma         = 0.0;
			}
		};

		// ������Ϣ
		struct TQSatInfo
		{
			string             satName;  // ��������
			POS3D              pcoAnt;   // ����ƫ����Ϣ
			vector<TimePosVel> orbList;  // ���ǹ���б�

			double             SatZeroDelay;
			int                JMS; // JMS��վ�۲�������
			int                KASH;
			int                NEUQ;

			TQSatInfo()
			{
				SatZeroDelay = 0.0;
				JMS          = 0;
				KASH         = 0;
				NEUQ         = 0;
			}

			bool getEphemeris(UTC t, TimePosVel& satOrb, int nLagrange = 9);
			bool getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& satOrb,  double threshold = 1.0E-8); // �������������źŷ����ʱ��
		};

		struct TQOBSSIMU_DEF
		{
			bool                     on_TropDelay;          // �Ƿ������������ӳ�
			bool                     on_SolidTides;         // �Ƿ��ǹ��峱ЧӦ
			bool                     on_OceanTides;         // �Ƿ��Ǻ���ЧӦ
			bool                     on_GraRelativity;      // �Ƿ��ǹ��������ЧӦ
			bool                     on_SatPCO;             // �Ƿ���������λ����ƫ��
			bool                     on_RecClock;           // �Ƿ��ǽ��ջ��Ӳ�
			bool                     on_ObsRandnNoise;      // �Ƿ���ӹ۲�����������
			bool                     on_StaZeroDelayNoise;  // �Ƿ���Ӳ�վϵͳƫ��
			bool                     on_SatZeroDelayNoise;  // �Ƿ��������ϵͳƫ��
			bool                     on_Day;                // �����Ƿ�ɼ������ڼ��������
			double                   min_Elevation;         // ��С�߽Ƕ�
			double                   min_DopplerIntergTime; // ��С�����ջ���ʱ��

			double                   r_Noise;         // λ������
			double                   v_Noise;         // �ٶ�����

			TQOBSSIMU_DEF()
			{
				on_TropDelay          = false;
				on_SolidTides         = false;
				on_OceanTides         = false;
				on_GraRelativity      = true;
				on_SatPCO             = true;	
				on_ObsRandnNoise      = false;
				on_StaZeroDelayNoise  = false;
				on_SatZeroDelayNoise  = false;
				on_Day                = false;
				min_Elevation         = 10;
                min_DopplerIntergTime = 5;
				r_Noise               = 0.01;
				v_Noise               = 0.0;
			}
		};

		class TQObsSimu
		{
		public:
			TQObsSimu(void);
		public:
			~TQObsSimu(void);
		public:
			bool   judgeUXBSignalCover(POS3D posSta, POS3D posSat, double cut_elevation = 10);
			bool   simuUXBObsFile(TQUXBObsFile &obsFile, UTC t0, UTC t1, string staname, string satname, double h = 10.0, double DopplerTime0 = 10.0);
			bool   judgeUXBElevation(UTC t_Receive, POS3D posSta, double& elevation1, double& elevation2);//����߶Ƚ�
		public:
			TQOBSSIMU_DEF       m_simuDefine;
			TimeCoordConvert    m_TimeCoordConvert; // ʱ������ϵת��
			TQStaInfo           m_staInfo; 
			TQSatInfo           m_satInfo;
			vector<TQSatInfo>   m_satInfoList; // �Ա�����̽�����ĸ߶Ƚǣ�����ǰ���������վ�ĸ߶Ƚ����������Ӧʱ�̵����ݣ�����������
		};
	}
}
