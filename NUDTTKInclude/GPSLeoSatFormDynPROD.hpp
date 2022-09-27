#pragma once
#include "SatdynBasic.hpp"
#include "MathAlgorithm.hpp"
#include "structDef.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp"
#include "Rinex2_1_LeoEditedSdObsFile.hpp"
#include "Rinex2_1_LeoMixedEditedSdObsFile.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "AntPCVFile.hpp"
#include "GPSLeoSatDynPOD.hpp"
#include "lambda.hpp"
#include "igs05atxFile.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::SpaceborneGPSPreproc;
using namespace NUDTTK::SpaceborneGPSPod;
using namespace NUDTTK::Math;
using namespace NUDTTK::LAMBDA;
namespace NUDTTK
{
	namespace SpaceborneGPSProd
	{
		struct GPSLeoSatFormDynPRODPara
		{
			int                          max_OrbitIterativeNum;       // ����Ľ��������ֵ
			double                       max_arclengh;    
			unsigned int                 min_arcpointcount;           // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double                       min_elevation;
			int                          min_eyeableGPScount;         // ��С�������Ǹ���, ����С�� min_eyeableGPScount ����Ԫ�����������
			double                       apriorityRms_PIF;            // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_LIF;            // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double                       threhold_LAMBDA_ksb_MW;
			double                       threhold_LAMBDA_ksb_NA;
			bool                         bOn_GPSRelativity;           // �Ƿ���� GPS ��������۸���
			bool                         bOn_RecRelativity;           // ���ջ����������
			bool                         bOn_GPSAntPCO;               // �Ƿ���� GPS ��������ƫ������
			bool                         bOn_LEOAntPCO;               // �Ƿ���� LEO ��������ƫ������
			bool                         bOn_LEOAntPCV;               // �Ƿ���� LEO ����������λ��������, ֻ������Ե�PCV
			bool                         bOnEst_LEOAntRPCO_X;         // �Ƿ���� LEO ���� X �����������ƫ�ƹ���
			bool                         bOnEst_LEOAntRPCO_Y;         // �Ƿ���� LEO ���� Y �����������ƫ�ƹ���
			bool                         bOnEst_LEOAntRPCO_Z;         // �Ƿ���� LEO ���� Z �����������ƫ�ƹ���
			double                       apriorityRms_LEOAntRPCO;     // ������� PCO ����, ����α����Լ��
			bool                         bOn_WeightElevation;         // �Ƿ���и߶ȽǼ�Ȩ
			int                          ambiguityFixedType_MW;       // ����ģ���ȹ̶�����: 0-������Ԫ˫���ϵ; 1-�����ֱ�ӹ���˫��[�Ƽ�����] 
			double                       max_var_MW;                  // ����ģ���Ȼ��η�����ֵ
			double                       threhold_MW_Decimal;         // ����ģ���ȸ����С������
			double                       threhold_NA_Decimal;         // խ��ģ���ȸ����С������
			double                       threhold_MW_ReREFRatio;      // ����ο�ģ���ȸ�������
			bool                         bOn_DebugInfo_AmFixed;
			bool                         bOn_LEOAntMWV;

