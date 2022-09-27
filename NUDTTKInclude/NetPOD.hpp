#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "dynPODStructDef.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"

//  Copyright 2018, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::Math;

// �����Ǽ���·����������
namespace NUDTTK
{
	namespace NETPOD
	{
		// �Ӳ�ⲿ��
		struct NETPOD_CLKSOL
		{
			GPST    t;
			double  clock;
		};
		// �������ݽṹ, �����������ݺ��������ⲿ��
		struct NETPOD_StaDatum 
		{
			// ���벿��
			GPST      t0;                     // ��ʼʱ��
			GPST      t1;                     // ����ʱ��
            double    interval;               // ���ݲ������
			string    staName;
			string    pathFolder;
			size_t    count_MixedEpoch;    
			double    orbOutputInterval;
			bool      on_IsUsed_InitDynDatum;
			double    ArcLength_InitDynDatum;
			SatdynBasicDatum dynDatum_Init;     // ��ʼֵ����ѧ�����Ϣ
			double    period_SolarPressure;    // Ŀǰ�������Ǳ�ӹ��Ӧ���߶����, �ֶ�����Ķ������ͳһ

			vector<Matrix>              attMatrixList;
			vector<int>                 attFlagList;
			vector<POS3D>               attXYZAntList[3];
			vector<TimePosVel>          interpOrbitlist;     // ��ֵ����
			vector<Matrix>              interpRtPartiallist; // ��ֵƫ��������
			vector<TDT>                 interpTimelist;
			vector<TimePosVel>          acOrbitList;         // ���ڲ�ֵ������� getEphemeris
			vector<Matrix>              acRtPartialList;     // ���ڲ�ֵƫ�������� getInterpRtPartial

			// �������
			int                   n0_EstParameters;         // ��¼�ϲ�����������ķֿ����, ���ڷֿ����
			int                   count_EstParameters;      // �����Ӳ�: count_EstDynParameters + count_EstAmbiguity + count_EstRecPCO + count_EstSysBias
			int                   count_EstDynParameters;   // ����ѧ��������
			int                   count_EstClockParameters; // �Ӳ��������
			int                   n0_SolarPressure;         // �ֿ��ڲ�λ��, ��¼̫����ѹ������ʼλ��
			int                   n0_EmpiricalForce;        // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			SatdynBasicDatum      dynDatum_Est;  // ����ֵ����ѧ�����Ϣ
			vector<TimePosVel>    orbList; // ������ƽ��������Ԥ�����֣�
			vector<NETPOD_CLKSOL> clkList; // �Ӳ���ƽ��������ѧ��
			Matrix                matN_aa; // ����ѧ������ģ���ȡ�����ƫ�ơ�ϵͳƫ��
			Matrix                matN_a;
			Matrix                matda;
			//////////////////////////
			bool getEphemeris(TDT t, TimePosVel& interpOrbit, int nLagrange = 9);
			bool getEphemeris_ITRF(GPST t, TimePosVel& interpOrbit, int nLagrange = 9); // �ع�ϵ
			bool getInterpRtPartial(TDT t, Matrix& interpRtPartial);
			// ���¹���Ľ����
			void updateDynDatum();
			// �������Ľ����
			void writeDynDatum();
			// �в�༭
			void ocResEdit(float factor = 3.0f, bool flag_UseSingleFrequency = false);
			void ocResEdit_NET(float factor = 3.0f);
			// ���²в�
			void ocResOutput();
			// �������������ʼλ��: n0_SolarPressure��n0_AtmosphereDrag��n0_EmpiricalForce��n0_ManeuverForce��n0_RecPCO��n0_SysBias
			void getEstParameters_n0();
			//*******************************************************************************
			// ����Ӳ��ļ�
			void getRecClkFile(CLKFile& recClkFile); 

			NETPOD_StaDatum()
			{
				count_MixedEpoch = 0;
				orbOutputInterval = 30.0;
				on_IsUsed_InitDynDatum = false;
				ArcLength_InitDynDatum = 3600.0 * 3.0;
				period_SolarPressure  = 3600 * 24.0; 
			}
		};
		typedef map<string, NETPOD_StaDatum> StaDatumMap;

		// �����������ݽṹ, ��������
		struct NETBDS_StaDatum
		{
			GPST      t0;                     // ��ʼʱ��
			GPST      t1;                     // ����ʱ��
			string    staName;
			vector<TimePosVel>          pOrbitlist;     // �������
			//////////////////////////
			bool getEphemeris(TDT t, TimePosVel& interpOrbit, int nLagrange = 9);

		};
		typedef map<string, NETBDS_StaDatum> BDSDatumMap;

