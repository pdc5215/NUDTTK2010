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
#include"TROZPDFile.hpp"
#include"StaOceanLoadingDisplacementFile.hpp"
#include"OceanTidesLoading.hpp"
#include"Troposphere_model.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::Math;
using namespace NUDTTK::LAMBDA;
namespace NUDTTK
{
	namespace BDPod
	{
		struct BDSatDynPODPara
		{
			int                          max_OrbitIterativeNum;     // ����Ľ��������ֵ		
			unsigned int                 min_arcpointcount;         // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			int                          min_eyeableGPSCount;         // ����Ԫ�������Ǹ���
			TYPE_SOLARPRESSURE_MODEL     solarPressure_Model;       // ̫����ѹģ��
			double                       period_SolarPressure;      // ̫����ѹ����
			double                       max_FitRms_Total;
			double                       min_elevation;
			double                       apriorityRms_PIF;          // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_LIF;          // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_STA;          // �����վλ�����꾫��, ���ڲ�վ����Լ�����̼�Ȩ����
			double                       apriorityRms_TZD_abs;      // �����վ������춥�ӳپ���, ���ڵ�����춥�ӳپ���Լ�����̼�Ȩ����
			double                       apriorityRms_TZD_rel;      // �������ڵ�����춥�ӳٲ��������Լ�����̼�Ȩ����
			double                       apriorityWet_TZD;          // ��վ������ʪ������������ֵ
			double                       apriorityWet_TZD_period;   // ��վ������ʪ������������
			double                       min_Wet_TZD_ncount;        // ���������������������ݸ���
			double                       max_arclengh;    
			double                       threhold_LAMBDA_ksb;
			bool                         bOn_AmbiguityFix;          // �Ƿ�̶�խ������ģ����
			bool                         bOn_StaSolidTideCor;       // �Ƿ���в�վ����Ĺ��峱����
			bool                         bOn_StaOceanTides;         // �Ƿ���в�վ����ĺ�������
			bool                         bOn_BDTroPriModCor;        // �Ƿ���ж���������ģ������
			bool                         bOn_BDSRelativity;         // �Ƿ���� BD ��������۸���,���ƫ�ĸ���
			bool                         bOn_BDSGraRelativity;      // �Ƿ�����������������۸���
			bool                         bOn_RecAntARP;             // �Ƿ���н��ջ����� ARP ����
			bool                         bOn_BDSAntPCO;             // �Ƿ���� BD ��������ƫ������
			bool                         bOn_RecAntPCOPCV;          // �Ƿ���н��ջ����� PCO/PCV ����
			bool                         bOn_PhaseWindUp;
			bool                         bOnEst_StaPos;             // �Ƿ���в�վλ�øĽ�
			bool                         bOnEst_StaTropZenithDelay; // �Ƿ���в�վ�������춥�ӳٹ���
			bool                         bOnEst_ERP;                // �Ƿ���е�����ת��������
			bool                         bOn_WeightElevation;       // �Ƿ�Ը߶Ƚǽ��м�Ȩ
			bool                         bOn_DDDEdit;               // �Ƿ�����������ݱ༭
			double                       threshold_slip_DDDEdit;    // �������ݱ༭����̽����ֵ
			double                       threshold_outlier_DDDEdit; // �������ݱ༭Ұֵ̽����ֵ
			double                       maxdistance_Ambfixed_short;// �̻��߳�����ֵ, ���ڳ�ʼģ���Ȳ��̶�
			double                       maxdistance_Ambfixed_long; // �����߳�����ֵ, ���ڸó��ȵĻ���ģ���Ȳ��̶�
			double                       robustfactor_OC_edited;
			double                       bOnConstraints_GPSEphemeris; 
			double                       apriorityRms_GPSEphemeris; // GPS���������ľ���
			int                          sampleSpan;                // �������ݵĲ������(s)
			int                          zpdProductInterval;        // ����Ķ������Ʒ�������(s)
			int                          OrbitIterativeNum;          //�ڲ��̶�ģ���ȵ�����£����Ƶ�������
		

