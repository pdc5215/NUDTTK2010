#pragma once
#include "TQISLObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "TimeCoordConvert.hpp"
#include <time.h>

using namespace NUDTTK;
using namespace NUDTTK::Math;

namespace NUDTTK
{
	namespace TQPod
	{
		// ������Ϣ
		struct TQSatISLInfo
		{
			string             satName;  // ��������
			POS3D              pcoAnt;   // ����ƫ����Ϣ
			vector<TimePosVel> orbList;  // ���ǹ���б�

			bool getEphemeris(UTC t, TimePosVel& satOrb, int nLagrange = 9);
			//bool getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& satOrb,  double threshold = 1.0E-07); // �������������źŷ����ʱ��
		};

		struct TQISLOBSSIMU_DEF
		{
			bool                     on_SatPCO;             // �Ƿ���������������ƫ��
			bool                     on_LightTime;          // �Ƿ��ǹ���ʱ

			bool                     on_GraRelativity;      // �Ƿ��ǹ��������

			bool                     on_ISLCodeSysBias;  // α��ϵͳ���             
			bool                     on_ISLPhaseSysBias;
			bool                     on_ISLCodeRandnNoise;   //�������
			bool                     on_ISLPhaseRandnNoise;

			double                   WAVELENGTH; // ���Ⲩ��


			TQISLOBSSIMU_DEF()
			{
				on_SatPCO             = false;
				on_LightTime          = false;
				on_ISLCodeSysBias     = false;
				on_ISLPhaseSysBias    = false;
				on_ISLCodeRandnNoise  = false;
				on_ISLPhaseRandnNoise = false;
				WAVELENGTH            = 1064 * 10E-9;  // ���Ⲩ��1024����
			}
		};

		class TQISLObsSimu
		{
		public:
			TQISLObsSimu(void);
		public:
			~TQISLObsSimu(void);
		public:
			bool   simuISLObsFile(TQISLObsFile &obsFile, UTC t0, UTC t1, double h = 60.0 * 30);// Ĭ�ϲ������30����

		public:
			TQISLOBSSIMU_DEF    m_simuDefine;
			TimeCoordConvert    m_TimeCoordConvert; // ʱ������ϵת��
			TQSatISLInfo        m_satInfoA;
			TQSatISLInfo        m_satInfoB;
			//vector<TQSatInfo>   m_satInfoList;
		};
	}
}
