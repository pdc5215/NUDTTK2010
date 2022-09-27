#pragma once
#include"structDef.hpp"
#include"SatdynBasic.hpp"
#include<map>
#include"TimePosVelFile.hpp"
#include"MathAlgorithm.hpp"
#include"TQUXBObsFile.hpp"
#include"TQISLObsFile.hpp"
#include"SLRSatDynPOD.hpp"
#include"RuningInfoFile.hpp"

// Copyright 2018, SUN YAT-SEN UNIVERSITY TianQin Research Center at ZhuHai
using namespace NUDTTK;
using namespace NUDTTK::Math;
using namespace NUDTTK::Geodyn;

// �����+�����࣬��������2022/03/15

namespace NUDTTK
{
	namespace TQPod
	{
		// ͳһS/X���Σ�USB/UXB����������, ����or˫�̣� ���ӻ��θ��
		struct TQNETPOD_UXBStaDatum
		{
			string                   nameSta;         // ��վ�� "XXXX"
            string                   nameSat;         // ������ "TQ01" +
			POS3D                    pos_ITRF;        // ��վ��ITRF����µ�λ��
			map<UTC, TQUXBObsLine>   obsList;         // �۲������б�

			double                   zeroDelay_0;     // ��վ������ֵ�ӳ�
			double                   zeroDelay_Est;   // ��վ������ֵ����ֵ
			int                      id_ZeroDelay;    // �±���㿪ʼ
			bool                     on_EstStaZeroDelay;
			// ע: ���Ƕ���ʱ��ͬһ��վ����ֵ�ӳٹ���һ����������ʱ�������Լ��
			TQNETPOD_UXBStaDatum()
			{
				zeroDelay_0        = 0.0;
				zeroDelay_Est      = 0.0;
				id_ZeroDelay       = -1;
				on_EstStaZeroDelay = true;
			}
		};
		typedef map<string, TQNETPOD_UXBStaDatum> TQNETPOD_UXBStaDatumMap;

		// SLR �������ݸ�ʽ�����壿ͳһ����UXB�۲����ݶ������ƣ�




		struct TQNETPOD_SatDatum
		{
			// ���벿��
			string                      satName;
			TQNETPOD_UXBStaDatumMap     mapUXBStaDatum;
			UTC                         t0;
			UTC                         t1;
			TimePosVelFile              orbJ2000File_0;           // ���������� +��J2000��
			SatdynBasicDatum            dynDatum_Init;            // ��ʼֵ����ѧ�����Ϣ
			TYPE_SOLARPRESSURE_MODEL    solarPressure_Model;      // ̫����ѹģ��
			double                      period_SolarPressure;     // ̫����ѹ����
			double                      period_EmpiricalAcc;      // ���������ٶ�����
			bool                        on_SRP9_D0;               // �Ƿ���ƹ�ѹD0����
			bool                        on_SRP9_DC1;              // �Ƿ���ƹ�ѹDC1����
			bool                        on_SRP9_DS1;              // �Ƿ���ƹ�ѹDS1����	
			bool                        on_SRP9_Y0;               // �Ƿ���ƹ�ѹY0����
			bool                        on_SRP9_YC1;              // �Ƿ���ƹ�ѹYC1����
			bool                        on_SRP9_YS1;              // �Ƿ���ƹ�ѹYS1����
			bool                        on_SRP9_B0;               // �Ƿ���ƹ�ѹB0����
			bool                        on_SRP9_BC1;              // �Ƿ���ƹ�ѹBC1����
			bool                        on_SRP9_BS1;              // �Ƿ���ƹ�ѹBS1����
			bool                        on_EstSatZeroDelay;       // �Ƿ������������ӳٲ���
			double                      satZeroDelay_0;           // ��������ӳ�
            
			// ������Ϣ
			vector<TimePosVel>          acOrbitList;              // ���ڲ�ֵ������� getEphemeris
			vector<Matrix>              acRtPartialList;          // ���ڲ�ֵƫ�������� getInterpRtPartial
			
			// �������
			int                         n0_EstParameters;         // ��¼�ϲ�����������ķֿ����, ���ڷֿ����
			int                         count_EstParameters; 
			int                         count_EstDynParameters;
			int                         count_EstSatZeroDelay;
			int                         count_EstStaZeroDelay;
			int                         n0_SolarPressure;         // �ֿ��ڲ�λ��, ��¼̫����ѹ������ʼλ��
			int                         n0_EmpiricalForce;        // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			int                         n0_ManeuverForce;         // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			SatdynBasicDatum            dynDatum_Est;             // ����ֵ����ѧ�����Ϣ
			vector<TimePosVel>          orbList;                  // ������ƽ��������Ԥ�����֣�
			double                      satZeroDelay_Est;