			BDSatDynPODPara()
			{
				max_OrbitIterativeNum     	= 8;				
				min_arcpointcount         	= 10;
				min_eyeableGPSCount       	= 3;
				solarPressure_Model       	= TYPE_SOLARPRESSURE_5PARA;
				period_SolarPressure      	= 3600 * 24.0;
				min_elevation             	= 5.0;
				apriorityRms_PIF          	= 0.5;
				apriorityRms_LIF          	= 0.002;
				apriorityRms_STA          	= 10.0;
				apriorityRms_TZD_abs      	= 0.5;
				apriorityRms_TZD_rel      	= 0.02;
				apriorityWet_TZD          	= 0;                      // ��վ������ʪ������������ֵ
				apriorityWet_TZD_period   	= 3600 * 2;               // ����
				min_Wet_TZD_ncount        	= 20;                     // �൱��40���ӵ����ݣ����ڴ�ֵ��ϲ���������
				max_arclengh              	= 3600;
				threhold_LAMBDA_ksb       	= 2.5;
				max_FitRms_Total          	= 20;
				bOn_AmbiguityFix          	= false;                   // true�̶�����ģ����;false,���̶�����ģ����
				bOn_StaSolidTideCor       	= true; 
				bOn_StaOceanTides         	= false;
				bOn_BDTroPriModCor        	= false;
				bOn_BDSRelativity         	= true;
				bOn_BDSGraRelativity     	= false;//20140929
				bOn_RecAntARP             	= true;
				bOn_BDSAntPCO             	= false;
				bOn_RecAntPCOPCV          	= false;				
				bOn_PhaseWindUp           	= true;
				bOnEst_StaPos             	= true;
				bOnEst_StaTropZenithDelay 	= true;
				bOnEst_ERP                	= false;
				bOn_WeightElevation       	= false;
				bOn_DDDEdit               	= true;
				threshold_slip_DDDEdit    	= 0.15;
				threshold_outlier_DDDEdit 	= 0.15;
				maxdistance_Ambfixed_short	= 2500000.0;
				maxdistance_Ambfixed_long 	= 5000000.0;
				robustfactor_OC_edited    	= 2.5;
				bOnConstraints_GPSEphemeris = true;
				apriorityRms_GPSEphemeris   = 1.0;
				sampleSpan                	= 60;
				zpdProductInterval        	= 3600 * 2;
				OrbitIterativeNum         	= 2;
			}
		};

		typedef struct {int valid; int id_arc; double obscorrected_value;} cvElement; // correctedvalueElement
		typedef struct {map<int, cvElement> mapDatum;} cvEpoch;   // correctedvalueEpoch

		class BDSatDynPOD : public SatdynBasic
		{
		public:
			BDSatDynPOD(void);
		public:
			~BDSatDynPOD(void);
		public:			
	        bool sp3Fit(string strSp3FilePath, Sp3FitParameter& paraSp3Fit, bool bOnEst_EOP = true);
			//static double graRelativityCorrect(POS3D satPos, POS3D staPos, double gamma = 1);			
			static bool obsSingleDifferencePreproc(Rinex2_1_EditedObsFile& editedObsFile_A, Rinex2_1_EditedObsFile& editedObsFile_B, Rinex2_1_EditedSdObsFile& editedSdObsFile);

			// �������ݱ༭
			bool obsTriDiFFEdited_LIF(int index_L1, int index_L2, int id_sat, Rinex2_1_EditedSdObsEpoch epoch_j_1, Rinex2_1_EditedSdObsEpoch epoch_j, map<int, cvElement> &mapCvDatum_j_1, map<int, cvElement> &mapCvDatum_j, double &max_res_ddd, bool &slipFlag, double threshold_slip = 0.15);
			bool mainTriDiFFEdited(SP3File &sp3File, Rinex2_1_EditedSdObsFile &editedSdObsFile, POS3D posAnt_A, POS3D posAnt_B, string outputFileName = "");

			void weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);		
			bool adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h = 75.0, int q = 11);
			bool mainNetPod_dd(string inputSp3FilePath, string outputSp3FilePath, SatOrbEstParameter& paraSatOrbEst, GPST t0, GPST t1, double h_sp3 = 300.0,bool bResEdit = true);
			
			
			// ���Դ���


			//bool initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3);
			//bool dynamicPOD_pos(vector<TimePosVel>  orbitlist, SatdynBasicDatum &dynamicDatum, GPST t_forecastBegin, GPST t_forecastEnd,  vector<TimePosVel> &orbitlist_forecast, double interval = 30.0, bool bforecast = true);
		public:
			BDSatDynPODPara                 m_podParaDefine;
			map<int, AntCorrectionBlk>      m_mapBDSAntCorrectionBlk;
			StaDatumMap                     m_mapStaDatum;
			vector<StaBaselineDatum>        m_staBaseLineList;
			igs05atxFile			        m_AtxFile;			  // ���������ļ�(2013/09/16, ������)
			StaOceanLoadingDisplacementFile m_staOldFile;         // �����ļ�
		};
	}
}
;