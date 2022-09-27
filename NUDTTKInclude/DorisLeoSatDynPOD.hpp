#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "dynPODStructDef.hpp"
#include "Doris2_2_EditedObsFile.hpp"
#include "DorisSTCDFile.hpp"
#include <map>

using namespace NUDTTK::Geodyn;
namespace NUDTTK
{
	namespace DORIS
	{
		struct DorisLeoSatDynPODPara
		{
	    	unsigned int                 min_arcpointcount;        // ��С���������, ����С�� min_count_arcpoint �Ļ��ν���ɾ��
			double                       max_arclengh;             // �����������μ����ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������
			TYPE_SOLARPRESSURE_MODEL     solarPressureType;        // ̫����ѹģ������
			TYPE_ATMOSPHEREDRAG_MODEL    atmosphereDragType;       // ��������ģ������
			double                       period_SolarPressure;  
			double                       period_AtmosphereDrag;
			double                       period_EmpiricalAcc;
			int                          max_OrbitIterativeNum;     // ����Ľ����������ֵ, ͳһ���й涨
			double                       apriorityRms_obs;          // ����۲�ֵ����
			bool                         bOnEst_StaTropZenithDelay; // �Ƿ���в�վ�������춥�ӳٹ���
			double                       apriorityRms_TZD;          // �����վ������춥�ӳ�һ��Gauss-Markov���̾���, ���ڵ�����춥�ӳ�Լ�����̼�Ȩ����
			bool                         bOnEst_ERP;                // �Ƿ���е�����ת��������
			double                       apriorityRms_xp;           // ��λ: ����, 7.84E-7
			double                       apriorityRms_xpDot;        // ��λ: ����/��, 2.80E-13
			double                       apriorityRms_yp;           // ��λ: ����, 7.84E-7
			double                       apriorityRms_ypDot;        // ��λ: ����/��, 2.80E-13
			double                       apriorityRms_ut1;          // ��λ: ��,  2.156E-8
			double                       apriorityRms_ut1Dot;       // ��λ: ��/��, 3.470E-8
			
			DorisLeoSatDynPODPara()
			{	
				min_arcpointcount         = 5;
				max_arclengh              = 2000.0;
				solarPressureType         = TYPE_SOLARPRESSURE_1PARA;
				atmosphereDragType        = TYPE_ATMOSPHEREDRAG_J71_GEODYN;
				period_AtmosphereDrag     = 3600 * 24.0;
				period_SolarPressure      = 3600 * 24.0; 
				period_EmpiricalAcc       = 3600 *  6.0;
				max_OrbitIterativeNum     = 8;
				bOnEst_StaTropZenithDelay = false;
				bOnEst_ERP                = false;
				apriorityRms_TZD          = 0.1;
				apriorityRms_obs          = 0.0005 * 10; // 0.5mm/s * 10s
				// �ο� ign07wd01.eop.dsc
				// ERP | x & y pole |
				// Current IERS C04 values used as apriori |
				// estimated parameters: XPOLE, YPOLE, XPOLERATE, YPOLERATE |
				// a priori constraints: X,Y POLE 7.84D-7 !5 m |
				// X,Y POLE RATE 2.80D-13 ! 5 masec/day |
				// UT1-UTC,UT1-UTCrate |
				// UT1-UTC 2.156D-8 ! 0.01 mm |
				// UT1-UTC rate 3.470D-8 ! 3 msec/day |
				apriorityRms_xp           = 7.84E-7;     
				apriorityRms_xpDot        = 2.80E-13;
				apriorityRms_yp           = 7.84E-7;
				apriorityRms_ypDot        = 2.80E-13;
				apriorityRms_ut1          = 2.156E-8;
				apriorityRms_ut1Dot       = 3.470E-8;
			}
		};

		struct DorisObsEqEpochElement
		{
			double  obscorrected_value;
			double  duration;                // �����ֶ�Ӧ�ĳ���ʱ��, ��λ��
			POS6D   vecLos_t0;               // ����ʸ��-��������ʼʱ��
            POS6D   vecLos_t1;               // ����ʸ��-��������ֹʱ��
			Matrix  interpRtPartial_t0;      // ��ֵƫ��������
			Matrix  interpRtPartial_t1;
			double  weight;                  // �۲�Ȩֵ
			double  elevation_sat;           // ���ǹ۲�߶Ƚ�
			double  azimuth_sat;             // ���ǹ۲ⷽλ��
			double  elevation_sta_t0;        // ��վ��������-�����ʼʱ��
			double  elevation_sta_t1;        // ��վ��������-�����ֹʱ��
			TAI     t0_sta_transmit;         // ��վ����ʱ��-�����ʼʱ��
			TAI     t1_sta_transmit;         // ��վ����ʱ��-�����ֹʱ��
		};