		struct NETPOD_KBRArcElement
		{   
			GPST   t;             // �۲�ʱ�����
			double obs;           // ԭʼKBR���, ����������ֵ
			double range_0;       // ������Ծ���ֵ
			double res;   
			double robustweight;  // ³�����Ƶ���Ȩ
			
			NETPOD_KBRArcElement()
			{
				robustweight = 1.0;
				res = 0.0;
			}
		};

		struct NETPOD_KBRArc 
		{
			double                       ambiguity;    
			vector<NETPOD_KBRArcElement> obsList;
		};

		struct NETPOD_DEF
		{
			bool          on_RecRelativity;
			float         max_ArcInterval;
			int           max_OrbIterationCount;     // ����Ľ�������ֵ	
            int           max_AmbNum;                // ����Ľ���ģ���ȹ̶��������ֵ 	
			unsigned int  min_ArcPointCount;         // ���ι۲������ֵ, ����С�ڸ���ֵ�Ļ��ν���ɾ��    
			unsigned int  min_SubSectionPoints;      // ���ӷֶ��������С����
			float         min_OrbPosIteration;     // ����Ľ�����Сֵ
			int           max_num_edit;           // ���ӱ༭����
			double        threshold_initDynDatumEst; // ����ȷ�����
			NETPOD_DEF()
			{
				on_RecRelativity          = true;
				max_OrbIterationCount     = 10;
				max_AmbNum                = 8;
				min_ArcPointCount         = 30;
				max_ArcInterval           = 2000.0f;
				min_SubSectionPoints      = 30;      // �ݶ�Ϊ30����Ч��, ���10���Ӳ����൱��5����
				min_OrbPosIteration       = 5.0E-3f;  // 5 mm
				threshold_initDynDatumEst = 300.0;
				max_num_edit              = 1;
			}
		};
				
		struct NETISLObsEpoch
		{
			DayTime                  t;
			string                   sat_A;
			string                   sat_B;
			double                   obs_AB;  
			double                   obs_BA; 
			double                   d;
			double                   clk_A;
			double                   clk_B; 

			//double                   obs_d; // ������obs
			//double                   obs_c; // �Ӳ���obs
			NETISLObsEpoch()
			{
				obs_AB = 0.0;
				//obs_d  = 0.0;
				//obs_c  = 0.0;
			}
			double getobs_d()
			{
				double obs_d = (obs_AB+obs_BA)/2;
				return obs_d;
			}
			double getobs_c()
			{
				double obs_c = (obs_AB-obs_BA)/2;
				return obs_c;
			}
		};
		// ��վ��վ���ݽṹ
		struct NETPOD_StaBaseline
		{
			string    staName_A; 
			string    staName_B;
			vector<NETISLObsEpoch>   m_data;
		};

		struct NETPOD_Sta_BDSBaseline
		{
			string    staName_A; 
			string    staName_BDS;
			vector<NETISLObsEpoch>   m_data;
		};

		class NetPOD: public SatdynBasic
		{
		public:
			NetPOD(void);
		public:
			~NetPOD(void);
		public:
			bool adamsCowell_Interp_Leo(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h = 10.0, int q = 11); 
            bool initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3); 
            void orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0); 
			void orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAcc> &forecastOrbList, double interval = 30.0); 
			void orbitExtrapolation_jerk(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAccJerk> &forecastOrbList, double interval = 30.0);
		public:
			bool adamsCowell_ac(TDT t0, TDT t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 10.0, int q = 11);
			bool mainNetPOD_InterSatLink(); // �����Ǽ��������������
		private:
			void updateNET_ISL();      // + ��ʼ��ISL��Ϣ, ���²в�
			void updateSta_AdamsCowell(StaDatumMap::iterator it_Sta); // + ���²�վ������Ϣ
			void updateNet_NEQ_SQL();
		public:
            double                         m_stepAdamsCowell;
			NETPOD_DEF                     m_podDefine;
			StaDatumMap                    m_mapStaDatum;          // �������� + 
			BDSDatumMap                    m_mapBDSDatum;          // BDS���� + 
			vector<NETPOD_StaBaseline>     m_staBaselineList;      // �����ⲿ���� + 
			vector<NETPOD_Sta_BDSBaseline> m_sta_BDSBaselineList;  // ���¿ռ䵼��������BDS��������
		};
	}
}