			GPSLeoSatFormDynPRODPara()
			{
				max_OrbitIterativeNum       = 5;
				max_arclengh                = 2000.0;
				min_arcpointcount           = 40;
				min_elevation               = 5.0;
				apriorityRms_PIF            = 0.5;
				apriorityRms_LIF            = 0.002;
				bOn_GPSRelativity           = true;
				bOn_RecRelativity           = true;
				bOn_GPSAntPCO               = true;
				bOn_LEOAntPCO               = true;
				bOn_LEOAntPCV               = true;
				bOnEst_LEOAntRPCO_X         = false;
				bOnEst_LEOAntRPCO_Y         = false;
				bOnEst_LEOAntRPCO_Z         = false;
				apriorityRms_LEOAntRPCO     = 1.0;
				bOn_WeightElevation         = false;
				min_eyeableGPScount         = 3;
                // ģ���ȹ̶���ز�������
				ambiguityFixedType_MW       = 1;    
				threhold_LAMBDA_ksb_MW      = 3.0;  
				threhold_LAMBDA_ksb_NA      = 5.0;
				max_var_MW                  = 0.5;  // �Ƽ�ֵ GRACE 0.5 ���ﲨ��, ����������α������ƫ��, ��Ҫ�ʵ��ſ�
				threhold_MW_Decimal         = 0.30; // �޳�����˫����ƫ�����������ĵ�, ����ģ���ȹ̶�������� 
				threhold_NA_Decimal         = 0.50; // խ��ģ���ȸ�����Ϊ׼ȷ, ���ʵ��ſ�������
				bOn_DebugInfo_AmFixed       = false;

				/*ambiguityFixedType_MW     = 0;
				threhold_LAMBDA_ksb         = 2.5;*/
				threhold_MW_Decimal         = 0.40; // ���ԭ��1m�������ʹ��, �ʵ��ſ���︡�����, ����20060107 04:49:20-05:23:00 4������ģ���ȸ����С��������0.3-0.4֮�䱻ɾ��
				threhold_MW_ReREFRatio      = 1.5;

				bOn_LEOAntMWV               = true;          
			}
		};
		struct MixedGNSSPRODDatum
		{
			char cSatSystem;                                              // ϵͳ���
			vector<Rinex2_1_LeoEditedSdObsEpoch>  editedObsEpochlist;     // ��ϵͳ������Ԫ��ʽ�۲�����, �ǻ�ϸ�ʽ
			vector<Rinex2_1_EditedSdObsSat>       editedObsSatlist;       // ��ϵͳ�������Ǹ�ʽ�۲�����, �ǻ�ϸ�ʽ
			map<int, PODEpoch>                    mapDynEpochList;   
			vector<AMBIGUITY_SECTION>             amSectionList;			
			int                                   ambiguityIndexBegin;		
			map<int, double>                      mapWindupPrev;          // ��¼Windup��������
			vector<int>                           mixedEpochIdList;       // ��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ���, �����໥����
			vector<int>                           epochIdList;            // ��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ���
			vector<O_CResEpoch>                   m_ocResP_IFEpochList;   // �޵����α��O-C�в�
			vector<O_CResEpoch>                   m_ocResL_IFEpochList;   // �޵������λO-C�в�
			double   FREQUENCE_L1;
			double   FREQUENCE_L2;
			double   WAVELENGTH_L1;
			double   WAVELENGTH_L2;
			double   WAVELENGTH_W;
			double   WAVELENGTH_N;
			double   coeff_mw;
			double   coefficient_IF;
			double   coefficient_L1;
			double   coefficient_L2;
			int      index_P1;
			int      index_P2;
			int      index_L1;
			int      index_L2;
			double   weightSystem;                                        // ϵͳ���Ȩϵ��֮��, Ĭ��Ϊ1.0
			double   sysBias;                                             // ϵͳ��ƫ���GPSΪ�ο�ϵͳ����ֵΪ0			
			
