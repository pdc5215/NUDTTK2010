#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "dynPODStructDef.hpp"
#include "MathAlgorithm.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "igs05atxFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "AntPCVFile.hpp"
#include "Ionex1_0_File.hpp"
#include "Rinex2_1_LeoMixedEditedObsFile.hpp"
#include "GPSYawAttitudeModel1995.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::SpaceborneGPSPreproc;
using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		// 2014/10/28, �ȵ·嶨��, ���ڵ��������������ӹ��� 
		struct IonGridRatioAlpha
		{	
			double  x;           // �Ա���x: ʱ������߶�           
			double  alpha_priori;// ������������
			double  alpha;       // ������������
			int     count;       // ��Ч��Ԫ����
		};

		struct IonGridRatioAlphaEstPara
		{
			bool                          bOnEst_IONRatio_Alpha; // �Ƿ���Ƶ�����������, 2014/10/27, �ȵ·�
			vector<IonGridRatioAlpha>     ionRatioAlphaList;
			int                           mark_IonRatio_x;       // �Ա���ѡ��: 0-����߶�; 1-��Ԫʱ��
			double                        period_IonRatio_x;
			double                        weight_IonRatio;
			
			bool                          bOnEst_IONSLMHeight;      // �Ƿ���Ƶ�����������, 2016/7/1, �ȵ·�
			double                        ionSLMHeight_priori; 
			double                        ionSLMHeight;
			
			IonGridRatioAlphaEstPara()
			{
				bOnEst_IONRatio_Alpha = false;
				mark_IonRatio_x = 1;
				period_IonRatio_x = 3600 * 3;
				weight_IonRatio = 0.1;
				ionRatioAlphaList.clear();
				
				bOnEst_IONSLMHeight = false;
				ionSLMHeight_priori = 80000;
				ionSLMHeight = 80000;
			}

			void init(double x0, double x1, int n = 1, double addSingleLayerHeight = 80000);	
			int  getIndexIonRatioAlphaList(double h); // ȷ���߶�h�ڵ����������Ӳ����б��е�λ��
		};

		struct GPSLeoSatDynPODPara
		{
			int                          max_OrbitIterativeNum;       // ����Ľ��������ֵ
			TYPE_SOLARPRESSURE_MODEL     solarPressureType;           // ̫����ѹģ������
			TYPE_ATMOSPHEREDRAG_MODEL    atmosphereDragType;          // ��������ģ������
			double                       period_SolarPressure;  
			double                       period_AtmosphereDrag;
			double                       period_EmpiricalAcc;
			double                       period_RadialEmpForce;       // R����ֵ��������������
			double                       period_EarthIrradiance;      // �����շ����������
			double                       min_arcpoint_addpara;        // ������������С����, 2013/04/22
			double                       min_elevation;
			double                       apriorityRms_PIF;            // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double                       apriorityRms_LIF;            // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double                       max_arclengh;    
			unsigned int                 min_arcpointcount;           // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			int                          min_eyeableGPScount;         // ��С�������Ǹ���, ����С�� min_eyeableGPScount ����Ԫ�����������
		    double                       threshold_initDynDatumEst;   // ����ȷ�����
			int                          flag_UseSingleFrequency;     // �Ƿ�ʹ�õ�Ƶ�۲�����, 0: δʹ��; 1: 1/2(L1+C1); 2: ������������
			bool                         bOn_WeightElevation;         // �Ƿ���и߶ȽǼ�Ȩ
			bool                         bOn_GPSRelativity;           // �Ƿ���� GPS ��������۸���
			bool                         bOn_RecRelativity;           // ���ջ����������
			bool                         bOn_GPSAntPCO;               // �Ƿ���� GPS ��������ƫ������
			bool                         bOn_LEOAntPCV_IGS;           // �Ƿ���� LEO ����������λ��������, ���� IGS �����ļ�
			bool                         bOn_LEOAntPCO;               // �Ƿ���� LEO ������������ƫ������
			bool                         bOn_LEOAntPCV;               // �Ƿ���� LEO ����������λ��������, �����ڹ�У׼���
			bool                         bOn_LEOAntCRV;               // �Ƿ���� LEO ����α��в���������, �����ڹ�У׼���
			bool                         bOn_PhaseWindUp;
			bool                         bOnEst_LEOAntPCO_X;          // �Ƿ���� LEO ���� X ��������ƫ�ƹ���
			bool                         bOnEst_LEOAntPCO_Y;          // �Ƿ���� LEO ���� Y ��������ƫ�ƹ���
			bool                         bOnEst_LEOAntPCO_Z;          // �Ƿ���� LEO ���� Z ��������ƫ�ƹ���			
			bool                         bOnCst_Maneuver_R;		  	  // �Ƿ���ӻ����� R ����Լ��
			bool						 bOnCst_Empirical_R;		  // �Ƿ���Ӿ����� R ����Լ��
			bool						 bOnCst_Empirical_T;		  // �Ƿ���Ӿ����� T ����Լ��
			bool						 bOnCst_Empirical_N;		  // �Ƿ���Ӿ����� N ����Լ��
			int                          cst_empirical_type;          // ������Լ��������, 0: ���Լ��; 1: ������Լ��;  2: ������ֵԼ��; 3: ���Լ�� + ������ֵԼ��; 4: ����Լ�� + ������ֵԼ��
			double                       apriorityRms_LEOAntPCO;    // ����PCO����, ����α����Լ��
			double						 apriorityRms_Maneuver_R;	// ���������Լ�����̾��ȣ�2014/5/9���� ��
			double                       apriorityRms_EmpiricalRel_R; // ������R�������Լ�����̾��ȣ�2014/10/07���ȵ·�
			double                       apriorityRms_EmpiricalRel_T; // ������T�������Լ�����̾��ȣ�2015/04/03���� ��
			double                       apriorityRms_EmpiricalRel_N; // ������N�������Լ�����̾��ȣ�2015/04/03���� ��

			bool                         bOn_OutputCovMatrix;         // �������ϵ�������ļ�
			double                       robustfactor_OC_edited;
			bool                         bOnEst_SysBias;              // �Ƿ����ϵͳ��ƫ����ϵͳ������Ҫ

			IonGridRatioAlphaEstPara     ionRatioAlpha;               // 2014/10/28, �ȵ·嶨��, ���ڵ����������ӹ���  

			double                       threshold_rms_oc_code;
			double                       threshold_max_adjustpos;
			// 2021.03.28�����ٶȼ����ݳ߶Ȳ�����ʱ��Ư����Լ������
			bool                         bOnCst_NonconserForce_Scale;
			double                       apriorityScale_X;              // // 2019/06/20,Τ����������߶Ȳ���ֵ
			double                       apriorityScale_Y;
			double                       apriorityScale_Z;
			double                       apriorityRms_NonconserForce_X; // �߶�����R�������Լ�����̾��ȣ�2019/04/10��Τ����
			double                       apriorityRms_NonconserForce_Y; // �߶�����T�������Լ�����̾��ȣ�2019/04/10��Τ����
			double                       apriorityRms_NonconserForce_Z; // �߶�����N�������Լ�����̾��ȣ�2019/04/10��Τ����
			// ʱ��Ư�����Լ��
			bool                         bOnCst_NonconserForce_c1Drift;
			bool                         bOnCst_NonconserForce_c2Drift;
			double                       apriorityRms_NonconserForce_c1X;
			double                       apriorityRms_NonconserForce_c1Y;
			double                       apriorityRms_NonconserForce_c1Z;
			double                       apriorityRms_NonconserForce_c2X;
			double                       apriorityRms_NonconserForce_c2Y;
			double                       apriorityRms_NonconserForce_c2Z;
			// �߶Ȳ�����Լ��
			bool                         bOnCst_NonconserForce_BiasX;
			bool                         bOnCst_NonconserForce_BiasY;
			bool                         bOnCst_NonconserForce_BiasZ;
			double                       apriorityRms_NonconserForce_BiasX;
			double                       apriorityRms_NonconserForce_BiasY;
			double                       apriorityRms_NonconserForce_BiasZ;
            
			GPSLeoSatDynPODPara()
			{
				max_OrbitIterativeNum       = 8;
				solarPressureType           = TYPE_SOLARPRESSURE_1PARA;
				atmosphereDragType          = TYPE_ATMOSPHEREDRAG_J71_GEODYN;
				period_AtmosphereDrag       = 3600 *  3.0;
				period_SolarPressure        = 3600 * 24.0; 
				period_EmpiricalAcc         = 3600 *  1.5;
				period_EarthIrradiance      = 3600 * 24.0;
				period_RadialEmpForce       = 3600 * 24.0;
				min_arcpoint_addpara        = 30; // �ݶ�Ϊ 30 ����Ч��, ��� 10 ���Ӳ����൱�� 5 ����
				min_elevation               = 5.0;
				apriorityRms_PIF            = 0.50;//2
				apriorityRms_LIF            = 0.002;//500
				max_arclengh                = 2000.0;
				min_arcpointcount           = 30;
				threshold_initDynDatumEst   = 300.0;//����ȷ�����
				bOn_WeightElevation         = false;
				bOn_GPSRelativity           = true;
				bOn_RecRelativity           = true;
				bOn_GPSAntPCO               = true;
				bOn_PhaseWindUp             = true;
				bOn_LEOAntPCO               = true;
				bOn_LEOAntPCV               = true;
				bOn_LEOAntCRV               = true;
				bOn_LEOAntPCV_IGS           = false;
				bOnEst_LEOAntPCO_X          = false;
				bOnEst_LEOAntPCO_Y          = false;
				bOnEst_LEOAntPCO_Z          = false;
				bOnCst_Maneuver_R	        = false;
				bOnCst_Empirical_R		    = true;
				bOnCst_Empirical_T		    = false;
				bOnCst_Empirical_N		    = false;
				cst_empirical_type          = 0;
				apriorityRms_LEOAntPCO      = 1.0;
				apriorityRms_Maneuver_R	    = 1E-6;
				apriorityRms_EmpiricalRel_R = 1E-9; //�ȵ·�
				apriorityRms_EmpiricalRel_T = 1E-6;
				apriorityRms_EmpiricalRel_N = 1E-6;

				flag_UseSingleFrequency     = 0;
				bOn_OutputCovMatrix         = false;
				min_eyeableGPScount         = 3;
				robustfactor_OC_edited      = 3.0;
				bOnEst_SysBias              = true;

				threshold_rms_oc_code       = 100.0;

				threshold_max_adjustpos     = 5.0E-3;
				if(flag_UseSingleFrequency != 0)
					threshold_max_adjustpos = 5.0E-2;
				// 2021.03.28�����ٶȼ����ݳ߶Ȳ�����ʱ��Ư����Լ������
				bOnCst_NonconserForce_Scale   = true;  // 2019/05/21,Τ�������߶Ȳ���Լ������
				apriorityScale_X              = 1.0;    // 2019/06/20,Τ����������߶Ȳ���ֵ
				apriorityScale_Y              = 1.0;
				apriorityScale_Z              = 1.0;
				apriorityRms_NonconserForce_X = 0.1;    // 2019/04/10,Τ�������߶Ȳ���Լ�����ο�GRACE TN-02
				apriorityRms_NonconserForce_Y = 0.02;
				apriorityRms_NonconserForce_Z = 0.02;
				// ���Ӷ�ʱ��Ư�����Լ������ֹʱ��Ư���������ɼ��ٶȼ����ݳ������Ե�Ư��
				bOnCst_NonconserForce_c1Drift   = true;  // 2019/05/21,Τ������ʱ��Ư����Լ������
				bOnCst_NonconserForce_c2Drift   = true;  // 2019/05/21,Τ������ʱ��Ư����Լ������
				apriorityRms_NonconserForce_c1X = 5E-10;  // ʱ��Ư����Լ��
				apriorityRms_NonconserForce_c1Y = 5E-9;
				apriorityRms_NonconserForce_c1Z = 5E-12;
				apriorityRms_NonconserForce_c2X = 5E-13;
				apriorityRms_NonconserForce_c2Y = 5E-12;
				apriorityRms_NonconserForce_c2Z = 5E-13;
				// ���Ӷ�ƫ�������Լ��
				bOnCst_NonconserForce_BiasX       = false;
				bOnCst_NonconserForce_BiasY       = false;
				bOnCst_NonconserForce_BiasZ       = false;
				apriorityRms_NonconserForce_BiasX = 1E-7;
				apriorityRms_NonconserForce_BiasY = 1E-9;
				apriorityRms_NonconserForce_BiasZ = 1E-9;
			}
		};

		struct MixedGNSSPODDatum
		{
			char cSatSystem;                                            // ϵͳ���
			vector<Rinex2_1_LeoEditedObsEpoch>  editedObsEpochlist;     // ��ϵͳ��Ԫ��ʽ�۲�����, �ǻ�ϸ�ʽ
			vector<Rinex2_1_EditedObsSat>       editedObsSatlist;       // ��ϵͳ���Ǹ�ʽ�۲�����, �ǻ�ϸ�ʽ
			map<int, PODEpoch>                  mapDynEpochList;    
			vector<ObsEqEpoch>                  P_IFEpochList;       
			vector<ObsEqArc>                    L_IFArcList;
			int                                 ambiguityIndexBegin;
			vector<ObsEqEpoch>                  L_IFEpochList;
			map<int, double>                    mapWindupPrev;          // ��¼Windup��������
			vector<int>                         mixedEpochIdList;       // ��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ���, �����໥����
			vector<int>                         epochIdList;            // ��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ���
			vector<O_CResEpoch>                 ocResP_IFEpochList;     // �޵����α��O-C�в�
			vector<O_CResArc>                   ocResL_IFArcList;       // �޵������λO-C�в�
			double   FREQUENCE_L1;
			double   FREQUENCE_L2;
			double   WAVELENGTH_L1;
			double   WAVELENGTH_L2;
			double   coefficient_L1;
			double   coefficient_L2;
			int      index_P1;
			int      index_P2;
			int      index_L1;
			int      index_L2;
			double   weightSystem;                                        // ϵͳ���Ȩϵ��֮��, Ĭ��Ϊ1.0
			double   sysBias;                                             // ϵͳ��ƫ���GPSΪ�ο�ϵͳ����ֵΪ0			
			
			MixedGNSSPODDatum(double frequence1 = GPS_FREQUENCE_L1, double frequence2 = GPS_FREQUENCE_L2)
			{
				weightSystem   = 1.0;
				FREQUENCE_L1   = frequence1;
				FREQUENCE_L2   = frequence2;
				WAVELENGTH_L1  = SPEED_LIGHT / FREQUENCE_L1;
				WAVELENGTH_L2  = SPEED_LIGHT / FREQUENCE_L2;
				coefficient_L1 = 1 / (1 - pow(FREQUENCE_L2 / FREQUENCE_L1, 2)); 
				coefficient_L2 = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2)); 
				sysBias        = 0;				
			}
		};

		class GPSLeoSatDynPOD : public SatdynBasic
		{
		public:
			GPSLeoSatDynPOD(void);
		public:
			~GPSLeoSatDynPOD(void);
		public:
			void    setSP3File(SP3File sp3File); 
			void    setCLKFile(CLKFile clkFile); 
			CLKFile getRecClkFile(); 
			void    setStepAdamsCowell(double step);
			double  getStepAdamsCowell();
			bool    loadSP3File(string  strSp3FileName);
			bool    loadCLKFile(string  strCLKFileName); 
			bool    loadCLKFile_rinex304(string  strCLKFileName);
            bool    adamsCowell_Interp_Leo(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h = 10.0, int q = 11);
            bool    initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3);
			void    weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			bool    dynamicPOD_pos(vector<TimePosVel> obsOrbitList, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0, bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
			bool    dynamicPOD_phase(string editedObsFilePath, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0,  bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true, bool bOn_NotUsedPhase = false);
			bool    dynamicPOD_L1(string editedObsFilePath, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0,  bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true, bool bOn_NotUsedPhase = false);
			void    orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0);
			void    orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAcc> &forecastOrbList, double interval = 30.0);
			void    orbitExtrapolation_jerk(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAccJerk> &forecastOrbList, double interval = 30.0, bool bECEF = true);
		
			// Mixed ��Ϲ۲�������ش�����
			bool    dynamicMixedPOD_phase(string editedMixedObsFilePath, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0,  bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
		private:
			double  m_stepAdamsCowell; // 20150308, �ȵ·�, ������ֲ���ͳһ�޸�
			bool    loadEditedObsFile(string  strEditedObsFileName);
			CLKFile m_recClkFile; // �����Ӳ������

			// Mixed ��Ϲ۲�������ش�����
			bool loadMixedEditedObsFile(string  strMixedEditedObsFileName);
		public:
			GPSLeoSatDynPODPara       m_podParaDefine;
			CLKFile                   m_clkFile;			     // �����Ӳ������ļ�
			SP3File                   m_sp3File;			     // �������������ļ�
			Rinex2_1_LeoEditedObsFile m_editedObsFile;		     // ԭʼ�۲�����
			POS3D                     m_pcoAnt;				     // ����ƫ����
			svnavFile                 m_svnavFile;			     // GPS����ƫ��
			igs05atxFile			  m_AtxFile;			     // ���������ļ�(2013/04/18, �ϱ�)
			AntPCVFile                m_pcvFile;			     // ������λ����, �ڹ���ƽ��
			AntPCVFile                m_crvFile_IF;			     // α��в�仯, �ڹ���ƽ��
			Ionex1_0_File             m_ionFile;                 // IGS����������Ʒ
			//TimeAttitudeFile        m_attFile;			     // ��̬�ļ�
			//Matrix                  m_matAxisBody2RTN;         // �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����, 2015/03/09
			vector<O_CResEpoch>       m_ocResP_IFEpochList;	     // �޵����α��O-C�в�
			vector<O_CResArc>         m_ocResL_IFArcList;	     // �޵������λO-C�в�
			svnavMixedFile                  m_svnavMixedFile;
			GNSSYawAttitudeModel            m_gymMixed;		
			// Mixed ��Ϲ۲�������ر���
			Rinex2_1_LeoMixedEditedObsFile m_editedMixedObsFile; // ��ϸ�ʽԭʼ�۲�����
			vector<MixedGNSSPODDatum>      m_dataMixedGNSSlist;  
			Matrix                         m_matAxisAnt2Body;    // ����ϵ���ǹ�ϵ��ת�ƾ����������߿��ܰ�װ�ڷ��춥��������
		public:
			POS3D                     m_pcoAntEst;			     // ������: ����ƫ�������ƽ��
		};
	}
}



