#pragma once
#include "SatdynBasic.hpp"
#include "constDef.hpp"
#include "structDef.hpp"
#include "MathAlgorithm.hpp"
#include "lambda.hpp"
#include <map>
#include "Rinex3_03_EditedObsFile.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "TROZPDFile.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"
#include "igs05atxFile.hpp"
#include "Sinex2_0_File.hpp"
#include"StaOceanLoadingDisplacementFile.hpp"

using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::Math;
using namespace NUDTTK::LAMBDA;

//  Copyright 2017, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	namespace GPSPod
	{
		// �۲�������Ԫ�ṹԪ�أ�����α�����ݣ���λ���� +
		struct NETPOD_ObsEqEpochElement
		{
			string name;           // ������, ��G01			
			float  Azimuth;        // �۲ⷽλ��
           float  Elevation;      // �۲�߶Ƚ�
			double obs_code;       // α��۲����ݣ�IF
			double obs_phase;      // ��λ�۲����ݣ�IF
			double ambiguity_IF;    // IFģ����
			double ambiguity_MW;    // MWģ���� +
			float  weightCode;        // α��۲�Ȩֵ
			float  weightPhase;       // ��λ�۲�Ȩֵ
			int    id_Ambiguity;      // ģ�������
			BYTE    mark_GNSSSatShadow;      // �����Ӱ���, Ĭ�� 0 �� δ�����Ӱ

			NETPOD_ObsEqEpochElement()
			{
				mark_GNSSSatShadow = 0;
			}
		};
		// �۲�������Ԫ�ṹ
		struct NETPOD_ObsEqEpoch
		{
			int                               id_Epoch;            // �۲���Ԫ���
			int                               validIndex;          // ��Чʱ���ǩ  (������λ�۲ⷽ����ƾ���������Чλ�ò����ţ�
			int                               eyeableSatCount;     // ����������
			map<string, NETPOD_ObsEqEpochElement>  obsList;       // ��ͬ���ǹ۲��б�
		};

		// ������Ԫ�ṹԪ�أ�����в����ʸ�� +
		struct NETPOD_PodEqEpochElement
		{
			string name;              // ������, ��G01			
			float  oc_code;           // α��в�
			float  oc_phase;          // ��λ�в�
			float  rw_code;           // α��³��Ȩ
			float  rw_phase;          // ��λ³��Ȩ
			float  weightCode;         // α��۲�Ȩֵ
			float  weightPhase;        // ��λ�۲�Ȩֵ
			bool   bEphemeris;         // ����Ԫ����������ȡ�ɹ���ʶ
			POSCLK vecLos_A;          // ����ʸ��, ����ʱʹ��
			TimePosVel pvEphemeris;          // ��¼GNSS���ǹ��λ�ü������ ???
			double  obscorrected_value;      // �۲����ݸ�����-��λ
			double  obscorrected_value_code;  // �۲����ݸ�����-��λ
			Matrix  interpRtPartial_A;       // ƫ����, ����ʱʹ�� ???  

			NETPOD_PodEqEpochElement()
			{
				rw_code  = 1.0f;
				rw_phase = 1.0f;
				oc_code  = 0.0f;
				oc_phase = 0.0f;
			}
		};
		// ������Ԫ�ṹ
		struct NETPOD_PodEqEpoch
		{
			int                            id_Epoch;            // �۲���Ԫ���
			int                            validIndex;          // ��Чʱ���ǩ  (������λ�۲ⷽ����ƾ���������Чλ�ò����ţ�
			int                            eyeableSatCount;     // ����������
			map<string, NETPOD_PodEqEpochElement>  obsList;       // ��ͬ���ǹ۲��б�
		};

		//// ��Ҫ����������ļ����ݸ�ʽ
		//struct tagStaEpochSatData
		//{
		//	string name;           // ������, ��G01			
		//	float  Azimuth;        // �۲ⷽλ��
  //         float  Elevation;      // �۲�߶Ƚ�
		//	double obs_code;       // α��۲����ݣ�IF
		//	double obs_phase;      // ��λ�۲����ݣ�IF
		//	double ambiguity_IF;    // IFģ����
		//	double ambiguity_MW;    // MWģ���� +
		//	float  weightCode;        // α��۲�Ȩֵ
		//	float  weightPhase;       // ��λ�۲�Ȩֵ
		//	int    id_Ambiguity;      // ģ�������
		//	float  oc_code;        // α��в�
		//	float  oc_phase;       // ��λ�в�
		//	float  rw_code;           // α��³��Ȩ
		//	float  rw_phase;          // ��λ³��Ȩ
		//	bool   bEphemeris;        // ����Ԫ����������ȡ�ɹ���ʶ
		//	POSCLK vecLos_A;          // ����ʸ��, ����ʱʹ��
		//	TimePosVel pvEphemeris;          // ��¼GNSS���ǹ��λ�ü������ ???
		//	double  obscorrected_value;      // �۲����ݸ�����-��λ
		//	double  obscorrected_value_code;  // �۲����ݸ�����-��λ
		//	Matrix  interpRtPartial_A;       // ƫ����, ����ʱʹ�� ???  
		//	BYTE    mark_GNSSSatShadow;      // �����Ӱ���, Ĭ�� 0 �� δ�����Ӱ

		//	tagStaEpochSatData()
		//	{
		//		mark_GNSSSatShadow = 0;
		//	}

		//};

		// �۲ⷽ�̻���Ԫ��, ���ڴ洢��λ���ݣ�����
		struct NETPOD_ObsEqArcElement
		{   
			// ���ڹ��취����
			int    nObsTime;     // �۲�ʱ�����
			double LIF;          // ԭʼ��λ�޵������Ϲ۲���
			double MW;          // ԭʼ��λMW��Ϲ۲���
			double L1_L2;
			double dt;
			float  wL;           // ����۲�Ȩֵ, ��߶Ƚ��й�
			float  rw;           // �༭���Ȩֵ
			float  oc;
			NETPOD_ObsEqArcElement()
			{
				rw = 1.0f;
				oc = 0.0f;
				wL = 1.0f;
			}
		};

		// ���������ڴ洢ʱ�������վ���ݽṹ�������ڻ��ߴ���ʱֱ�Ӽ���������
		struct NETPOD_ObsEqArc 
		{
			double       ambiguity_IF; 
			double       ambiguity_MW; // +
			short int    id_Ambiguity; 
			//GPST         t0;
			//GPST         t1;
			//string       nameSta;
			string       nameSat;
			string       nameFreq;     // ���ڲ���Ƶ������� 
			int          count;
			map<int, NETPOD_ObsEqArcElement> obsList;
			bool updateRobustWeight(double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double robustfactor = 3.0);
		};


		struct NETPOD_DownSamplingElement // ����
		{
			// ������㲿��
			POS3D  vecLos;
			Matrix interpRtPartial;
			// ��������������
			short int id_zenithDelay_0;     
	       double    zenithDelay;           // ����ֵ
			double    zenithDelay_partial_0; // ƫ����
			double    zenithDelay_partial_1;
			// �в�༭����
			float  oc_LIF;
			float  oc_PIF;
			float  rwP;
			float  rwL;
		};
		struct NETPOD_ObsElement   // ����
		{
			double    LIF;
			double    PIF;
			short int id_ambiguity;
			float     wL;
			float     wP;
			float     zenithDelay_wmf;            // ���ڶ�������������
			double    corrected_value;
			double    corrected_value_code;       // ������-α��
			vector<NETPOD_DownSamplingElement> ds; // �߲����͵Ͳ����Ĳ������֣�����Matrix��vector�ȶ�̬�ṹ����ֹռ�ÿռ�
		};

		typedef map<string, NETPOD_ObsElement> NETPOD_SatMap; // ����

		typedef map<string, NETPOD_SatMap> NETPOD_StaMap; // ����

		struct NETPOD_TropZenithDelay  // ����
		{
			GPST    t;                  // ��������Ƶ�ʱ��
			double  zenithDelay_Init;    // ������ʪ�������Ƴ�ֵ
			double  zenithDelay_Est;     // ��������ƽ��
			int     count;              // ��Ч��Ԫ����
		};

		// �������ݽṹ
		struct NETPOD_SatDatum  // ����
		{
			int                index;             // �����������λ��(������� 0,1,2,..)
			int                index_0;           // �׸���������λ��
			int                count_obs;         // �۲����ݸ���
			SatdynBasicDatum    dynamicDatum_Init;  // ��ʼ���
			SatdynBasicDatum    dynamicDatum_Est;   // ���ƵĹ�����

			// PCO����
			bool      bOnEstPco_X;
			bool      bOnEstPco_Y;
			bool      bOnEstPco_Z;
			double    sigmaPco; 
			POS3D     pco_Est;      // λ�ù��ƽ��
			// PCV����
			bool      bOnEstPcv;

			vector<TimePosVel>  REF_OrbList;       // ���ڹ����ֵ�Ĳο�������, �����ʵȲ�����adamsCowell���ֲ�����75s��
			vector<Matrix>      REF_RtPartialList; // ����ƫ������ֵ
			bool getEphemeris(TDT t, TimePosVel& interpOrb, int nLagrange = 9);
			bool getRtPartial(TDT t, Matrix& interpRtPartial);
			bool getEphemeris_PathDelay(TDT t, POSCLK staPosClk, double& delay, TimePosVel& gnssOrb, Matrix& gnssRtPartial, double threshold = 1.0E-07);
		};
		typedef map<string, NETPOD_SatDatum> NETPOD_SatDatumMap; // ����
		
		struct NETPOD_SatOrbEstParameter  // ����
		{
			NETPOD_SatDatumMap satParaList;
			GPST   t0_xpyput1;
			double xp;           // ��λ: ����
			double xpDot;        // ��λ: ����/��
			double yp;
			double ypDot;
			double ut1;          // ��λ: ����
			double ut1Dot;       // ��λ: ����/��
			
			NETPOD_SatOrbEstParameter()
			{
				xp     = 0;
				xpDot  = 0;
				yp     = 0;
				ypDot  = 0;
				ut1    = 0;
				ut1Dot = 0;
			}

			// ���������ת����Ľ���
			void getEst_EOP(GPST t, Matrix &matEst_EP, Matrix &matEst_ER)
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
		// �������ݽṹ�����ڹ�����
		struct NETPOD_SatSp3FitDatum       // ����
		{
			SatdynBasicDatum    dynamicDatum_Init;     // ��ʼ���, ��sp3�ļ���ȡ
			SatdynBasicDatum    dynamicDatum_Est;      // ���ƵĹ�����
			vector<TimePosVel>  sp3orbitList_ECEF;     // sp3 ���, �ع�ϵ
            vector<TimePosVel>  fitorbitList_ECEF;     // ��Ϲ��, �ع�ϵ
			double              fitrms_X;              // �����Ͼ��� 
			double              fitrms_Y;
			double              fitrms_Z;
			double              fitrms_R;
			double              fitrms_T;
			double              fitrms_N;
			double              fitrms_Total;				
            
			bool getInterpOrb_Fit(GPST t, TimePosVel& interpOrbit, int nLagrange = 9);
		};

		typedef map<string, NETPOD_SatSp3FitDatum> NETPOD_SatSp3FitDatumMap; // ����

		struct NETPOD_Sp3FitParameter // ����
		{
			NETPOD_SatSp3FitDatumMap satParaList;
			GPST   t0_xpyput1;
			double xp;           // ��λ: ����
			double xpDot;        // ��λ: ����/��
			double yp;
			double ypDot;
			double ut1;          // ��λ: ����
			double ut1Dot;       // ��λ: ����/��

			double meanFitRms_X; // �����Ͼ��� 
			double meanFitRms_Y;
			double meanFitRms_Z;
			double meanFitRms_R;
			double meanFitRms_T;
			double meanFitRms_N;
			double meanFitRms_Total;

			// ���������ת����Ľ���
			void getEst_EOP(GPST t, Matrix &matEst_EP, Matrix &matEst_ER)
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

			NETPOD_Sp3FitParameter()
			{
				xp     = 0;
				xpDot  = 0;
				yp     = 0;
				ypDot  = 0;
				ut1    = 0;
				ut1Dot = 0;
			}
		};


		// Ƶ����Ϣ�ṹ
		struct NETPOD_Freq // ����
		{
			double                                  freq_L1;
			double                                  freq_L2;
			NETPOD_Freq()
			{
				freq_L1 = 0.0;
				freq_L2 = 0.0;
			}
		};

      // ������������ṹ, �����Ӳ���
		struct NETPOD_DEF
		{
			int                          max_OrbIterationCount;      // ����Ľ�������ֵ		
			unsigned int                  min_ArcPointCount;         // ���ι۲������ֵ, ����С�ڸ���ֵ�Ļ��ν���ɾ��
			int                          min_EyeableSatCount;       // ��С�������Ǹ���, ����С�ڸ���ֵ����Ԫ�����������
			double                       max_ArcInterval;     // ��Ԫ�����ֵ, ��������ֵ��Ϊ���»���(��λ����)
           bool                          on_UsedInShadow; // +
		   bool                          on_WeightGEO;
		   float                         weightInShadow; // +
		   float                         weightGEO;
			bool                         on_AmbiguityFixing;        // �Ƿ���й̶�����ģ����
			bool                         bOn_GEOSolut;             // �Ƿ����GEO����	
			bool                         bOn_IGSOSolut;            // �Ƿ����IGSO����
			bool                         bOn_Used_delta_u;          // �Ƿ�ʹ��Delta_u��Ϊ̫����ѹ������
			double                       apriorityRms_TZD_abs;      // �����վ������춥�ӳپ���, ���ڵ�����춥�ӳپ���Լ�����̼�Ȩ����
			double                       apriorityRms_TZD_rel;      // �������ڵ�����춥�ӳٲ��������Լ�����̼�Ȩ����
			double                       apriorityWet_TZD;          // ��վ������ʪ������������ֵ
			double                       apriorityWet_TZD_period;   // ��վ������ʪ������������
			double                       min_Wet_TZD_ncount;        // ���������������������ݸ���

			float                        min_Elevation;             // ��С�߶Ƚ�
			float                        priorsigma_LIF;            // ������λ��Ȩ����
			bool                         bOn_WeightElevation;       // �Ƿ���и߶ȽǼ�Ȩ +
			bool                         bOn_StaSolidTideCor;       // �Ƿ���в�վ����Ĺ��峱���� +
			bool                         bOn_StaOceanTidesCor;      // �Ƿ���в�վ����ĺ������� +
			bool                         bOn_GraRelativity;         // �Ƿ�����������������۸��� +


			bool                     bOnEst_StaTropZenithDelay;      // ��վ��������ƿ��� +
			bool                     bOnEst_StaPos;           // ��վλ�ù���
			bool                     bOnEst_SatEphemeris;       // ���ǹ������

			double                   max_FitRms_Total;        // ��ϲв������ֵ

			NETPOD_DEF()
			{
				max_OrbIterationCount     = 10;				
				min_ArcPointCount         = 12;
				min_EyeableSatCount      = 3;
				max_ArcInterval          = 180;
				on_UsedInShadow          = false;
				on_WeightGEO             = false;
				weightInShadow           = 0.0;
				weightGEO                = 0.0;
				on_AmbiguityFixing       = false;
				bOn_GEOSolut             = false;
				bOn_IGSOSolut            = false;
				bOn_Used_delta_u          = false;
				apriorityRms_TZD_abs      = 0.5;
				apriorityRms_TZD_rel      = 0.05;
				apriorityWet_TZD          = 0;                      // ��վ������ʪ������������ֵ
				apriorityWet_TZD_period   = 3600 * 2;               // ����                
				min_Wet_TZD_ncount        = 12;
				min_Elevation = 5.0f;
				priorsigma_LIF            = 0.005f;
				bOn_WeightElevation       = false;
				bOn_StaSolidTideCor  = true;
				bOn_StaOceanTidesCor = true;
				bOn_GraRelativity    = true;
				bOnEst_StaTropZenithDelay = true;
				bOnEst_StaPos = false;
				bOnEst_SatEphemeris = false;
				max_FitRms_Total    = 0.5;
			}
		};

		// ��ϵͳԤ����, ��Ϊͳһ��ϵͳѡ������, [ע: ѡ���ͬ��վ�Ķ�ϵͳ���ݿ��ܲ�ȫ]
		struct NETPOD_MixedSys
		{
			// ��Ҫ��ʼ������
			char                     cSys;                         // ϵͳ��ʶ
			float                   wSys;                         // ϵͳȨֵ, Ĭ�� 1.0
			string                   name_C1;    // α��
			string                   name_C2;
			string                   name_L1;    // ��λ
			string                   name_L2;
			string                   nameFreq_L1;                  // ����PCV����
            string                   nameFreq_L2;
			NETPOD_Freq               freqSys;
			map<string, NETPOD_Freq>   freqSatList;                  // ��Ҫ���GLONASS��string����Ϊ������

			// ����Ҫ��ʼ������
			int                      iSys;                         // ��¼������m_editedMixedObsFile�洢��λ��
			int                      index_C1;
			int                      index_C2;
			int                      index_L1;
			int                      index_L2;
			float                    priorsigma_PIF;               // ����α���Ȩ����

			
			// ��ϵͳ�йص�Ԥ���岿��, ʹ��ʱ��Ҫͨ��sysFreqName���� 
			bool                     on_GNSSPhaseWindUp;   
			bool                     on_GNSSRelativity;        // �Ƿ���� GNSS ��������۸��� +
		    bool                      on_GNSSAntPCO;       // �Ƿ���� GNSS �������� PCO ���� +
			bool                     on_GNSSAntPCV;       // �Ƿ���� GNSS �������� PCV ���� +
			bool                     on_GNSSRecARP;       // �Ƿ���н��ջ����� ARP ����
			bool                     on_GNSSRecPCOPCV;    // �Ƿ���н��ջ����� PCO/PCV ����

			TYPE_SOLARPRESSURE_MODEL       solarPressure_Model;       // ÿ��ϵͳ��̫����ѹģ��
			TYPE_SOLARPRESSURE_MODEL      solarPressure_Model_BDYF;       // yaw-fixed��̬̫����ѹģ�ͣ�2015/11/13��������	
			TYPE_SOLARPRESSURE_MODEL      solarPressure_Model_BDYS;       // yaw-steering��̬̫����ѹģ��
			double                       period_SolarPressure;  


			string getSysFreqName()
			{
				char sysFreqName[4];
				sprintf(sysFreqName, "%1c%1c%1c", cSys, name_L1[1], name_L2[1]); // "Gij" "Cij" "Eij" "Rxx"
				sysFreqName[3] = '\0';
				return sysFreqName;
			}

			NETPOD_MixedSys()
			{
				wSys = 1.0;
				index_C1 = -1;
				index_C2 = -1;
				index_L1 = -1;
				index_L2 = -1;
				name_C1  = "";
				name_C2  = "";
				name_L1  = "";
				name_L2  = "";
				nameFreq_L1 = "";
				nameFreq_L2 = "";
				on_GNSSPhaseWindUp = true;
				on_GNSSRelativity = true;
				on_GNSSAntPCO = true;
				on_GNSSAntPCV  = true;
				priorsigma_PIF = 0.50f;
			}
		};
		// ��ϵͳ����
		struct NETPOD_MixedSysData        // - 
		{
			// ����Ҫ��ʼ������
			char                      cSys;                   // ϵͳ��ʶ
			float                    wSys;                   // ϵͳȨֵ, Ĭ�� 1.0
			string                    name_C1;                // "C1" "P1" "P2"
			string                    name_C2;                
			string                    name_L1;                // "L1" "L2"
			string                    name_L2;                
			string                    nameFreq_L1;            // Ƶ������"G01"����Ҫ����PCV����
			string                    nameFreq_L2;  
			NETPOD_Freq                freqSys;
			map<string, NETPOD_Freq>    freqSatList;            // ��Ҫ���GLONASS��string����Ϊ������
			float                     priorsigma_PIF;         // ����α���Ȩ����
			int                       index_C1;               // Ƶ��1��2�ڴ���ʱ����
			int                       index_C2;
			int                       index_L1;
			int                       index_L2;
			
			bool                     on_GNSSPhaseWindUp;   
			bool                     on_GNSSRelativity;        // �Ƿ���� GNSS ��������۸��� +
		    bool                     on_GNSSAntPCO;       // �Ƿ���� GNSS �������� PCO ���� +
			bool                     on_GNSSAntPCV;       // �Ƿ���� GNSS �������� PCV ���� +
			bool                     on_GNSSRecARP;       // �Ƿ���н��ջ����� ARP ����
			bool                     on_GNSSRecPCOPCV;    // �Ƿ���н��ջ����� PCO/PCV ����
			
			// ���̼���洢����, ��ʡ�ڴ棬����۲�����
			string                               sysFreqName;                 // ��¼ϵͳ��Ƶ����Ϣ, ����������ص�Ԥ����
			int                                  iSys;                       // ��¼������m_editedMixedObsFile�洢��λ��

			vector<Rinex3_03_EditedObsEpoch>        editedObsEpochList;          // ��ϵͳ��Ԫ��ʽ�۲����ݣ�������������
			vector<Rinex3_03_EditedObsSat>          editedObsSatlist;           // ��ϵͳ���Ǹ�ʽ�۲����ݣ������������� 

			//map<int, NETPOD_ObsEqEpoch>            P_IFEpochList;               // ���������������α�루��Ԫ�ṹ��������map��ʽ����������intΪnObsTime
			
			map<int, NETPOD_ObsEqEpoch>             mapNEQEpochList;            // ��Ԫ�����̹۲�����(����α�����λ��Ԫ����), ��editedObsEpochlist���Ӧ��intΪnObsTime + д���ļ������
			map<int, NETPOD_PodEqEpoch>             mapPODEpochList;            // ��Ԫ�����̶�������(�����в����ʸ������), ��editedObsEpochlist���Ӧ��intΪnObsTime + д���ļ������
			
			vector<NETPOD_ObsEqArc>                 L_IFArcList;               // �����������������λ���ݣ����νṹ��->��Ԫ�ṹ��������������
			
			//string                                pathEditedObsEpochListFile;    // ��ϵͳ��Ԫ�۲������ļ�Ŀ¼ +
			//map<int, NETPOD_ObsElement>             mapNEQEpochList;              // ��Ԫ����������, ��editedObsEpochlist���Ӧ��intΪnObsTime
			int                                   id_Ambiguity_0;               // ģ������ʼ���
			map<string, int>                       mapObsCount;                  // ÿ�����ǵĹ۲����ݸ���

			NETPOD_MixedSysData()
			{
			}
		};

		// �Ӳ�ⲿ��
		struct GNSSPOD_CLKSOL // ��վ���ݽ����
		{
			GPST    t;
			double  clock;
			int     sqlMark;          // ����: 0-��ֵ���, δ��������; 1-α���; 2-��λ��
			int     eyeableSatCount; 
			int     validIndex;       // ��¼��Ч���������, ��ЧΪ-1 
			double   sigma; 
			map<int, double> mapN_ca; // ��¼������ cc��ca
		};

		typedef map<int,   POS3D>    StaPosMap;

		// ��վ���ݽṹ + 
		struct NETPOD_StaDatum
		{
			// ���벿��
			string                     staName;
			bool                       bOnUsed;
			POS6D                      pv0_ITRF;                    // ITRF�²�վ��������
			//Rinex3_03_EditedObsFile    m_editedMixedObsFile;           // ��վ�۲������ļ�
			string                     pathRinex3_03_EditedObsFile;   // ��վ�۲������ļ�Ŀ¼ +
			int                        count_MixedEpoch;             // �۲�������Ԫ����
			GPST                       t0;                         // �۲�������ʼʱ��
           GPST                        t1;                         // �۲�������ʼʱ��
		   	ENU                        arp0_ENU;                   // ���߲ο���ARP�ڲ�վ����ϵ�µ�����(ENU)

			// ���̼���洢����: ��ϵͳ����, ���ϵͳ��Ԥ���� m_dataMixedSysList ��Ӧ
			// ��������: Ԥ���� m_dataMixedSysList �� N ��ϵͳ, �������п���ֻ�� M ��ϵͳ dataMixedSysList, ��ƥ��
			//           ÿ����վ�����ļ����������� "index_xx" �����ǲ�ͬ�ģ���Ҫ�� dataMixedSysList ��
			// ����취: m_dataMixedSysList ��ֻ��������ѡȡ֮��ı�Ҫ���������Ϣ, �õ�������������Ϣ���� dataMixedSysList ��ȥ
			vector<NETPOD_MixedSysData>  dataMixedSysList;
			bool                       on_RecPCVFromIGS;      // �Ƿ����IGS���ջ�����PCV
			AntCorrectionBlk            pcvRecFromIGS;         //  IGS���ջ�����PCV�ṹ

			vector<string>              satGnssNameList;       // GNSS�����б�

			// λ�ù��ƿ�
			char      szAntType[20 + 1];
			ENU       arp_Ant;	
			bool      bOnEstPos;     // ��վλ�ù��ƿ���
			int		  indexEstPos_0;  // ��վλ�����������Ʋ����б��е�λ��
			double    sigmaPos;      // ���Ʋ�վλ�ù��Ƶ�Լ��������2012.12.09��������
			POS3D     pos_Est;      // λ�ù��ƽ��

			// �Ӳ���ƿ�
			vector<GNSSPOD_CLKSOL>       clkList; // �Ӳ�ⲿ��
			vector<int>                 validMixedEpochList; // ��¼�Ӳ���Ч���Ӧ��ʱ��λ��

			// ģ���Ȳ���
			int count_EstAmbiguity;   // ��ǰ��վģ���ȸ���

			vector<POS3D>     corrTideList;      // �洢��վ���곱ϫ������[�ع�ϵ], ���ⷴ������

			// ������̲�վ����
			StaPosMap                 staPosList;     //��վA�ڹ���ϵ�µ������б�
			StaPosMap                 staECEFPosList; //��վA�ڵع�ϵ�µ������б�
			//map<GPST, POS3D> staJ2000PosList; // ��˸ߵͲ�����
			//map<GPST, POS3D> staECEFPosList;  // ��˸ߵͲ�����

			// ��վλ�ó�ϫ����д���ļ��� ��¼ÿ��ʱ�̵Ĺ۲�Ȩֵ��Ϣ L_IFArcList staPosList staECEFPosList��
			
			// ���������
			int       indexZenith_0;  // ��վ�׸���������Ʋ������������Ʋ����б��е�λ��	
			vector<NETPOD_TropZenithDelay> zenithDelayEstList;

			// ϵͳƫ�����
			int       indexSysBias_0; // ��վ�׸�ϵͳƫ��������������Ʋ����б��е�λ��	

			
			// ������ת��������
			vector<Matrix>            matPR_NRList;
			vector<Matrix>            matERList_0;    // ���Ե�����ת����
			vector<Matrix>            matEPList_0;    // ���Լ�����ת����
		    vector<Matrix>            matEPList;      // �Ľ���ĵ�����ת����
			vector<Matrix>            matERList;      // �Ľ���ĵ�����ת����

			void init(double period_TropZenithDelay = 3600 * 2.0, double apriorityWet_TZD = 0);	
			int  getIndexZenithDelayEstList(DayTime t);      // ȷ��ʱ��t�ڶ���������б��е�λ��
		};

		typedef map<string, NETPOD_StaDatum> NETPOD_StaDatumMap;

		// GNSS �������ܶ�����(SatdynBasic��������)
		class GNSSNetPOD : public SatdynBasic
		{
			public:
				GNSSNetPOD(void);
			public:
				~GNSSNetPOD(void);
			private:
				void weighting_Elevation(float Elevation, float& weight_P_IF, float& weight_L_IF);
				bool adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 75.0, int q = 11);			
			public:
				bool sp3Fit(string strSp3FilePath, NETPOD_Sp3FitParameter& paraSp3Fit, bool bOnEst_EOP = true, string outputSp3FitFilePath = "");
				bool mainGNSSPOD(string inputSp3FilePath, string outputSp3FilePath, string outputClkFilePath, string outputTrozpdFilePath, GPST t0, GPST t1, double h_clk = 30.0, double h_sp3 = 30.0);
			
			private:
				bool correctSta_ObsEpoch(NETPOD_StaDatumMap::iterator it_Sta); // �۲���������
				bool updateSta_ObsEpoch(NETPOD_StaDatumMap::iterator it_Sta); // ��ʼ���۲�������Ԫ��Ϣ 
			private:
				double  m_stepAdamsCowell;
				SP3File m_sp3FileJ2000;                    	// ��¼J2000��Gnss���ǹ��
				map<string, AntCorrectionBlk>  m_mapGnssPCVBlk; //  ��¼Gnss���ǵ�PCV�������
			public:
				svnavMixedFile                 m_svnavMixedFile;	     // ���ڻ����������
				GNSSYawAttitudeModel           m_gymMixed;
				SP3File                        m_sp3File;
				igs05atxFile			       m_atxFile;		         // ���������ļ�
				NETPOD_StaDatumMap             m_mapStaDatum;          // ��վ����
				vector<NETPOD_MixedSys>        m_dataMixedSysList;     // ���ϵͳ���� +
				Sinex2_0_File                  snxFile;                // ��վ�����ļ� +
				StaOceanLoadingDisplacementFile  m_staOldFile;          // �����ļ� +
				NETPOD_DEF                      m_netPodParaDefine;
				TROZPDFile                      zpdFile;              // �������ļ� +	
				NETPOD_SatOrbEstParameter        paraSatOrbEst; // ����������ṹ
		};
	}
}