		typedef map<int, DorisObsEqEpochElement> DorisPODEpochStationMap;

		struct DorisPODEpoch
		{
			int                         eyeableStaCount;
			TAI                         t;
			DorisPODEpochStationMap     mapDatum;
		};

		struct DorisObsEqArc
		{
			int                     id_Station;       // ��վ��
			double                  offsetFrequence;  // Ƶƫ���ƽ��
			double                  zenithDelay;      // ��������ƽ��
			vector<ObsEqArcElement> obsList;
		};

		struct DorisEopEstParameter
		{
			TAI    t0_xpyput1;
			double xp;           // ��λ: ����
			double xpDot;        // ��λ: ����/��
			double yp;
			double ypDot;
			double ut1;          // ��λ: ����
			double ut1Dot;       // ��λ: ����/��
			
			DorisEopEstParameter()
			{
				xp     = 0;
				xpDot  = 0;
				yp     = 0;
				ypDot  = 0;
				ut1    = 0;
				ut1Dot = 0;
			}

			// ���������ת����Ľ���
			void getEst_EOP(TAI t, Matrix &matEst_EP, Matrix &matEst_ER)
			{
				double spanSeconds = t - t0_xpyput1;
				double delta_xp = xp + xpDot * spanSeconds;
				double delta_yp = yp + ypDot * spanSeconds;
				matEst_EP.Init(3,3);
				matEst_EP.SetElement(0, 0,  1);
				matEst_EP.SetElement(0, 2,  delta_xp);
				matEst_EP.SetElement(1, 1,  1);
				matEst_EP.SetElement(1, 2, -delta_yp);
				matEst_EP.SetElement(2, 0, -delta_xp);
				matEst_EP.SetElement(2, 1,  delta_yp);
				matEst_EP.SetElement(2, 2,  1);

				double delta_ut1 = ut1 + ut1Dot * spanSeconds;
				matEst_ER.Init(3,3);
				matEst_ER.SetElement(0, 0,  1);
				matEst_ER.SetElement(0, 1,  delta_ut1);
				matEst_ER.SetElement(1, 0, -delta_ut1);
				matEst_ER.SetElement(1, 1,  1);
				matEst_ER.SetElement(2, 2,  1);
			}
		};

		class DorisLeoSatDynPOD : public SatdynBasic
		{
		public:
			DorisLeoSatDynPOD(void);
		public:
			~DorisLeoSatDynPOD(void);
		public:
			bool loadStdcFile(string strStdcFileFolder);
			bool getStationPos(string site_code, TAI t, double &x, double &y, double &z);
			bool getObsArcList(vector<Doris2_2_EditedObsEpoch> obsEpochList, vector<DorisObsEqArc> &obsArcList);
			bool adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 30.0, int q = 11);
			bool getOrbPartial_interp(TDT t, vector<TimePosVel>& orbitlist_ac, vector<Matrix>& matRtPartiallist_ac, TimePosVel &interpOrbit, Matrix &interpRtPartial);
			bool getTransmitPathDelay(TAI t, POS3D satPos_j2000, POS3D staPos_ECEF, DorisEopEstParameter eopEstPara, double& delay, POS6D& staPosVel_j2000, double threshold = 1.0E-07);
            bool dynamicPOD_2_2(string obsFilePath, SatdynBasicDatum &dynamicDatum, TAI t0_forecast, TAI t1_forecast, vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bForecast = true, bool bResEdit = true);
		public:
			double                 m_ppDorisStationPos[MAX_ID_DORISSTATION + 1][3]; // ��¼ÿ��DORIS��վλ��(�ع�ϵ)
			DorisLeoSatDynPODPara  m_podParaDefine;
			Doris2_2_EditedObsFile m_obsFile;      // �۲������ļ�
			vector<DorisSTCDFile>  m_listStcdFile; // ��վλ�������ļ�
			POS3D                  m_pcoAnt;       // ����ƫ����
		};
	}
}