			MixedGNSSPRODDatum(double frequence1 = GPS_FREQUENCE_L1, double frequence2 = GPS_FREQUENCE_L2)
			{
				weightSystem   = 1.0;
				FREQUENCE_L1   = frequence1;
				FREQUENCE_L2   = frequence2;
				WAVELENGTH_L1  = SPEED_LIGHT / FREQUENCE_L1;
				WAVELENGTH_L2  = SPEED_LIGHT / FREQUENCE_L2;
				WAVELENGTH_W   = SPEED_LIGHT / (FREQUENCE_L1 - FREQUENCE_L2);
				WAVELENGTH_N   = SPEED_LIGHT / (FREQUENCE_L1 + FREQUENCE_L2);
				coeff_mw       = WAVELENGTH_L2 * pow(FREQUENCE_L2, 2) / (pow(FREQUENCE_L1, 2) - pow(FREQUENCE_L2, 2));
				coefficient_IF = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2));
				coefficient_L1 = 1 / (1 - pow(FREQUENCE_L2 / FREQUENCE_L1, 2)); 
				coefficient_L2 = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2)); 
				sysBias        = 0;				
			}
		};

		struct PROD_DD_AmbEq
		{
			int    id_DDREFAmbiguity_GL;
			int    id_Ambiguity_GL;
			double ambiguity_DD_MW;

			// ���ڼ��
			double ambiguity_DD_MW_Float;
		};

		class GPSLeoSatFormDynPROD
		{
		public:
			GPSLeoSatFormDynPROD(void);
		public:
			~GPSLeoSatFormDynPROD(void);
		public:
			bool loadSP3File(string  strSp3FileName);
			bool loadCLKFile(string  strCLKFileName);
			bool loadCLKFile_rinex304(string strCLKFileName);
			CLKFile getRelativeClkFile(); 
			bool mainFuncSdPreproc(Rinex2_1_LeoEditedSdObsFile& editedSdObsFile);
			bool mainFuncMixedSdPreproc(Rinex2_1_LeoMixedEditedSdObsFile& editedSdObsFile);
			void weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			bool lambdaSelected(Matrix matAFloat, Matrix matQahat, Matrix& matSqnorm, Matrix& matAFixed, Matrix matSelectedFlag);
			// ģ���ȸ����--����ѧ����
			bool dynamicPROD_amfloat_IF(string editedSdObsFilePath, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
			bool dynamicPROD_amfixed_mw_L1(string editedSdObsFilePath, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
			// ģ���ȹ̶���--����ѧ����
			bool dynamicPROD_amfixed_mw(string editedSdObsFilePath, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
            bool dynamicPROD_amfixed_prior(string editedSdObsFilePath, string editedSdClockFilePath, string priorOrbitFilePath_A, string priorOrbitFilePath_B, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
			bool dynamicMixedPROD_amfixed_mw(string editedMixedSdObsFilePath, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
		    // �о����Ժ���
			bool dynamicPROD_amfixed_mw_varest(string editedSdObsFilePath, SatdynBasicDatum &dynamicDatum_A, SatdynBasicDatum &dynamicDatum_B, GPST t0_forecast, GPST t1_forecast, vector<TimePosVel> &forecastOrbList_A, vector<TimePosVel> &forecastOrbList_B, double interval = 30.0,  bool bForecast = true, bool bResEdit = true, bool bClkSolve = false);
		    AntPCVFile                  m_mwvFile;   // mw�в�����
			vector<O_CResArc>           m_mwResArcList_amfloat;
			vector<O_CResArc>           m_mwResArcList_amfixed;
		private:
			bool loadEditedSdObsFile(string  strEditedSdObsFileName);
			bool loadMixedEditedSdObsFile(string  strMixedEditedSdObsFileName);
			CLKFile m_recRelativeClkFile;                    // ��������Ӳ������
		public:
			GPSLeoSatDynPOD             m_dynPOD;
			GPSLeoSatFormDynPRODPara    m_prodParaDefine;
			svnavFile                   m_svnavFile;         // GPS����ƫ��
			igs05atxFile			    m_AtxFile;		     // ���������ļ�(2013/04/18, �ϱ�)
			CLKFile                     m_clkFile;           // �����Ӳ������ļ�
			SP3File                     m_sp3File;           // �������������ļ�
			Rinex2_1_LeoEditedObsFile   m_editedObsFile_A;   // A����ԭʼ�۲�����
			POS3D                       m_pcoAnt_A;          // A��������ƫ����
			Matrix                      m_matAxisBody2RTN_A; // �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����, 2015/03/09
			TimeAttitudeFile            m_attFile_A;         // A������̬�ļ�
			AntPCVFile                  m_pcvFile_A;         // A����������λ����
			Rinex2_1_LeoEditedObsFile   m_editedObsFile_B;   // B����ԭʼ�۲�����
			POS3D                       m_pcoAnt_B;          // B��������ƫ����
			Matrix                      m_matAxisBody2RTN_B; // �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����, 2015/03/09
			TimeAttitudeFile            m_attFile_B;         // B������̬�ļ�
			AntPCVFile                  m_pcvFile_B;         // B����������λ����
			Rinex2_1_LeoEditedSdObsFile m_editedSdObsFile;
			vector<O_CResEpoch>         m_ocResP_IFEpochList;
			vector<O_CResEpoch>         m_ocResL_IFEpochList;
			// Mixed ��Ϲ۲�������ر���
			Rinex2_1_LeoMixedEditedObsFile   m_editedMixedObsFile_A;   // A���ǻ�ϸ�ʽԭʼ�۲�����
			Rinex2_1_LeoMixedEditedObsFile   m_editedMixedObsFile_B;   // B���ǻ�ϸ�ʽԭʼ�۲�����
			Rinex2_1_LeoMixedEditedSdObsFile m_editedMixedSdObsFile;   // A��B���ǻ�ϸ�ʽ����۲�����
			vector<MixedGNSSPRODDatum>       m_dataMixedGNSSlist;  
		public:
			POS3D                       m_pcoAntEst;         // �������ƫ�������ƽ��
		};
	}
}

