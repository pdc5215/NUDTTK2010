#pragma once
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include <time.h>
#include "structDef.hpp"
#include "dynPODStructDef.hpp"
#include "MathAlgorithm.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace NETPOD
	{
		struct NETISLObsEpoch_simu
		{
			DayTime                  t;
			string                   sat_A;
			string                   sat_B;
			double                   obs_AB;  
			double                   obs_BA; 
			double                   d;
			double                   clk_A;
			double                   clk_B; 
		};

		// �������ݽṹ, �����������������
		struct NETISL_StaDatum 
		{
			// ���벿��
			GPST      t0;
			GPST      t1;
			string    staName;
			vector<TimePosVel>        m_pvOrbList; //               ����ļ�
			double    orbOutputInterval;
			double    clk_bias;       // ƫ��
			double    clk_biasdraft; // ��Ư

			bool getEphemeris(GPST t, TimePosVel& interpOrbit, int nLagrange = 9);

			NETISL_StaDatum()
			{
				clk_bias = 1E-7;
				clk_biasdraft = 1E-10;
				orbOutputInterval = 30.0;
			}
		};
		typedef map<string, NETISL_StaDatum> StaDatumMap_simu; 

		// ��վ��վ���ݽṹ
		struct NETISL_StaBaseline
		{
			// ����
			string    staName_A; 
			string    staName_B;
			// ���
			vector<NETISLObsEpoch_simu>   m_data; // 

			NETISL_StaBaseline()
			{
			}
		};

		// ��������
		struct NETISLObsSimu_DEF
		{
			double        system_err; // ���ϵͳ���
			double        noise_err; // ���������
			double        beta_1; // MEO�����ڵ���
			double        beta_2; // GEO��IGSO�����ڵ���
			double        alpha;  // ka����ɨ�跶Χ�������ǡ�60��
			double        intervel; // �������
			NETISLObsSimu_DEF()
			{
				system_err = 0.5;
				noise_err  = 0.5;
				beta_1 = 12.9;
				beta_2 = 8.7;
				alpha = 60.0;
				intervel = 600.0;
			}
		};

		class NETISLObsimu
		{
		public:
			NETISLObsimu(void);
		public:
			~NETISLObsimu(void);
		public: 
			bool  main_obssimu();
			bool  main_EMobssimu();
			bool  main_EM_BDSobssimu();
			bool  main_EM_GEOobssimu();  //  20220620
			bool  main_BDS_EMobssimu();  //  20220620
		public:
			NETISLObsSimu_DEF              m_simuParaDefine;
			JPLEphFile				       m_JPLEphFile;       // JPL DE405 ���������ļ�
			TimeCoordConvert		       m_TimeCoordConvert;  // ʱ������ϵת��
			StaDatumMap_simu               m_staDatumMap;
			vector<NETISL_StaBaseline>     m_staBaselineList;      // �����ⲿ���� + 
		};
	}
}