            // �в���Ϣ
			int                         count_obs;
			double                      ocResRMS_Range;
			double                      ocResRMS_Doppler;
            // ��������Ϣ
			Matrix                      n_xx; 
			Matrix                      nx;
			Matrix                      dx;

			bool getEphemeris(UTC t, TimePosVel& interpOrbit, int nLagrange = 9);
			bool getInterpRtPartial(UTC t, Matrix& interpRtPartial);
		    bool getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& tqOrb, Matrix& tqRtPartial, double threshold = 1.0E-8);
			
			// �������������ʼλ��: n0_SolarPressure��n0_EmpiricalForce��n0_ManeuverForce
			void getEstParameters_n0();

			// ���²в�
			void ocResOutput();

			// �в�༭
			void ocResEdit(double factor);

			// ���¹���Ľ����
			void updateDynDatum();

			// �������Ľ����
			void writeDynDatum(string pathDynPodFitFile);

			TQNETPOD_SatDatum()
			{
				period_SolarPressure       = 3600.0 * 24.0;  // ���������ʱ�Ĺ�ѹ����һ��
				period_EmpiricalAcc        = 3600.0 * 24.0;
				solarPressure_Model        = TYPE_SOLARPRESSURE_9PARA; 
				on_EstSatZeroDelay         = false;
				satZeroDelay_0             = 0.0;
				satZeroDelay_Est           = 0.0;
				count_obs                  = 0;
				ocResRMS_Range             = 0.0;
				ocResRMS_Doppler           = 0.0;

				on_SRP9_D0                 = true;
				on_SRP9_DC1                = true;
				on_SRP9_DS1                = true;
				on_SRP9_Y0                 = true;
				on_SRP9_YC1                = true;
				on_SRP9_YS1                = true;
				on_SRP9_B0                 = true;
				on_SRP9_BC1                = true;
				on_SRP9_BS1                = true;
			}
		};

		typedef map<string, TQNETPOD_SatDatum> TQNETPOD_SatDatumMap;

		// �Ǽ���·����
		struct TQNETPOD_ISLArcElement
		{   
			GPST   t;             // �۲�ʱ�����
			double obs_code;      // ԭʼ���, ����������ֵ
			double obs_phase;     // ԭʼ���, ����������ֵ
			double range_0;       // ������Ծ���ֵ
			double oc_code;   
			double oc_phase;  
			double rw_code;       // ³�����Ƶ���Ȩ
			double rw_phase;
			
			TQNETPOD_ISLArcElement()
			{
				rw_code  = 1.0;
				rw_phase = 1.0;
				oc_code  = 0.0;
				oc_phase = 0.0;
			}
		};

		typedef map<UTC, TQNETPOD_ISLArcElement> ISLArcElementMap;

		struct TQNETPOD_ISLArc
		{
			double                         ambiguity;    // ��ģ����
			vector<TQNETPOD_ISLArcElement> obsList;
		};

		// �Ǽ���·���ݽṹ
		struct TQNETPOD_InterSatLink
		{
			string    satName_A;
			string    satName_B;
			// ��һ�����ص㿼�������Ǽ���߼���������Լ�� ?????????
			//ISLFile                   tqISLFile;
			vector<TQNETPOD_ISLArc>     ISLArcList;
			TQISLObsFile                tqISLFile;

			// �������
			int                         n0_ISLAmbiguity;
		};


