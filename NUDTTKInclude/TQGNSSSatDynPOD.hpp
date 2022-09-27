#pragma once
#include "structDef.hpp"
#include "SatdynBasic.hpp"
#include "dynPODStructDef.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "igs05atxFile.hpp"
#include "TimeAttitudeFile.hpp"
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
	namespace TQGNSSPod
	{
		struct TQGNSSSatDynPODPara
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
			bool                         bOn_OutputCovMatrix;
			double                       robustfactor_OC_edited;
			double                       threshold_rms_oc_code;
			double                       threshold_max_adjustpos;   
			int                          flag_UseSingleFrequency;     // �Ƿ�ʹ�õ�Ƶ�۲�����, 0: δʹ��; 1: C1

			TQGNSSSatDynPODPara()
			{
				max_OrbitIterativeNum       = 8;
				solarPressureType           = TYPE_SOLARPRESSURE_1PARA;
				atmosphereDragType          = TYPE_ATMOSPHEREDRAG_J71_GEODYN;
				period_AtmosphereDrag       = 3600 *  3.0;
				period_SolarPressure        = 3600 * 24.0; 
				period_EmpiricalAcc         = 3600 *  1.5;
				period_EarthIrradiance      = 3600 * 24.0;
				period_RadialEmpForce       = 3600 * 24.0;
				min_arcpoint_addpara        = 30;         // �ݶ�Ϊ 30 ����Ч��, ��� 10 ���Ӳ����൱�� 5 ����
				min_elevation               = 5.0;
				apriorityRms_PIF            = 0.50;
				apriorityRms_LIF            = 0.002;
				max_arclengh                = 2000.0;
				min_arcpointcount           = 30;
				threshold_initDynDatumEst   = 300.0;
				bOn_WeightElevation         = false;
				bOn_GPSRelativity           = false;
				bOn_RecRelativity           = false;
				bOn_GPSAntPCO               = false;
				bOn_PhaseWindUp             = false;
				bOn_LEOAntPCO               = false;
				bOn_LEOAntPCV               = false;
				bOn_LEOAntCRV               = false;
				bOn_LEOAntPCV_IGS           = false;
				bOnEst_LEOAntPCO_X          = false;
				bOnEst_LEOAntPCO_Y          = false;
				bOnEst_LEOAntPCO_Z          = false;
				bOnCst_Maneuver_R	        = false;
				bOnCst_Empirical_R		    = false;
				bOnCst_Empirical_T		    = false;
				bOnCst_Empirical_N		    = false;
				cst_empirical_type          = 0;
				apriorityRms_LEOAntPCO      = 1.0;
				apriorityRms_Maneuver_R	    = 1E-6;
				apriorityRms_EmpiricalRel_R = 1E-9; //�ȵ·�
				apriorityRms_EmpiricalRel_T = 1E-6;
				apriorityRms_EmpiricalRel_N = 1E-6;
				bOn_OutputCovMatrix         = false;
				min_eyeableGPScount         = 3;
				robustfactor_OC_edited      = 3.0;
				threshold_rms_oc_code       = 100.0;
				threshold_max_adjustpos     = 5.0E-1;
				flag_UseSingleFrequency     = 0;
			}
		};

		struct MixedGNSSTQPODDatum
		{
			char cSatSystem;                                            // ϵͳ���
			vector<Rinex2_1_LeoEditedObsEpoch>  editedObsEpochlist;     // ��ϵͳ��Ԫ��ʽ�۲�����, �ǻ�ϸ�ʽ
			vector<Rinex2_1_EditedObsSat>       editedObsSatlist;       // ��ϵͳ���Ǹ�ʽ�۲�����, �ǻ�ϸ�ʽ
			map<int, PODEpoch>                  mapDynEpochList;    
			vector<ObsEqEpoch>                  P_IFEpochList;       
			int                                 ambiguityIndexBegin;
			vector<int>                         mixedEpochIdList;       // ��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ���, �����໥����
			vector<int>                         epochIdList;            // ��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ���
			vector<O_CResEpoch>                 ocResP_IFEpochList;     // �޵����α��O-C�в�
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

			vector<double> leoClockList;                                     // ���ջ��Ӳϵͳ���
			vector<int>    validSysIndexList;                                // �Ӳ���Ч���λ��
			vector<int>    EyeableSysCountList;                              // ������������
            vector<int>    validSysObsTimeList;  // �Ӳ���Ч���λ�ö�Ӧ��ʱ��λ��
			int            count_clk;            // ��Ч�Ӳ����
			vector<double> n_cc;
			vector<double> n_cc_inv;
			Matrix n_cx;
			Matrix n_cp;
			Matrix nc;
			CLKFile m_recClkFile;         // �����Ӳ������

			vector<TimePosVel> interpOrbitlist; // ��ֵ����
			vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
			vector<TDT> interpTimelist;

			MixedGNSSTQPODDatum(double frequence1 = GPS_FREQUENCE_L1, double frequence2 = GPS_FREQUENCE_L2)
			{
				weightSystem   = 1.0;
				FREQUENCE_L1   = frequence1;
				FREQUENCE_L2   = frequence2;
				WAVELENGTH_L1  = SPEED_LIGHT / FREQUENCE_L1;
				WAVELENGTH_L2  = SPEED_LIGHT / FREQUENCE_L2;
				coefficient_L1 = 1 / (1 - pow(FREQUENCE_L2 / FREQUENCE_L1, 2)); 
				coefficient_L2 = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2));
				count_clk = 0;
				n_cc.clear();
                leoClockList.clear(); 
                validSysObsTimeList.clear();
                validSysIndexList.clear();	
                EyeableSysCountList.clear();		
			}
		};

		class TQGNSSSatDynPOD : public SatdynBasic
		{
		public:
			TQGNSSSatDynPOD(void);
		public:
			~TQGNSSSatDynPOD(void);
		public:
			void    setSP3File(SP3File sp3File); 
			void    setCLKFile(CLKFile clkFile); 
			CLKFile getRecClkFile(); 
			void    setStepAdamsCowell(double step);
			double  getStepAdamsCowell();
			bool    loadSP3File(string  strSp3FileName);
			bool    loadCLKFile(string  strCLKFileName);
            bool    adamsCowell_Interp_Leo(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h = 10.0, int q = 11);
            bool    initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength = 3600 * 3);
			void    weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			void    orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0);
	
			// Mixed ��Ϲ۲�������ش�����
			bool    dynamicTQGnssMixedPOD(string editedMixedObsFilePath, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval = 30.0,  bool bInitDynDatumEst = false, bool bForecast = true, bool bResEdit = true);
		private:
			double  m_stepAdamsCowell; // 20150308, �ȵ·�, ������ֲ���ͳһ�޸�
			bool    loadEditedObsFile(string  strEditedObsFileName);
			// Mixed ��Ϲ۲�������ش�����
			bool loadMixedEditedObsFile(string  strMixedEditedObsFileName);
			bool loadMixedEditedObsFile_5Obs(string  strMixedEditedObsFileName);
		public:
			TQGNSSSatDynPODPara       m_podParaDefine;
			CLKFile                   m_clkFile;			     // �����Ӳ������ļ�
			SP3File                   m_sp3File;			     // �������������ļ�
			Rinex2_1_LeoEditedObsFile m_editedObsFile;		     // ԭʼ�۲�����
			POS3D                     m_pcoAnt;				     // ����ƫ����
			svnavFile                 m_svnavFile;			     // GPS����ƫ��
			igs05atxFile			  m_AtxFile;			     // ���������ļ�(2013/04/18, �ϱ�)
			vector<O_CResEpoch>       m_ocResP_IFEpochList;	     // �޵����α��O-C�в�
			svnavMixedFile            m_svnavMixedFile;
			GNSSYawAttitudeModel      m_gymMixed;		
			// Mixed ��Ϲ۲�������ر���
			Rinex2_1_LeoMixedEditedObsFile m_editedMixedObsFile; // ��ϸ�ʽԭʼ�۲�����
			vector<MixedGNSSTQPODDatum>    m_dataMixedGNSSlist;  
			Matrix                         m_matAxisAnt2Body;    // ����ϵ���ǹ�ϵ��ת�ƾ����������߿��ܰ�װ�ڷ��춥��������
			//CLKFile m_recClkFile;         // �����Ӳ������
		public:
			POS3D                     m_pcoAntEst;			     // ������: ����ƫ�������ƽ��
		};
	}
}



