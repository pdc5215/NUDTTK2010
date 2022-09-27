#pragma once
#include"SatdynBasic.hpp"
#include"constDef.hpp"
#include"structDef.hpp"
#include"MathAlgorithm.hpp"
#include"Rinex2_1_EditedObsFile.hpp"
#include"Rinex2_1_EditedSdObsFile.hpp"
#include"dynPODStructDef.hpp"
#include"lambda.hpp"
#include"igs05atxFile.hpp"
#include"SP3file.hpp"
#include<windows.h>
#include<map>
#include"StaOceanLoadingDisplacementFile.hpp"
#include"svnavFile.hpp"
#include"CLKfile.hpp"
#include"TROZPDFile.hpp"
#include"Troposphere_model.hpp"
#include "GPSYawAttitudeModel1995.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"

using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::Math;
using namespace NUDTTK::LAMBDA;
//  Copyright 2012, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	namespace GPSPod
	{
		// ������������ṹ
		struct GPSMeoSatDynPODPara
		{
			int                          max_OrbitIterativeNum;     // ����Ľ��������ֵ		
			unsigned int                 min_arcpointcount;         // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			int                          min_eyeableGNSScount;      // ����Ԫ��С�������Ǹ���, ����С�� min_eyeableGPScount ����Ԫ�����������
			double                       min_elevation;
			double                       period_SolarPressure;  
			double                       apriorityRms_PIF;          // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_LIF;          // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_STA;          // �����վλ�����꾫��, ���ڲ�վ����Լ�����̼�Ȩ����
			double                       apriorityRms_TZD_abs;      // �����վ������춥�ӳپ���, ���ڵ�����춥�ӳپ���Լ�����̼�Ȩ����
			double                       apriorityRms_TZD_rel;      // �������ڵ�����춥�ӳٲ��������Լ�����̼�Ȩ����
			double                       apriorityWet_TZD;          // ��վ������ʪ������������ֵ
			double                       apriorityWet_TZD_period;   // ��վ������ʪ������������
			double                       min_Wet_TZD_ncount;        // ���������������������ݸ���
			double                       max_arclengh;    
			bool                         bOn_AmbiguityFix;          // �Ƿ�̶�����ģ����
			bool                         bOn_StaSolidTideCor;       // �Ƿ���в�վ����Ĺ��峱����
			bool                         bOn_GPSRelativity;         // �Ƿ���� GPS ��������۸���,���ƫ�ĸ���
			bool                         bOn_GraRelativity;         // �Ƿ�����������������۸���
			bool                         bOn_GPSAntPCOPCV;          // �Ƿ���� GPS �������� PCO/PCV ����
			bool                         bOn_RecAntARP;             // �Ƿ���н��ջ����� ARP ����
			bool                         bOn_RecAntPCOPCV;          // �Ƿ���н��ջ����� PCO/PCV ����
			bool                         bOn_PhaseWindUp;
			bool						 bOn_TropDelayCorrection;	// �Ƿ���ж������ӳ�����(����ģ������)
			bool                         bOnEst_StaPos;             // �Ƿ���в�վλ�øĽ�
			bool                         bOnEst_StaTropZenithDelay; // �Ƿ���в�վ�������춥�ӳٹ���
			bool                         bOn_StaOceanTides;
			bool                         bOnEst_ERP;                // �Ƿ���е�����ת��������
			bool                         bOn_WeightElevation;       // �Ƿ���и߶ȽǼ�Ȩ
			bool                         bOn_GYM95;                 // �Ƿ�ʹ��GYM95ģ�ͻ�ȡ������̬��������λ���ƺ�����������λ��������
			bool                         bOn_Used_delta_u;          // �Ƿ�ʹ��Delta_u��Ϊ̫����ѹ������
			TYPE_SOLARPRESSURE_MODEL     solarPressure_Model;       // ̫����ѹģ��
			double                       maxdistance_Ambfixed_short;// �̻��߳�����ֵ, ���ڳ�ʼģ���Ȳ��̶�
			double                       maxdistance_Ambfixed_long; // �����߳�����ֵ, ���ڸó��ȵĻ���ģ���Ȳ��̶�
			double                       threshold_slip_DDDEdit;
			double                       threshold_outlier_DDDEdit;
			double                       robustfactor_OC_edited;
			bool                         bOnConstraints_GPSEphemeris; 
			double                       apriorityRms_GPSEphemeris; // GPS���������ľ���
			double                       max_var_MW;
			double                       threhold_LAMBDA_ksb_MW;
			double                       threhold_LAMBDA_ksb_NA;
			double                       threhold_MW_Decimal;         // ����ģ���ȸ����С������
			double                       threhold_NA_Decimal;         // խ��ģ���ȸ����С������
			double                       min_arclengh_min_5;          // ��С���γ���
			bool                         bOn_DebugInfo_AmFixed;
			bool                         bOnConstraints_SolarPressure4; 
			double                       apriorityRms_SolarPressure4;
			int                          sampleSpan;
			int                          min_countobs_clkest;        // �Ӳ����ʱ, ÿ����վ�������Ӷ�Ӧ����С�۲����ݸ���
			int                          zpdProductInterval;         // ����Ķ������Ʒ�������(s)
			//////////////////////////////////////////////////////////�˲��ֲ������������������ڼ��ݱ���,2014/09/29					
			int                          OrbitIterativeNum;          // �ڲ��̶�ģ���ȵ�����£����Ƶ�������
			bool                         bOn_StaWeight;              // �Ƿ���ݲ�վ��Ȩ
			bool                         bOn_SatWeight;              // �Ƿ�������Ǽ�Ȩ
			bool                         bOn_SDEdited;               // �Ƿ���е���༭
			bool                         bOn_GEOSolut;               // �Ƿ����GEO����			
			bool                         bOn_GPSzpd;                 // �Ƿ�ʹ��GPS���ƵĶ��������
			bool                         bOn_ArcEdited;              // �Ƿ���Ҫ���ݵ������ε�Ȩֵ
			bool                         bOn_RefSat;
			bool                         bOn_GEOSRP_DC1;              // �Ƿ����GEO���ǹ�ѹDC1����
			bool                         bOn_GEOSRP_DS1;              // �Ƿ����GEO���ǹ�ѹDS1����			
			bool                         bOn_GEOSRP_YC1;              // �Ƿ����GEO���ǹ�ѹYC1����
			bool                         bOn_GEOSRP_YS1;              // �Ƿ����GEO���ǹ�ѹYS1����
			bool                         bOn_GEOSRP_BC1;              // �Ƿ����GEO���ǹ�ѹBC1����
			bool                         bOn_GEOSRP_BS1;              // �Ƿ����GEO���ǹ�ѹBS1����	
			TYPE_SOLARPRESSURE_MODEL     solarPressure_Model_BDYF;       // yaw-fixed��̬̫����ѹģ�ͣ�2015/11/13��������	
			TYPE_SOLARPRESSURE_MODEL     solarPressure_Model_BDYS;       // yaw-steering��̬̫����ѹģ��
			///////////////////////////////////////////////////////////
			vector<string>               clkFixedStaNameList;        // ָ���Ӳ�ο�վ�б�, �ȵ·�, 2015/09/06  
			// ***********************************************************************************************
			bool                         bOn_EstSatPCO_X;               // ����PCO���ƿ���,�ۿ���2019.09.25
			bool                         bOn_EstSatPCO_Y;               
			bool                         bOn_EstSatPCO_Z;               
			bool                         bOn_EstSatPCV;                 // ����PCV���ƿ���
			TYPE_OCEANTIDE_MODEL         oceanTideType_Model;        // ��������
			bool                         bOn_BDSYawAttitude;         // �Ƿ�ʹ�ñ���ƫ����̬ģ�ͻ�ȡ������̬��������λ���ƺ�����������λ��������
			bool                         on_UsedInShadow;            // �Ƿ�ʹ�õ�Ӱ�ڼ�����
		    double                       weightInShadow;              // ��Ӱ�ڼ����ݽ�Ȩ����
			// **********************************************************************************************8
			double                       period_EarthIrradiance;      // �������, 2017/10/03
			TYPE_EARTHRADIATION_MODEL    earthRadiation_Model;        // �����շ���ѹģ��
			TYPE_SATELLITESHAPE_MODEL    satelliteShape_Model;        // ������״ģ��
			bool                         bOn_earthRadiation;          // �Ƿ���Ƶ����շ��俪��
			bool                         bOn_earthRadiation_noest;          // �Ƿ��������շ��俪��
			// ***********************************************************************************************

			GPSMeoSatDynPODPara()
			{
				max_OrbitIterativeNum     = 10;				
				min_arcpointcount         = 12;
				min_eyeableGNSScount      = 3;
				min_elevation             = 7.0;
				period_SolarPressure      = 3600 * 24.0;
				apriorityRms_PIF          = 1.0;
				apriorityRms_LIF          = 0.01;
				apriorityRms_STA          = 1.0;			
				apriorityRms_TZD_abs      = 0.5;
				apriorityRms_TZD_rel      = 0.05;
				apriorityWet_TZD          = 0;                      // ��վ������ʪ������������ֵ
				apriorityWet_TZD_period   = 3600 * 2;               // ����                
				min_Wet_TZD_ncount        = 12;
				max_arclengh              = 3600;

				bOn_AmbiguityFix          = true;
				min_arclengh_min_5        = 60;  // 1Сʱ
				threhold_LAMBDA_ksb_MW    = 3.0;
				threhold_LAMBDA_ksb_NA    = 3.0;
				max_var_MW                = 2.0;  
				threhold_MW_Decimal       = 0.30; // �޳�����˫����ƫ�����������ĵ�, ����ģ���ȹ̶�������� 
				threhold_NA_Decimal       = 0.40; // խ��ģ���ȸ�����Ϊ׼ȷ, ���ʵ��ſ�������
				bOn_DebugInfo_AmFixed     = false;
				min_countobs_clkest       = 1;

				bOn_GPSRelativity         = true;
				bOn_GraRelativity         = false;
				bOn_GPSAntPCOPCV          = true;
				bOn_RecAntARP             = true;
				bOn_RecAntPCOPCV          = true;
				bOn_PhaseWindUp           = true;
				bOn_TropDelayCorrection   = true;
				bOnEst_StaPos             = true;
				bOnEst_StaTropZenithDelay = true;
				bOnEst_ERP                = true;
				bOn_StaSolidTideCor       = true;
				bOn_StaOceanTides         = true;
				bOn_WeightElevation       = false;
				bOn_GYM95                 = false;
				bOn_Used_delta_u          = true;
				solarPressure_Model       = TYPE_SOLARPRESSURE_9PARA;

				maxdistance_Ambfixed_short= 2500000.0;
				maxdistance_Ambfixed_long = 5000000.0;
				threshold_slip_DDDEdit    = 0.15;
				threshold_outlier_DDDEdit = 0.15;
				robustfactor_OC_edited    = 2.5;
				bOnConstraints_GPSEphemeris = true;
				apriorityRms_GPSEphemeris   = 0.30;
				bOnConstraints_SolarPressure4 = false;
				apriorityRms_SolarPressure4   = 1.0E-8;
				sampleSpan                  = 120;
				zpdProductInterval          = 3600 * 2;				
				OrbitIterativeNum           = 3;	 // ������Σ�����
				
				bOn_StaWeight               = false;
				bOn_SatWeight               = false;
				bOn_SDEdited                = false;
				bOn_GEOSolut                = true;
				bOn_GPSzpd                  = false;
				bOn_ArcEdited               = false;
				bOn_RefSat                  = false;
				bOn_GEOSRP_DC1              = false;
				bOn_GEOSRP_DS1              = true;
				bOn_GEOSRP_YC1              = false;
				bOn_GEOSRP_YS1              = false;	
				bOn_GEOSRP_BC1              = true;
				bOn_GEOSRP_BS1              = false;		
				solarPressure_Model_BDYF    = TYPE_SOLARPRESSURE_9PARA;
				solarPressure_Model_BDYS    = TYPE_SOLARPRESSURE_9PARA;
				bOn_EstSatPCO_X                = false;
				bOn_EstSatPCO_Y                = false;
				bOn_EstSatPCO_Z                = false;
				bOn_EstSatPCV                  = false;
				oceanTideType_Model         = TYPE_OCEANTIDE_CSR4;
				bOn_BDSYawAttitude          = false;
				on_UsedInShadow             = true;
				weightInShadow              = 0.2;
				earthRadiation_Model        = TYPE_EARTHRADIATION_ANALYTICAL;
				satelliteShape_Model        = TYPE_SATELLITESHAPE_BALL;
				period_EarthIrradiance      = 3600 * 24.0;
				bOn_earthRadiation          = false;
				bOn_earthRadiation_noest    = true;
			}
		};

		// �Ӳ����ݽṹ
		struct CLKEST_ObsElement
		{
			string          name_Sta;
			int             id_Sat;
			int             id_Ambiguity;
			int             id_Arc;
			int             nObsTime;
			double          distance; // ���ξ���, ����������, ƫ��, ������תӰ��
			double          obs_L_IF;
			double          weight_L_IF;
			double          obs_P_IF;
			double          weight_P_IF;
			double          ambiguity;
			double          oc_P_IF;
			double          oc_L_IF;
			//double          nadir; // ��׽�
			// 2016/01/11, ���Ӷ��������, �ȵ·�
			int             id_zenithDelay_0;     
			double          zenithDelay;           // ����ֵ
			double          zenithDelay_partial_0; // ƫ����
			double          zenithDelay_partial_1; 
		};
		// �Ӳ������Ԫ���ݽṹ
		struct CLKEST_Epoch
		{
			vector<CLKEST_ObsElement> obsList;
			string                    name_FixedSta;        // ��վ����
			GPST                      t;                    // ��Ԫʱ��
			bool                      bSuccess;
			vector<int>               id_SatList;           // ��Ч�����б�
			vector<double>            clkBias_SatList;      // �����Ӳ���ƽ���б�
			vector<double>            clkBiasSigma_SatList; // �����Ӳ���ƽ���б�
			vector<string>            name_StaList;         // ��Ч��վ�б�
			vector<double>            clkBias_StaList;      // ��վ�Ӳ���ƽ���б�
			vector<double>            clkBiasSigma_StaList; // ��վ�Ӳ���ƽ���б�
			Matrix                    matN_c;               // �������̶�, ��5min�����ʿ�ʹ��, 30sec�����ʲ��ý�Լ�ڴ�
			Matrix                    matN_cc_inv;          // �������̶�, ��5min�����ʿ�ʹ��, 30sec�����ʲ��ý�Լ�ڴ�
			Matrix                    matN_cb;              // �������̶�, ��5min�����ʿ�ʹ��, 30sec�����ʲ��ý�Լ�ڴ�
			Matrix                    matdc;
			vector<int>               indexAmbList;         // ��¼ÿ���۲����ݶ�Ӧ��ģ�������
			map<int, int>             mapIdSat;
			map<string, int>          mapIdSta;             // �����ο�վ����
			map<string, int>          mapIndexZenithDelay;  // ��¼ÿ����վ��Ӧ���춥�������ӳ����, ���� 2 ��, mapIndexZenithDelay[name] �� mapIndexZenithDelay[name] + 1
			//map<int, int>             mapIndexSatDyn;       // ��¼ÿ�����Ƕ�Ӧ�Ķ���ѧ�������
			//map<int, int>             mapIndexSatDyn;       // ��¼ÿ�����Ƕ�Ӧ��PCOPCV�������
			int                       k0_SatEphemeris;      // ��ѧ������ʼλ��
			int                       k0_SatPCOPCV;         // PCOPCV������ʼλ��
			int                       k0_ERP;			
			bool create_Sat_Sta_List(string name_FixedSta, int count_obs_Min = 1); // ���� obsList ����id_SatList_t��name_StaList_t
			CLKEST_Epoch()
			{
				name_FixedSta = "";
				bSuccess = false;
			}
		}; 
		// �Ӳ���Ʋ����ṹ
		struct CLKEST_Parameter
		{
			map<int, int>         mapIndexSat;      // ��Ч���������б�
			map<string, int>      mapIndexSta;      // ��Ч��վ�����б�
			vector<CLKEST_Epoch>  clkEpochList_5min;
			vector<CLKEST_Epoch>  clkEpochList;

			Matrix                matN_bb;          // ģ���ȡ�������
			Matrix                matN_b;
			Matrix                matdb;
		};

		typedef struct {int valid; int id_arc; double obscorrected_value;} cvElement; // correctedvalueElement
		typedef struct {map<int, cvElement> mapDatum;} cvEpoch;   // correctedvalueEpoch

        struct NETEST_Parameter
		{
			string             pathSp3File_input;    // ����sp3����ļ�, �ǿ�
			string             pathSp3File_output;   // ���sp3����ļ�
			string             pathTroFile_input;    // ����������ļ�
			string             pathTroFile_output;   // ����������ļ�
			string             pathSinexFile_input;  // �����վ���ļ�
			string             pathSinexFile_output; // �����վ���ļ�
			SatOrbEstParameter paraSatOrbEst;        // ������ƽ��
			CLKEST_Parameter   paraClkEst;           // �Ӳ���ƽ��
			GPST               t0;
			GPST               t1;                   
			double             h_sp3;                // ������
			double             h_clk;                // �Ӳ���

			NETEST_Parameter()
			{
				h_sp3 = 900.0;
				h_clk = 300.0;
			};

		};

		struct CLKEST_AmbFixElement
		{   
			int    nObsTime;      // �۲�ʱ�����, �뽵�����е�����ʱ��һ��
			BYTE   id_Sat;        // ���Ǻ�
			int    id_Ambiguity;
			double ambiguity_mw;  // ����ģ���ȹ۲�ֵ
			double ambiguity_IF;  // ����������ģ�������
			int    id_Arc_A;          // �ǲ�������Ҫ
			int    id_Arc_B;          // �ǲ�������Ҫ
			int    id_Ambiguity_GL_A; // �ǲ�������Ҫ
			int    id_Ambiguity_GL_B; // �ǲ�������Ҫ
			
			CLKEST_AmbFixElement()
			{
			}
		};

		typedef map<int, CLKEST_AmbFixElement> CLKEST_AmbFixElementMap;

		struct CLKEST_AmbFixArc 
		{
			BYTE                    id_Sat;   
			int                     id_Ambiguity;
			double                  ambiguity_mw;
			double                  ambiguity_IF;
			int                     id_Ambiguity_GL_A;   // �ǲ�������Ҫ
			int                     id_Ambiguity_GL_B;   // �ǲ�������Ҫ
			CLKEST_AmbFixElementMap obsList;
		};

		struct CLKEST_AmbFixEpoch 
		{
			int                          nObsTime;             // �۲�ʱ�����
			int                          id_DDObs;             // ˫��GPS�ο����ǵĹ۲��������
			int                          id_DDRefSat;          // ˫��GPS�ο��������
			int                          id_DDRefAmbiguity;    // ˫��ο�ģ�������
			int                          id_DDRefAmbiguity_GL_A;  // �ǲ�������Ҫ
			int                          id_DDRefAmbiguity_GL_B;  // �ǲ�������Ҫ
			vector<CLKEST_AmbFixElement> obsSatList;        
			
			CLKEST_AmbFixEpoch()
			{
				id_DDRefSat       =  NULL_PRN;
				id_DDRefAmbiguity = -1;
				id_DDObs          = -1;
			}
		};

		struct CLKEST_AMBIGUITY_SECTION
		{
			int                          id_t0;                        // ��ʼʱ��
			int                          id_t1;                        // ����ʱ��
			vector<CLKEST_AmbFixArc>     mw_ArcList;                   // ��¼�ֶκ󻡶�����
			vector<CLKEST_AmbFixEpoch>   mw_EpochList;                 // ��¼�ֶκ�ʱ������,  ����˫��ʱ��Ҫ����ʱ�̽���
			bool                         bDDAmFixed_MW;                // ��ǿ���ģ��������̶��Ƿ�ɹ�
			Matrix                       matDDFixedFlag_MW;            // ���ÿ������ģ���ȹ̶��Ƿ�ɹ�, 0: δ�̶�, 1: �ѹ̶�
			vector<double>               ambiguity_DD_MW_List;         // ÿ������ģ���ȹ̶����, δ�̶��Ĳ��ø����
			vector<double>               ambiguity_DD_MW_Float_List;   // ÿ������ģ���ȸ����
			bool                         bDDAmFixed_NA;                // ���խ��ģ��������̶��Ƿ�ɹ�
			Matrix                       matDDFixedFlag_NA;            // ���ÿ��խ��ģ���ȹ̶��Ƿ�ɹ�
			vector<double>               ambiguity_DD_NA_List; 
			vector<double>               ambiguity_DD_NA_Float_List;   // ÿ��խ��ģ���ȸ����

			bool ArcList2EpochList(vector<CLKEST_AmbFixArc>& arcList, vector<CLKEST_AmbFixEpoch>& epochList, int min_arcpointcount = 20, bool bOn_Loop = true);
		};

		struct CLKEST_DD_AmbEq
		{
			int    id_DDRefAmbiguity_GL_A;
			int    id_DDRefAmbiguity_GL_B;
			int    id_Ambiguity_GL_A;
			int    id_Ambiguity_GL_B;
			double ambiguity_DD_IF;
			// ���ڼ��
			double ambiguity_DD_MW;
			double ambiguity_DD_NA;
			double ambiguity_DD_MW_Float;
			double ambiguity_DD_NA_Float;
			string nameSta_A;
			string nameSta_B;
			int    id_REFSat;
			int    id_Sat;
		};

		struct CLKEST_FCBs_Eq
		{
			string staName;
			double mwFCBs;
			double naFCBs;
			int    SD_MW_INT;
			int    SD_NA_INT;
			double mwFCBs_cos; // ���Ǻ����任���FCBs
			double mwFCBs_sin;
			double naFCBs_cos;
			double naFCBs_sin;
			bool   bUsed_mw;   // ���ֵ����ʹ�ñ��
			bool   bUsed_na;
		};

		struct CLKEST_FCBs_Sat
		{
			int                     id_Sat_REF;
			int                     id_Sat;
			vector<CLKEST_FCBs_Eq>  eqFCBsList;
			double                  mwFCBs_mean; // ƽ������FCBs
			double                  mwFCBs_var;
			double                  naFCBs_mean; // ƽ��խ��FCBs
			double                  naFCBs_var;

			void robustStatMeanFCBs_mw(); // ����ƽ������FCBs�����׼��, ����ÿ�����εĿ���FCBs������
			void robustStatMeanFCBs_na(); // ����ƽ��խ��FCBs�����׼��, ����ÿ�����ε�խ��FCBs������
		};

		// GPS �������ܶ�����(SatdynBasic��������)
		class GPSMeoSatDynPOD : public SatdynBasic
		{
		public:
			GPSMeoSatDynPOD(void);			
		public:
			~GPSMeoSatDynPOD(void);
		private:
			void weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			bool adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 75.0, int q = 11);			
		public:
			static bool getStaBaselineList_MiniPath(vector<POS3D> staList, vector<int>& staBaseLineIdList_A, vector<int>& staBaseLineIdList_B);
			static bool generateEditedSdObsFile(Rinex2_1_EditedObsFile editedObsFile_A, Rinex2_1_EditedObsFile editedObsFile_B, Rinex2_1_EditedSdObsFile& editedSdObsFile);
			static bool lambdaSelected(Matrix matAFloat, Matrix matQahat, Matrix& matSqnorm, Matrix& matAFixed, Matrix matSelectedFlag);
		public:	
			bool obsTriDiFFEdited_LIF(int index_L1, int index_L2, int id_sat, Rinex2_1_EditedSdObsEpoch epoch_j_1, Rinex2_1_EditedSdObsEpoch epoch_j, map<int, cvElement> &mapCvDatum_j_1, map<int, cvElement> &mapCvDatum_j, double &max_res_ddd, bool &slipFlag, double threshold_slip = 0.15);
			bool mainTriDiFFEdited(SP3File &sp3File, Rinex2_1_EditedSdObsFile &editedSdObsFile, POS3D posAnt_A, POS3D posAnt_B, string outputFileName = "");
			bool mainSDEpochDiffEdited(SP3File &sp3File, Rinex2_1_EditedSdObsFile &editedSdObsFile, POS3D posAnt_A, POS3D posAnt_B, string outputFileName = "");
			bool sp3Fit(string strSp3FilePath, Sp3FitParameter& paraSp3Fit, bool bOnEst_EOP = true, string outputSp3FitFilePath = "");
			bool mainNetPod_dd(string inputSp3FilePath, string outputSp3FilePath, SatOrbEstParameter& paraSatOrbEst, GPST t0, GPST t1, double h_sp3 = 900.0);
			bool mainClkEst_iterator(string inputSp3FilePath, string inputTrozpdFilePath, string inputSinexFilePath, string outputClkFilePath, CLKEST_Parameter &paraClkEst, GPST t0, GPST t1, double h_clk = 30.0, bool bUsed_Phase = true);
			bool mainClkEst_zd(string inputSp3FilePath, string inputTrozpdFilePath, string inputSinexFilePath, string outputClkFilePath, CLKEST_Parameter &paraClkEst, GPST t0, GPST t1, double h_clk = 30.0, string outputTrozpdFilePath = "", string outputSp3FilePath = "", double h_sp3 = 900.0);
			//// PCO+PCV���ƣ�2019/09/24, �ۿ�
			//bool mainClkEst_zd_pcopcv(string inputSp3FilePath, string inputTrozpdFilePath, string inputSinexFilePath, string outputClkFilePath, CLKEST_Parameter &paraClkEst, GPST t0, GPST t1, double h_clk = 30.0, string outputTrozpdFilePath = "", string outputSp3FilePath = "", double h_sp3 = 900.0);
			// �Ӳ����
			bool mainClkEst_Gamit(string inputSp3FilePath, string outputClkFilePath, CLKEST_Parameter &paraClkEst, GPST t0, GPST t1, double h_clk = 300.0);
			// ���Դ���
			bool initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3);
			bool dynamicPOD_pos(vector<TimePosVel>  orbitlist, SatdynBasicDatum &dynamicDatum, GPST t_forecastBegin, GPST t_forecastEnd,  vector<TimePosVel> &orbitlist_forecast, double interval = 30.0, bool bforecast = true);
		public:
			GPSMeoSatDynPODPara             m_podParaDefine;
			map<int, AntCorrectionBlk>      m_mapGPSAntCorrectionBlk;
			StaDatumMap                     m_mapStaDatum;
			vector<StaBaselineDatum>        m_staBaseLineList;
			svnavFile                       m_svnavFile;	      // GPS����ƫ��
			igs05atxFile			        m_AtxFile;			  // ���������ļ�
			StaOceanLoadingDisplacementFile m_staOldFile;         // �����ļ�
			GPSYawAttitudeModel1995         m_GYM95;              // ��ȡ��̬����ģʽ�µ�������̬
			// �ۿ���2019/09/30����ģ���ļ�
			svnavMixedFile                  m_svnavMixedFile;
			GNSSYawAttitudeModel            m_gymMixed;						
			map<string, AntCorrectionBlk>   m_mapGnssPCVBlk;// ��ȡ����PCV��Ϣ

		public: //�˲��ֲ������������������ڼ��ݱ���,2014/09/29
			char                            cSatSystem;	
			double                          FREQUENCE_L1;
			double                          FREQUENCE_L2;
			bool                            getFREQUENCE();
		};
	}
}
;