		struct TQNETPOD_DEF
		{
			// ���ݵı༭��Ϣͳһ�ŵ�Ԥ������
			bool                     on_TropDelay;         // �Ƿ��Ƕ������ӳ�ЧӦ
			bool                     on_SolidTides;        // �Ƿ��ǹ��峱ЧӦ
			bool                     on_OceanTides;        // �Ƿ��Ǻ���ЧӦ
			bool                     on_GraRelativity;     // �Ƿ��ǹ��������ЧӦ
			bool                     on_SatPCO;            // ������λ�������
			bool                     on_ObsEditedInfo;  
			bool                     on_ResEdit;
			bool                     on_EstStaZeroDelay; 
			bool                     on_Used_Range;        // �Ƿ�ʹ�ò�����ݶ���
			bool                     on_Used_Doppler;      // �Ƿ�ʹ�ò������ݶ���
			bool                     on_Used_ISL_Code;     // Inter satellite link, �Ǽ���·α��
			bool                     on_Used_ISL_Phase;    // Inter satellite link, �Ǽ���·��λ
			double                   min_Elevation;        // ��С�߶Ƚ���ֵ
			double                   max_Edit_OcRange;     // ���в������ֵ�������޳�һЩ�в��ĵ�
			double                   max_Edit_OcDoppler;   // ���ٲв������ֵ // ��ʱ��� 
			double                   ratio_ocEdit;         // �в�༭����  
			int                      min_ArcPointCount;    // �۲����ݴ�����Сֵ
			int                      max_OrbIteration;     // ����Ľ��������ֵ
			string                   nameDynPodFitFile;
			double                   min_OrbPosIteration;  // ����Ľ�����Сֵ
			double                   weight_doppler;       // �Բ��ټ�Ȩ�������̼�Ȩ
			double                   weight_code;          // ��α���Ȩ
			double                   weight_phase;         // ����λ��Ȩ

			 TQNETPOD_DEF()
			{
				on_TropDelay               = true;
				on_SolidTides              = true;
				on_OceanTides              = true;
				on_GraRelativity           = true;
				on_SatPCO                  = true;
				on_ResEdit                 = true;
				on_EstStaZeroDelay         = false;
				min_Elevation              = 10;
				max_Edit_OcRange           = 1000.0;
				max_Edit_OcDoppler         = 1.0;           // ������ ���Ը�СһЩ ���ٴ���
				on_ObsEditedInfo           = false;
				ratio_ocEdit               = 3.0;
				min_ArcPointCount          = 0;
				max_OrbIteration           = 10;
				on_Used_Range              = true;
				on_Used_Doppler            = true;
				nameDynPodFitFile          = "dynpod.fit";
				min_OrbPosIteration        = 1.0E-1;
				weight_doppler             = 1.0;
				weight_code                = 1.0;
				weight_phase               = 1.0;
				on_Used_ISL_Code           = false;
				on_Used_ISL_Phase          = false;
			}
		};

		class TQSatNetDynPOD : public SatdynBasic
		{
		public:
			TQSatNetDynPOD(void);
		public:
			~TQSatNetDynPOD(void);
		public:
			bool    adamsCowell_ac(UTC t0, UTC t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 60.0, int q = 11);
			bool    initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3, double h = 75.0);
			bool    dynamicPOD_pos(vector<TimePosVel> obsOrbitList, TQNETPOD_SatDatumMap::iterator it_Sat, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
		    void    orbitExtrapolation(SatdynBasicDatum dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel>    &forecastOrbList, double interval = 30.0);
			void    orbitExtrapolation(SatdynBasicDatum dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVelAcc> &forecastOrbList, double interval = 30.0);
			bool    mainNetPOD(UTC t0, UTC t1, double interval = 30.0);
		private:
			double  m_stepAdamsCowell;  // ������ֲ���ͳһ�޸�
			void    updateSat_Obs(TQNETPOD_SatDatumMap::iterator it_Sat); // + ��ʼ���۲����� 
			void    updateSat_AdamsCowell(TQNETPOD_SatDatumMap::iterator it_Sat); // + ���»�����Ϣ
			void    updateSat_NEQ(TQNETPOD_SatDatumMap::iterator it_Sat); // + ���·�����
            void    updateSat_SOL(TQNETPOD_SatDatumMap::iterator it_Sat); // + �������ǽ�
			void    updateNET_ISLArc(TQNETPOD_InterSatLink &satISL);      // + ��ʼ��ISL������Ϣ, ���²в�
			void    updateNet_NEQ_SQL();                                  // + �������������̺�����
			void	updateISLResEdit(double factor = 3.0); // + �����Ǽ�����Ϣ�в�༭  ���Դ��룺2019/11/22  ���
		public:
			string                         m_tqSatNetDynPODPath;
			TQNETPOD_DEF                   m_tqNetPODDefine;
			TQNETPOD_SatDatumMap           m_mapSatDatum; // �������� +
			vector<TQNETPOD_InterSatLink>  m_ISLList;     // �Ǽ���·���� + 
		};
	}
}
