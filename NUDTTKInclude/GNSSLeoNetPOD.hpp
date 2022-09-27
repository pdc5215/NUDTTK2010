#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "dynPODStructDef.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "igs05atxFile.hpp"
#include "AntPCVFile.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"
#include "GNSS_AttitudeFile.hpp"
#include "graceKBR1BFile.hpp"
#include "gracefoKBR1BFile.hpp"
#include "lambda.hpp"
#include "wsbFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "LeoNetPODEditedSdObsFile.hpp"
#include "Rinex2_1_LeoMixedEditedObsFile.hpp"
#include "Ionex1_0_File.hpp"

//  Copyright 2018, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::SpaceborneGPSPreproc;
using namespace NUDTTK::Math;
using namespace NUDTTK::LAMBDA;

namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		// ���������ڴ洢ʱ�������վ���ݽṹ�������ڻ��ߴ���ʱֱ�Ӽ���
		struct LEOPOD_ObsArc 
		{
			double             ambiguity_IF; 
			double             ambiguity_MW; // +
			double             rms_mw;          // ���� MW �仯����
			double             ambFixedWL;      // ����ģ���ȹ̶��⣺����ģ���ȹ̶���ͨ�� MW ��Ϸ�ʽ����
			double             res_WL;          // ����ģ���ȹ̶���в�
			bool               bOn_fixedWL;     // �û��ο���ģ���ȹ̶��ɹ����
			int                id_Ambiguity; 
			GPST               t0;
			GPST               t1;
			GPST               t;               // �����м�ʱ��
			BYTE               id_Sat;
			double             mean_elevation;  // ƽ���߶Ƚǣ��ۿ���2020.7.22
			double             rms_res;          // ������λ�в�RMS
			ObsEqArcElementMap obsList;
			LEOPOD_ObsArc()
			{
				rms_mw     = 0.0;
				bOn_fixedWL = false;
				mean_elevation = 0.0;
				rms_res = 0.0;
			}
			bool updateRobustWeight(double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double robustfactor = 3.0);
			bool updateRobustWeight_New(double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double robustfactor = 3.0);
		};

		// �Ӳ�ⲿ��
		struct LEOPOD_CLKSOL
		{
			GPST    t;
			double  clock;
			POS3D   pos;              // ����λ��
			POS3D   vel;              // �ٶ���Ϣ
			int     sqlMark;          // ����: 0-��ֵ���, δ��������; 1-α���; 2-��λ��
			double  pdop;
			int     eyeableSatCount; 
			int     validIndex;       // ��¼��Ч���������, ��ЧΪ-1 
			double  sigma; 
			map<int, double> mapN_ca; // ��¼������
		    map<int, Matrix> mapN_cb; // ��¼������,�˶�ѧ +
		};

		struct LEOPOD_ZD_AmbEq
		{
			GPST     t0;
			GPST     t1;
			int      id_Ambiguity;
			BYTE     id_Sat;
				//double   rms_MW;
			double   ambiguity_MW;
			double   ambiguity_IF; // IF �����
			double   ambiguity_Fixed_IF; // IF �̶���
			double   ambiguity_Fixed_MW; // WL�̶���
			double   ambiguity_NL; // NL �����
			double   ambiguity_Fixed_NL; // NL�̶��� 
			double   res_NL;         // NL�в�
		    double   mw_rms; // +
			double   res_rms; // + ����в�RMS
			bool     flag_fixed_nl; // ����ģ���ȹ̶��Ƿ�ɹ���ǩ
			LEOPOD_ZD_AmbEq()
			{
				flag_fixed_nl = false;
			}
			//double   mw_res; // +
		};

		struct LEOPOD_SD_AmbEq
		{
			GPST     t0;
			GPST     t1;
			int      id_Ambiguity_A;
			int      id_Ambiguity_B;
			BYTE     id_Sat;
			BYTE     id_Sat_B; // +
			double   rms_mw;    // ����mw_rmsֵ
			double   ambiguity_MW;
			double   ambiguity_IF;
            double   ambiguity_SD_IF;
		};

		// ģ����˫��Լ������
		struct LEOPOD_DD_AmbEq
		{
			int    id0_Ambiguity_A;
			int    id0_Ambiguity_B;
			int    id1_Ambiguity_A;
			int    id1_Ambiguity_B;
			double ambiguity_DD_IF;
		};

		// ��ϵͳԤ����, ��Ϊͳһ��ϵͳѡ������, [ע: ѡ���ͬ��վ�Ķ�ϵͳ���ݿ��ܲ�ȫ]
		struct LEOPOD_MixedSys
		{
			// ��Ҫ��ʼ������
			char                         cSys;        // ϵͳ��ʶ
			double                       wSys;        // ϵͳȨֵ, Ĭ�� 1.0
			char                         recType;     // ���ڲ�ͬ���ǲ�ͬ���ͽ��ջ���ʶ����
			string                       name_C1;
			string                       name_C2;
			string                       name_L1;
			string                       name_L2;
			string                       nameFreq_L1; // Ƶ������"G01"����Ҫ����PCV����
			string                       nameFreq_L2;  
			double                       freq_L1;
			double                       freq_L2;
			// ��ϵͳ�йص�Ԥ���岿��, ʹ��ʱ��Ҫͨ��sysFreqName���� 
			bool                         on_PhaseWindUp;   
			bool                         on_GnssRelativity;
			bool                         on_GnssPCV;
			bool                         on_RecPCV;
			bool                         on_RecCRV;
			bool                         on_RecCRVP1;
			bool                         on_RecCRVP2;
			float                        priorsigma_PIF;              // ����α���Ȩ����

			string getSysFreqName()
			{
				char sysFreqName[4];
				sprintf(sysFreqName, "%1c%1c%1c", cSys, name_L1[1], name_L2[1]); // "Gij" "Cij" "Eij" "Rxx"
				sysFreqName[3] = '\0';
				return sysFreqName;
			}
			
			LEOPOD_MixedSys()
			{
				wSys = 1.0;
				recType = 'N';
				on_PhaseWindUp = true;
				on_GnssRelativity = true;
				on_GnssPCV = true;
				on_RecCRV  = false;
				on_RecPCV  = false;
				on_RecCRVP1 = false;
				on_RecCRVP2 = false;
				priorsigma_PIF = 0.50f;
			}
		};

		struct LEOPOD_MixedSysData
		{
			// ����Ҫ��ʼ������
			char                                cSys;                   // ϵͳ��ʶ
			double                              wSys;                   // ϵͳȨֵ, Ĭ�� 1.0
			string                              name_C1;                // "C1" "P1" "P2"
			string                              name_C2;                
			string                              name_L1;                // "L1" "L2"
			string                              name_L2;                
			string                              nameFreq_L1;            // Ƶ������"G01"����Ҫ����PCV����
			string                              nameFreq_L2;  
			double                              freq_L1;
			double                              freq_L2;
			int                                 index_C1;               // Ƶ��1��2�ڴ���ʱ����
			int                                 index_C2;               // ���ĸ�C1C2L1L2Ĭ��Ϊ-1
			int                                 index_L1;
			int                                 index_L2;
			float                               priorsigma_PIF;         // ����α���Ȩ����

           // ���̼���洢����
			string                              sysFreqName;            // ��¼ϵͳ��Ƶ����Ϣ, ����������ص�Ԥ����
			int                                 iSys;
			vector<Rinex2_1_LeoEditedObsEpoch>   editedObsEpochlist;     // ��ϵͳ��Ԫ��ʽ�۲�����, �ǻ�ϸ�ʽ
			vector<Rinex2_1_EditedObsSat>        editedObsSatlist;       // ��ϵͳ���Ǹ�ʽ�۲�����, �ǻ�ϸ�ʽ
			map<int, PODEpoch>                  mapDynEpochList;    
			vector<ObsEqEpoch>                  P_IFEpochList;       
			vector<LEOPOD_ObsArc>               L_IFArcList;
			vector<ObsEqEpoch>                  L_IFEpochList;
			//map<int, double>                    mapWindupPrev;          // ��¼Windup��������
			vector<int>                         mixedEpochIdList;       // ��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ���, �����໥����
			vector<int>                         epochIdList;            // ��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ���
			vector<O_CResEpoch>                 ocResP_IFEpochList;     // �޵����α��O-C�в�
			vector<O_CResArc>                   ocResL_IFArcList;       // �޵������λO-C�в�
			double                              ocResRMS_P_IF;
			double                              ocResRMS_L_IF;
			POS3D                               pcoRecEst;              // ����ƫ�ƹ��ƽ������ϵͳ��Ƶ���й�
			double                              sysBias;                // ϵͳ��ƫ���GPSΪ�ο�ϵͳ����ֵΪ 0
			
			// �ǲ�ģ���ȸ������Ϣ, ���ڲ�ͬ��վ�������γ�˫�ϵͳ�䲻�������
			int                                 n0_AmbiguitySys;        // ÿ��ϵͳģ���Ȳ��������

			LEOPOD_MixedSysData()
			{
				sysBias = 0.0;
				wSys    = 1.0;
				ocResRMS_P_IF = 0.0;
				ocResRMS_L_IF = 0.0;
			}
		};
		// ���μ䵥��ģ���Ƚ��
		struct BetweenPass_SD_AmbEq_q
		{
			double   delta_bIJ;       // ���� I �ͻ��� J �䵥��ֵ  delta_bIJ = bI - bJ, խ��
			double   delta_MWbIJ;     // ���� I �ͻ��� J ��, ����ģ���ȵ���ֵ
			double   delta_IFbIJ;     // ���� I �ͻ��� J ��, IFģ���ȵ���ֵ
			double   res;             // rmsֵ,����խ��ģ����
			double   res_WL;          // �������ģ���Ȳв�
			bool     fixed;           // �̶��ɹ����
			bool     fixed_wl;        // ����̶��ɹ����
			bool     fixed_nl;        // խ��̶��ɹ����
			int      id_Ambiguity_A;  // ��һ������ģ�������
			int      id_Ambiguity_B;  // �ڶ�������ģ�������
			double   delta_nl_fixed;  // ����խ��ģ���ȹ̶�ֵ
			double   delta_wl_fixed;  // �������ģ���ȹ̶�ֵ
			double   constraint_AB;   // A-B ģ����Լ��
			BetweenPass_SD_AmbEq_q()
			{
				res = 0.0;
				res_WL = 0.0;
				delta_nl_fixed = 0;
				delta_wl_fixed = 0;
				fixed          = false;
				constraint_AB  = 0.0;
			}
		};
		//// ��ͬ��̬ģ������
		//enum TYPE_ATT_MODEL
		//{
		//	TYPE_ATT_Body2J2000   = 1,            // �ǹ�����ϵ����������ϵ, GRACE,CHAMP
		//	TYPE_ATT_Body2ECEF    = 2             // �ǹ�����ϵ���ع�����ϵ��Swarm
		//};
		struct LEOPOD_SD_Section  // �ۿ� +
		{
			GPST   t0;
			GPST   t1;
			bool   on_Fixed_mw;        // ��¼ģ�����Ƿ�̶�
			float  max_Decimal_mw;     // ��¼ģ���ȹ̶������С������
			Matrix matSDFixedFlag_mw;  // ��¼ģ���ȹ̶��ɹ����
			Matrix matSDFixed_mw;      // ��¼ģ���ȹ̶�ֵ
			int    count_Fixed_mw;     // mw�̶�����
			bool   on_Fixed_NA;
			float  max_Decimal_NA;
			Matrix matSDFixedFlag_NA;
			Matrix matSDFixed_NA;
			int    count_Fixed_NA;     // NA�̶�����
			vector<double>     WL_ResList; // ��¼WLС���в�
			vector<double>     N1_ResList; // ��¼N1С���в�
			vector<LEOPOD_ZD_AmbEq> ambZDEqList;
            vector<LEOPOD_SD_AmbEq> ambSDEqList;
		};
		// ��վ���ݽṹ, �����������ݺ��������ⲿ��
		struct LEOPOD_StaDatum 
		{
			// ���벿��
			GPST      t0;                     // ��ʼʱ��
			GPST      t1;                     // ����ʱ��
			string    staName;
			string    pathMixedEditedObsFile;
			string    pathFolder;
			size_t    count_MixedEpoch;    
			double    orbOutputInterval;
			bool      on_IsUsed_InitDynDatum;// �Ƿ�ʹ���Ѿ��еĳ���
			double    ArcLength_InitDynDatum;// ����ʱ�䳤�� 3��СʱĬ��
			SatdynBasicDatum dynDatum_Init; // ��ʼֵ����ѧ�����Ϣ
			double    period_SolarPressure; // Ŀǰ�������Ǳ�ӹ��Ӧ���߶����, �ֶ�����Ķ������ͳһ
			double    period_AtmosphereDrag;
			double    period_EarthIrradiance; // + �����շ���
			double    period_RadialEmpAcc; // +
			double    period_TangentialEmpAcc; // +
			double    period_NormalEmpAcc; // +
			double    period_EmpiricalAcc;
			bool      on_Cst_EmpAcc_R;//R�������������ǳ�ֵԼ��
			bool      on_Cst_EmpAcc_T;//T����������Լ��
			bool      on_Cst_EmpAcc_N;//N����������Լ��
			bool      on_Cst_Maneuver_R;       // R����Ļ�����Լ��, R �� T ������ͬʱ����, Ϊ�Σ��ۿ���2020.9.5����
			bool      on_Cst_Maneuver_T;       // T����Ļ�����Լ��
			bool      on_Cst_Maneuver_N;       // N����Ļ�����Լ��
			bool      on_Cst_Radial_EmpAcc;    // R����������ֵ����Լ�� +
			bool      on_Cst_Tangential_EmpAcc;// T����������ֵ����Լ�� +
			bool      on_Cst_Normal_EmpAcc;    // N����������ֵ����Լ�� +
			float         priorsigma_RecPCO;
			float         priorsigma_RadialEmpAcc;  // R����ֵ����������ֵ
			float         priorsigma_TangentialEmpAcc;  // T����ֵ����������
			float         priorsigma_NormalEmpAcc;  // N����ֵ����������
			float         priorsigma_EmpAcc_R;
			float         priorsigma_EmpAcc_T;
			float         priorsigma_EmpAcc_N;
			float         priorsigma_EmpAcc_Sum;
			double        priorsigma_ManeuverAcc;
			bool      on_RecPCVFromIGS;
			bool      on_EstRecPCO_X; // �Ƿ���� X ��������PCO����
			bool      on_EstRecPCO_Y; // �Ƿ���� Y ��������PCO����
			bool      on_EstRecPCO_Z; // �Ƿ���� Z ��������PCO����
			bool      on_EstSysBias;  // �Ƿ����ϵͳ��ƫ����ϵͳ������Ҫ
			bool      on_UseWSB;     // �Ƿ�ʹ��WSB����
			bool      flag_amb;     // ģ���ȹ̶���ر�ʶ
			int       k_amb;        // ��¼ģ���ȹ̶��µ����Ĵ��� +
			vector<int> no_NA_List; // ��һ��NA�޷��̶���Ӧ�ĵ��������
			//TYPE_ATT_MODEL         attModelType; // ��̬��������
			vector<BetweenPass_SD_AmbEq_q>  sdAmb_List;  // ���μ䵥��ģ�����б����ڷǲ�ģ���ȹ̶��������ۿ���2019/07/06
			TimeAttitudeFile        attFile;
			map<string, AntPCVFile> pcvRecFileList; // ������λ���ı仯����ϵͳ��Ƶ���йأ�Ĭ�ϲ���ʼ��ʱΪ��
			map<string, AntPCVFile> crvRecFileList; // α��в����ı仯����ϵͳ��Ƶ���йأ�Ĭ�ϲ���ʼ��ʱΪ��
			map<string, POS3D>      pcoRecList;     // ������λ����ƫ�ƣ���ϵͳ��Ƶ���йأ�Ĭ�ϲ���ʼ��ʱΪ��
			Matrix                  m_matAxisBody2RTN;    // �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����, 2015/03/09	
			Matrix                  m_matAxisAnt2Body;    // ����ϵ���ǹ�ϵ��ת�ƾ����������߿��ܰ�װ�ڷ��춥����������2021/06/07
			SatMacroFile            m_satMacroFile;      // ���Ǻ�ģ���ļ�

			map<string, AntPCVFile> rcvRecFileList; // ���ջ�RCV�仯����ϵͳ��Ƶ���й�ϵ������GP1ΪGPS��P1Ƶ��CRV
			// ���̼���洢����: ��ϵͳ����, ���ϵͳ��Ԥ���� m_dataMixedSysList ��Ӧ
			// ��������: Ԥ���� m_dataMixedSysList �� N ��ϵͳ, �������п���ֻ�� M ��ϵͳ dataMixedSysList, ��ƥ��
			//           ÿ����վ�����ļ����������� "index_xx" �����ǲ�ͬ�ģ���Ҫ�� dataMixedSysList ��
			//           ����PCO���ƿ����Ǻ�ϵͳ����󶨻��ǲ�վ��Ŀǰ����Ͳ�վ�󶨣�
			// ����취: m_dataMixedSysList ��ֻ��������ѡȡ֮��ı�Ҫ���������Ϣ, �õ�������������Ϣ���� dataMixedSysList ��ȥ
			vector<LEOPOD_MixedSysData> dataMixedSysList;
			AntCorrectionBlk            pcvRecFromIGS;
			vector<Matrix>              attMatrixList;//
			vector<int>                 attFlagList;
			//vector<POS3D>               attXYZBodyList[3];
			vector<POS3D>               attXYZAntList[3];
			vector<int>                 validMixedEpochList; // ��¼�Ӳ���Ч���Ӧ��ʱ��λ��
			vector<string>              satGnssNameList;
			vector<TimePosVel>          interpOrbitlist;     // ��ֵ����
			vector<Matrix>              interpRtPartiallist; // ��ֵƫ��������
			vector<TDT>                 interpTimelist;      // ��ֵTDTʱ������
			vector<TimePosVel>          acOrbitList;         // ���ڲ�ֵ������� getEphemeris
			vector<Matrix>              acRtPartialList;     // ���ڲ�ֵƫ�������� getInterpRtPartial

			// �������
			int                   n0_EstParameters;         // ��¼�ϲ�����������ķֿ����, ���ڷֿ����
			int                   count_EstParameters;      // �����Ӳ�: count_EstDynParameters + count_EstAmbiguity + count_EstRecPCO + count_EstSysBias
			int                   count_EstRecPCO;          // PCO��������
			int                   count_EstAmbiguity;       // ģ���Ȳ�������
			int                   count_EstSysBias;         // ϵͳ���������
			int                   count_EstDynParameters;   // ����ѧ��������
			int                   count_EstClockParameters; // �Ӳ��������
			int                   n0_SolarPressure;         // �ֿ��ڲ�λ��, ��¼̫����ѹ������ʼλ��
			int                   n0_EarthIrradiance;       // �ֿ��ڲ�λ��, ��¼�����շ��������ʼλ�� +
			int                   n0_AtmosphereDrag;        // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			int                   n0_RadialForce;           // �ֿ��ڲ�λ��, ��¼R��������������ʼλ�� + 
			int                   n0_TangentialForce;       // �ֿ��ڲ�λ��, ��¼T��������������ʼλ�� + 
			int                   n0_NormalForce;           // �ֿ��ڲ�λ��, ��¼N��������������ʼλ�� + 
			int                   n0_EmpiricalForce;        // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			int                   n0_ManeuverForce;         // �ֿ��ڲ�λ��, ��¼��������������ʼλ��
			// 2021.03.28��Τ�����޸ģ��޸ļ��ٶȼ����ݶ�����򣬽��߶Ⱥ�ƫ������Ĺ��Ʒֿ�
			int                   n0_ScaleParaBegin;         // ��¼���ٶȼƱ�У������������������ѧ�����б��е�λ��
			int                   n0_xBiasParaBegin;         // ��¼���ٶȼ�x�����У������������������ѧ�����б��е�λ��
			int                   n0_yBiasParaBegin;         // ��¼���ٶȼ�y�����У������������������ѧ�����б��е�λ��
			int                   n0_zBiasParaBegin;         // ���ٶȼ�z�����У������������������ѧ�����б��е�λ��
			int                   n0_c1DriftParaBegin;       // ʱ��Ư����c1
			int                   n0_c2DriftParaBegin;       // ʱ��Ư����c2
			//
            int                   n0_RecPCO;                // �ֿ��ڲ�λ��, ��¼PCO������ʼλ��
			int                   n0_SysBias;               // �ֿ��ڲ�λ��, ��¼ϵͳƫ�������ʼλ��
			int                   n0_Ambiguity;             // �ֿ��ڲ�λ��, ��¼ģ���Ȳ�����ʼλ��
			SatdynBasicDatum      dynDatum_Est;  // ����ֵ����ѧ�����Ϣ
			vector<TimePosVel>    orbList; // ������ƽ��������Ԥ�����֣�
			vector<LEOPOD_CLKSOL> clkList; // �Ӳ���ƽ��������ѧ��+ ������ƽ�����˶�ѧ��
			Matrix                matN_aa; // ����ѧ������ģ���ȡ�����ƫ�ơ�ϵͳƫ��
			Matrix                matN_a;
			Matrix                matN_c;
			vector<double>        N_cc;
			vector<double>        N_cc_Inv;
			Matrix                matN_ac_cc_inv_na; 
			Matrix                matN_ac_cc_inv_ca;
             //Matrix                mat_a; // ��¼�������ƽ��: ����ѧ������ģ���ȡ�����ƫ�ơ�ϵͳƫ��
			 //Matrix                mat_c; // ��¼�������ƽ��: �Ӳ�
			Matrix                matda;
			Matrix                matdc;
			// �˶�ѧ���� + �ۿ�
			vector<Matrix>        Nxx_List; // �˶�ѧ 4 x 4��[x,y,z,clk] +
			vector<Matrix>    Nxx_inv_List;
			vector<Matrix>        Nxb_List;
			vector<Matrix>         nx_List;
			Matrix                      nb;
			Matrix                     Nbb;
		    vector<Matrix>         dx_List;      // ��¼λ�ò����Ľ���
			Matrix                      db;      // ��¼ϵͳƫ��+ģ���Ȳ����Ľ���   
			Matrix       matN_bx_xx_inv_xb;      // 
			Matrix       matN_bx_xx_inv_nx;
			//////////////////////////
			bool getEphemeris(TDT t, TimePosVel& interpOrbit, int nLagrange = 9);
			bool getEphemeris_ITRF(GPST t, TimePosVel& interpOrbit, int nLagrange = 9); // �ع�ϵ
			bool getInterpRtPartial(TDT t, Matrix& interpRtPartial);
			// ***************************
			bool on_ION_HO;       // �߽׵�������
			//*******************************
			// ����ÿ���������Ч���ݵ����clkList.validIndexͳ��, ����dynDatum_Est���������;�����ٶ�����ϲ�
			void mergeDynDatum(unsigned int min_SubSectionPoints);
			// ���¹���Ľ����
			void updateDynDatum();
			int updateDynDatum_Kinematic();
			// �������Ľ����
			void writeDynDatum();
			void writeDynDatum_SD();
			// �в�༭
			void ocResEdit(float factor = 3.0f, bool flag_UseSingleFrequency = false);
			void ocResEdit_Kine(float factor = 3.0f);
			void ocResEdit_Kine_New(float factor = 3.0f);
			void ocResEdit_NET(float factor = 3.0f);
			// ���²в�
			void ocResOutput();
			void ocResOutput_Kine();
			// �������������ʼλ��: n0_SolarPressure��n0_AtmosphereDrag��n0_EmpiricalForce��n0_ManeuverForce��n0_RecPCO��n0_SysBias
			void getEstParameters_n0();
			void getEstParameters_n0_Kinematic();
            // ����L_IFArcList�������ɶ�Ӧ�ķǲ�ģ�����б�
			void getAmbZDEqList(string sysFreqName, vector<LEOPOD_ZD_AmbEq> &ambZDEqList);
			//*******************************************************************************
			// ���ݷǲ�ģ�����б�ambZDEqList��������ģ��������ambSectionList���ۿ���2020.4.25 +
			void getAmbZDEqList_sd(string sysFreqName, vector<LEOPOD_ZD_AmbEq> &ambZDEqList);
			void getAmbZDEqList_zd(string sysFreqName, vector<LEOPOD_ZD_AmbEq> &ambZDEqList);
			vector<LEOPOD_SD_Section>   ambSectionList;
			int                         count_SDAmbiguity_All;
			int                         count_FixedSDAmbiguity_mw;
			int                         count_FixedSDAmbiguity_NA;
			double                      ratio_Delete_NA; // Delete��ռ����
			float     max_var_MW_singel; // �ǲ�mw������ֵ
			float     max_rms_res_singel; // �ǲ�IF����в���ֵ
			float     ratio_UpdateREFAmb_mw;
			float     ksb_LAMBDA_mw;
			float     max_AmbDecimal_mw;
			float     ksb_LAMBDA_NA;
			float     max_AmbDecimal_NA;
			double     max_Rms_mw_ZD;
			bool      on_SDAmbDebugInfo;
			double    minCommonViewTime; // ��С����ʱ��
			string    sysFreqName; // ����ͬһϵͳ��ͬƵ�����
			void getSDSection();
			void getSDSection_zd();
			// ����lambda��������Ѱ��, ������ѵ�ģ�������, ������ģ���ȹ̶���׼��
			bool lambdaSelected(Matrix matAFloat, Matrix matQahat, Matrix& matSqnorm, Matrix& matAFixed, Matrix matSelectedFlag);
			// ����ambSectionList���е���ģ���ȹ̶�
			void fixSDAmb(float ratio_UpdateREFAmb_mw, 
				          float max_AmbDecimal_mw, 
						  float max_AmbDecimal_NA, 
						  float ksb_LAMBDA_mw, 
						  float ksb_LAMBDA_NA, 
						  double COEFF_MW, 
						  double WAVELENGTH_N, 
						  bool on_SDAmbDebugInfo);
			void fixSDAmb_sd(float max_AmbDecimal_mw, 
						  float max_AmbDecimal_NA, 
						  double COEFF_MW, 
						  double WAVELENGTH_N, 
						  bool on_SDAmbDebugInfo);
			// ����ģ�����ѹ̶���ֻ�̶�խ��
			void fixSDAmb_zd(float max_AmbDecimal_mw, 
						  float max_AmbDecimal_NA, 
						  double COEFF_MW, 
						  double WAVELENGTH_N, 
						  bool on_SDAmbDebugInfo);
			//*******************************************************************************
			// ����Ӳ��ļ�
			void getRecClkFile(CLKFile& recClkFile); 
			void getRecClkFile_Kine(CLKFile& recClkFile); 

			//vector<TimePosVel>             m_leoOrbitList;            // ����ѧ�ο����
			//void setLeoOrbitList(vector<TimePosVel> leoOrbitList); // +
			//bool getLeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange = 8); // +

			LEOPOD_StaDatum()
			{
				on_ION_HO = false;
				count_MixedEpoch = 0;
				orbOutputInterval = 30.0;
				on_IsUsed_InitDynDatum = false;
				ArcLength_InitDynDatum = 3600.0 * 3.0;
				on_RecPCVFromIGS       = false;
				period_AtmosphereDrag = 3600 *  3.0;
				period_SolarPressure  = 3600 * 24.0; 
				period_RadialEmpAcc   = 3600 * 24.0;
				period_EarthIrradiance = 3600 * 24.0;
				period_TangentialEmpAcc = 3600.0 * 24.0;
				period_NormalEmpAcc   = 3600.0 * 24.0;
				period_EmpiricalAcc   = 3600 *  1.5;
				on_Cst_EmpAcc_R		  = false;
				on_Cst_EmpAcc_T		  = false;
				on_Cst_EmpAcc_N		  = false;
				on_Cst_Radial_EmpAcc  = false;
				on_Cst_Tangential_EmpAcc  = false;
				on_Cst_Normal_EmpAcc  = false;
				on_Cst_Maneuver_R     = false;
				on_Cst_Maneuver_T     = false;
				on_Cst_Maneuver_N     = false;
				on_EstSysBias         = false;
				on_UseWSB             = false;
				flag_amb              = false;
				k_amb                 = 0;
				//attModelType          = TYPE_ATT_Body2J2000;
				priorsigma_RecPCO         = 1.0f;
				priorsigma_EmpAcc_R       = 1E-9f; 
				priorsigma_EmpAcc_T       = 1E-6f;
				priorsigma_EmpAcc_N       = 1E-6f;
				priorsigma_RadialEmpAcc       = 1E-6f;
				priorsigma_TangentialEmpAcc   = 1E-6f;
				priorsigma_NormalEmpAcc       = 1E-6f;
				priorsigma_EmpAcc_Sum     = 1E-8f;
				priorsigma_ManeuverAcc    = 1E-4f;
				m_matAxisBody2RTN.MakeUnitMatrix(3);
				m_matAxisAnt2Body = TimeCoordConvert::rotate(PI, 1);
				// **************************************************************
				ratio_Delete_NA                  = 0.6;
				max_var_MW_singel                = 1.5f;
				max_rms_res_singel               = 0.01f;
				on_SDAmbDebugInfo         = false;
				ratio_UpdateREFAmb_mw     = 1.5f;
				max_AmbDecimal_mw         = 0.4f;    // ���ԭ��1m�������ʹ��, �ʵ��ſ���︡�����, ����20060107 04:49:20-05:23:00 4������ģ���ȸ����С��������0.3-0.4֮�䱻ɾ��
			    max_AmbDecimal_NA         = 0.5f;
				ksb_LAMBDA_mw             = 3.0f; 
				ksb_LAMBDA_NA             = 5.0f;
				minCommonViewTime         = 480;     // 8����
				max_Rms_mw_ZD             = 0.1;
				// **************************************************************
				
			}
			~LEOPOD_StaDatum()
			{
				rcvRecFileList.clear();
			}
		};
		typedef map<string, LEOPOD_StaDatum> StaDatumMap;
		struct LEOPOD_DD_Section
		{
			GPST   t0;
			GPST   t1;
			bool   on_Fixed_mw;        // ��¼ģ�����Ƿ�̶�
			float  max_Decimal_mw;     // ��¼ģ���ȹ̶������С������
			Matrix matDDFixedFlag_mw;  // ��¼ģ���ȹ̶��ɹ����
			Matrix matDDFixed_mw;      // ��¼ģ���ȹ̶�ֵ
			int    count_Fixed_mw;     // mw�̶�����
			bool   on_Fixed_NA;
			float  max_Decimal_NA;
			Matrix matDDFixedFlag_NA;
			Matrix matDDFixed_NA;
			int    count_Fixed_NA;     // NA�̶����� 
			vector<LEOPOD_SD_AmbEq> ambSDEqList;
			vector<LEOPOD_DD_AmbEq> ambDDEqList;
		};
		struct LEOPOD_KBRArcElement
		{   
			GPST   t;             // �۲�ʱ�����
			double obs;           // ԭʼKBR���, ����������ֵ
			double range_0;       // ������Ծ���ֵ
			double res;   
			double robustweight;  // ³�����Ƶ���Ȩ
			
			LEOPOD_KBRArcElement()
			{
				robustweight = 1.0;
				res = 0.0;
			}
		};

		struct LEOPOD_KBRArc 
		{
			double                       ambiguity;    
			vector<LEOPOD_KBRArcElement> obsList;
		};

		// ��վ��վ���ݽṹ, �����������ݺ��������ⲿ��
		struct LEOPOD_StaBaseline
		{
			int       id_priority; // ���ȼ���1,2,3
			float     w_function; // �۲�Ȩֵ
			float     ratio_UpdateREFAmb_mw;
			float     ksb_LAMBDA_mw;//Ĭ��3.0f
			float     max_AmbDecimal_mw;//���С������
			float     ksb_LAMBDA_NA;
			float     max_AmbDecimal_NA;
			bool      on_DDAmbDebugInfo;
			double    minCommonViewTime; // ��С����ʱ��
			double    max_var_mw_sd; // ����MW��Ϸ�����ֵ
  			string    sysFreqName; // ����ͬһϵͳ��ͬƵ�����
			string    staName_A; 
			string    staName_B;
			vector<LEOPOD_ZD_AmbEq>   ambZDEqList_A;
			vector<LEOPOD_ZD_AmbEq>   ambZDEqList_B;

			// ��һ�����ص㿼�������Ǽ����KBR�������Լ��
			// updateNet_NEQ_SQL ������Ҫ��Ӧ�޸ģ�����KBR�������Լ����ģ���ȵ����
			// �Ի���Ϊ��λ, ���� updateBL_KBRObsArc����ʼ�����Σ���ocKBRResEdit������Ȩֵ����ocKBRResOutput�����KBR�в
			// ���ò�վ���ֵ�����ֱ��ö�Ӧָ��ʱ���t��ƫ����
			bool bOn_KBR_i;       // ����i�Ƿ�ʹ��KBR����(�п���û��)���ۿ���2020.7.28
			graceKBR1BFile            graceKBRFile;
			//gracefoKBR1BFile          gracefoKBRFile;
			vector<LEOPOD_KBRArc>     KBRArcList;
			
			// �����ļ�
			LeoNetPODEditedSdObsFile editedSDObsFile;
			// ���̲���
			vector<LEOPOD_SD_AmbEq>   ambSDEqList;  // ��������, ����getDDSection��������ambSectionListʱ, �����ambSDEqList

			// �������
			int                       n0_KBRAmbiguity;
			vector<LEOPOD_DD_Section>   ambSectionList;
			int                       count_DDAmbiguity_All;
			int                       count_FixedDDAmbiguity_mw;
			int                       count_FixedDDAmbiguity_NA;

			// ���ݷǲ�ģ�����б�ambZDEqList_A��ambZDEqList_B�������ɵ���ģ�����б� ambSDEqList
			void getSDAmbEq();

			// ���ݵ���ģ�����б�ambSDEqList����˫��ģ��������ambSectionList
			void getDDSection();
			void getDDSection_new();

			// ����lambda��������Ѱ��, ������ѵ�ģ�������, ������ģ���ȹ̶���׼��
			bool lambdaSelected(Matrix matAFloat, Matrix matQahat, Matrix& matSqnorm, Matrix& matAFixed, Matrix matSelectedFlag);
			// ����ambSectionList����˫��ģ���ȹ̶�
			void fixDDAmb(float ratio_UpdateREFAmb_mw, 
				          float max_AmbDecimal_mw, 
						  float max_AmbDecimal_NA, 
						  float ksb_LAMBDA_mw, 
						  float ksb_LAMBDA_NA, 
						  double COEFF_MW, 
						  double WAVELENGTH_N, 
						  bool on_DDAmbDebugInfo);
			LEOPOD_StaBaseline()
			{
				bOn_KBR_i   = false;
				id_priority = 1;
				w_function = 1.0E+2;
				on_DDAmbDebugInfo         = false;
				ratio_UpdateREFAmb_mw     = 1.5f;
				max_AmbDecimal_mw         = 0.4f;    // ���ԭ��1m�������ʹ��, �ʵ��ſ���︡�����, ����20060107 04:49:20-05:23:00 4������ģ���ȸ����С��������0.3-0.4֮�䱻ɾ��
			    max_AmbDecimal_NA         = 0.5f;
				ksb_LAMBDA_mw             = 3.0f; 
				ksb_LAMBDA_NA             = 5.0f;
				minCommonViewTime         = 300; // 5����
				max_var_mw_sd             = 1.5;
			}
		};

		struct LEOPOD_DEF
		{
			bool          on_RecARP;  // �Ƿ���������PCO�ڸ�����������ȥͶӰ һ����˵�϶����밡
			bool          on_RecRelativity;
			bool          on_GraRelativity;
			bool          on_WeightElevation;
			bool          on_WeightGEO;
			bool          bOnNotUseGEO;
			bool          bOnUseOBX;//�Ƿ����������̬
			float         weightGEO;
			float         max_ArcInterval;
			int           max_OrbIterationCount;     // ����Ľ�������ֵ	
            int           max_AmbNum;                // ����Ľ���ģ���ȹ̶��������ֵ 	
			unsigned int  min_ArcPointCount;         // ���ι۲������ֵ, ����С�ڸ���ֵ�Ļ��ν���ɾ��
            unsigned int  min_ArcPointCount_SD;       // ����
            double        max_var_MW;
			double        max_var_MW_SD;         
			bool          flag_UseSingleFrequency;    // �Ƿ�ʹ�õ�Ƶ�۲����ݣ�1/2(L1+C1)
			double        max_pdop;               // ���ξ���������ֵ(��Ȩ��), ������ֵ�Ĺ۲�㽫����������
			int           min_EyeableSatCount_kine;   // ��С�������Ǹ���, ����˶�ѧ����
			int           min_EyeableSatCount;       // ��С�������Ǹ���, ����С�ڸ���ֵ����Ԫ�����������
			int           min_EyeableCommonnSatCount; // ��С���ӹ������Ǹ�����+
			float         min_Elevation;
			float         priorsigma_LIF;            // ������λ��Ȩ����
			//float         priorsigma_RecPCO;
			//float         priorsigma_RadialEmpAcc;  // R����ֵ������Լ��
			//float         priorsigma_EmpAcc_R;
			//float         priorsigma_EmpAcc_T;
			//float         priorsigma_EmpAcc_N;
			//float         priorsigma_EmpAcc_Sum;
			//float         priorsigma_ManeuverAcc;
			int           type_Cst_EmpAcc;           // ������Լ������, 0: ���Լ��; 1: ������Լ��; 2: ������ֵԼ��; 3: ���Լ�� + ������ֵԼ��; 4: ����Լ�� + ������ֵԼ��
			unsigned int  min_SubSectionPoints;      // ���ӷֶ��������С����
			bool          on_ocResEdit;              // �в�༭
			bool          on_ocResEdit_NET;          // �в�༭
			bool          on_ocResEdit_KBR;           // �в�༭
			float         priorsigma_KBR;            // ����KBR��Ȩ����

			float         ratio_ocEdit;
			bool          on_KinematicPROD;         // �˶�ѧ��Զ��죬�ۿ���2020.12.24
			bool          on_FixDDAmbiguity;
			bool          on_FixSDAmbiguity;        // ����ģ���ȹ̶����ۿ� +
			bool          on_FixZDAmbiguity;        // �ǲ�ģ���ȹ̶����ۿ���2020.2.8 +
			//bool          on_DDAmbDebugInfo;
			//float         ratio_UpdateREFAmb_mw;     // ����ο�ģ���ȸ�������
			//float         max_AmbDecimal_mw;
			//float         max_AmbDecimal_NA;
			//float         ksb_LAMBDA_mw;
			//float         ksb_LAMBDA_NA;
			float         min_OrbPosIteration;     // ����Ľ�����Сֵ
			float         min_OrbPosIteration_NET; // ������Ĺ���Ľ�����Сֵ
			int           max_num_edit;           // ���ӱ༭����
			double        threshold_initDynDatumEst; // ����ȷ�����
			bool          on_UsedKBR;

			int           ambiguityFixedType_MW;       // ����ģ���ȹ̶�����: 0-������Ԫ˫���ϵ; 1-�����ֱ�ӹ���˫��[�Ƽ�����] 

			LEOPOD_DEF()
			{
				on_KinematicPROD          = false;
				on_RecARP                 = true;
				on_RecRelativity          = true;
				on_GraRelativity          = true;
				on_WeightElevation        = true;
				on_WeightGEO              = false;
				bOnNotUseGEO              = false;
				bOnUseOBX                 = false;
				weightGEO                 = 1.0f / 3.0f;
				max_OrbIterationCount     = 10;
				max_AmbNum                = 8;
				min_ArcPointCount         = 30;
                min_ArcPointCount_SD      = 40;
                max_var_MW                = 0.5;  // �Ƽ�ֵ GRACE 0.5 ���ﲨ��, ����������α������ƫ��, ��Ҫ�ʵ��ſ�
                max_var_MW_SD             = 0.5;  // �Ƽ�ֵ GRACE 0.5 ���ﲨ��, ����������α������ƫ��, ��Ҫ�ʵ��ſ�
				max_ArcInterval           = 2000.0f;
				max_pdop                  = 4.5;
				min_EyeableSatCount_kine  = 5;
				min_EyeableSatCount       = 3;
				min_EyeableCommonnSatCount = 3;
				min_Elevation             = 5.0f;
				priorsigma_LIF            = 0.005f;
				priorsigma_KBR            = 0.0001f;
				type_Cst_EmpAcc           = 0;
				//priorsigma_RecPCO         = 1.0f;
				//priorsigma_EmpAcc_R       = 1E-9f; 
				//priorsigma_EmpAcc_T       = 1E-6f;
				//priorsigma_EmpAcc_N       = 1E-6f;
				//priorsigma_RadialEmpAcc   = 1E-6f;
				//priorsigma_EmpAcc_Sum     = 1E-8f;
				//priorsigma_ManeuverAcc    = 1E-4f;
				min_SubSectionPoints       = 30;      // �ݶ�Ϊ30����Ч��, ���10���Ӳ����൱��5����
				on_ocResEdit               = true;
				on_FixZDAmbiguity          = false;
				on_FixDDAmbiguity          = false;
				on_FixSDAmbiguity          = false;
				//on_DDAmbDebugInfo         = false;
				ratio_ocEdit               = 3.0f;
				min_OrbPosIteration        = 5.0E-3f;  // 5 mm
				min_OrbPosIteration_NET    = 1.0E-3f;  // 1.0 mm
				//ratio_UpdateREFAmb_mw     = 1.5f;
				//max_AmbDecimal_mw         = 0.4f;    // ���ԭ��1m�������ʹ��, �ʵ��ſ���︡�����, ����20060107 04:49:20-05:23:00 4������ģ���ȸ����С��������0.3-0.4֮�䱻ɾ��
			 //   max_AmbDecimal_NA         = 0.5f;
				//ksb_LAMBDA_mw             = 3.0f; 
				//ksb_LAMBDA_NA             = 5.0f;
				on_UsedKBR                  = false;
				on_ocResEdit_KBR            = false;
				on_ocResEdit_NET            = false;
				threshold_initDynDatumEst   = 300.0;
			    ambiguityFixedType_MW       = 1;
				max_num_edit                = 1;
				flag_UseSingleFrequency     = false;
			}
		};

		class GNSSLeoNetPOD : public SatdynBasic
		{
		public:
			GNSSLeoNetPOD(void);
		public:
			~GNSSLeoNetPOD(void);
		public:
			void setSP3File(SP3File sp3File); 
			void setCLKFile(CLKFile clkFile);
			void setIONFile(Ionex1_0_File ionFile); 
			bool loadSP3File(string strSp3FileName); 
			bool loadCLKFile(string strCLKFileName);
			bool loadCLKFile_rinex304(string strCLKFileName);
			bool loadIONFile(string strIONFileName); 
			bool adamsCowell_Interp_Leo(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h = 10.0, int q = 11); 
            bool initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3); 
			
			// ���Ƕ���ѧ��ϣ�+
			bool dynamicGNSSPOD_pos(StaDatumMap::iterator it_Sta, double interval = 30.0, bool bInitDynDatumEst = false);

            void orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0); 
			void orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAcc> &forecastOrbList, double interval = 30.0); 
			void orbitExtrapolation_jerk(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAccJerk> &forecastOrbList, double interval = 30.0);
		public:
			bool adamsCowell_ac(TDT t0, TDT t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 10.0, int q = 11);
			bool mainNetPOD_MixedGNSS(GPST t0, GPST t1, double interval = 30.0); // ��������ѧ���Զ���+����˫��ģ���ȵ���Զ���
			bool mainNetPOD_MixedGNSS_DD(GPST t0, GPST t1, double interval = 30.0,char cSatSystem = 'G'); // ˫��������������ѧ+�˶�ѧ��Զ���
			bool mainNetPOD_MixedGNSS_DD_L1(GPST t0, GPST t1, double interval = 30.0, char cSatSystem = 'G'); // ˫��������������ѧ+�˶�ѧ��Զ���,��ƵL1�̶���
			bool mainNetPOD_MixedGNSS_Kinematic(GPST t0, GPST t1, double interval = 30.0); // �˶�ѧ���Զ��죬�ݲ����˶�ѧ���Զ�������Ͻ�����Զ���
			// ��õ��㶨λ�ļ��ξ�������pdop��+ �ۿ���2020.2.1
			bool pdopSPP(int index_P1, int index_P2, double freq_L1, double freq_L2, char cSys, Rinex2_1_LeoEditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop);
			bool mainFuncSdPreproc(LEOPOD_StaBaseline staBL, LeoNetPODEditedSdObsFile& editedSdObsFile);
		private:
			double  m_stepAdamsCowell;
			SP3File m_sp3FileJ2000;	// + ��¼J2000��Gnss���ǹ��, ����updateSta_AdamsCowell���ֺ��������Ե�ʹ��
			map<string, AntCorrectionBlk> m_mapGnssPCVBlk; // + ��¼Gnss���ǵ�PCV�������, ����updateSta_AdamsCowell���ֺ��������Ե�ʹ��
			bool updateSta_ObsArc(StaDatumMap::iterator it_Sta); // ��ʼ���۲����ݻ�����Ϣ
			bool updateSta_ObsArc_Kinematic(StaDatumMap::iterator it_Sta); // ��ʼ���۲����ݻ�����Ϣ,�˶�ѧ
			void updateSta_AdamsCowell(StaDatumMap::iterator it_Sta); // + ���²�վ������Ϣ������ѧ
			void updateSta_CorrectData(StaDatumMap::iterator it_Sta); // + ���²�վ�������ݣ��˶�ѧ
			void updateSta_NEQ(StaDatumMap::iterator it_Sta);         // + ���²�վ�����̣�����ѧ
			void updateSta_NEQ_Kinematic(StaDatumMap::iterator it_Sta); // + ���²�վ������,�˶�ѧ
			void updateSta_SOL(StaDatumMap::iterator it_Sta); // + ���²�վ��
			void updateSta_SOL_Kinematic(StaDatumMap::iterator it_Sta); // + ���²�վ��,�˶�ѧ
			void updateNET_KBRArc(LEOPOD_StaBaseline &staBL); // + ��ʼ��KBR������Ϣ
			void updateNet_NEQ_SQL(); // + �������������̺�����
			void updateNet_NEQ_SQL_Kinematic(); // + �������������̺����⣬�˶�ѧ
			void updateKBRRes(float factor = 3.0); // +����KBR�в�༭
		public:
			LEOPOD_DEF                     m_podDefine;
			CLKFile                        m_clkFile;			   // �����Ӳ������ļ�
			SP3File                        m_sp3File;			   // �������������ļ�
			wsbFile                        m_wsbFile;              // wsb�ļ�
			Ionex1_0_File                  m_ionFile;              // IGS����������Ʒ
			igs05atxFile			       m_atxFile;		       // ���������ļ�
			svnavMixedFile                 m_svnavMixedFile;
			GNSSYawAttitudeModel           m_gymMixed;             // +
			GNSS_AttFile                   m_gnssAttFile;        // GNSS��̬�����ļ�
			//POS3D                        m_arpAnt;		       // ����ƫ���� (��վPCO)
			StaDatumMap                    m_mapStaDatum;          // �������� + 
			vector<LEOPOD_MixedSys>        m_dataMixedSysList;     // ���ϵͳ���� +
			vector<LEOPOD_StaBaseline>     m_staBaselineList;      // ��ʱ�������ⲿ���� + 
			vector<O_CResEpoch>            m_ocResP_IFEpochList;
			vector<O_CResEpoch>            m_ocResL_IFEpochList;
		};
	}
}
