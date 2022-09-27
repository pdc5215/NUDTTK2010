#pragma once
#include "Rinex2_1_ObsFile.hpp"
#include "Rinex2_1_MixedObsFile.hpp"
#include "Rinex2_1_LeoMixedEditedObsFile.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp" 
#include "Rinex2_1_NavFile.hpp"
#include "Rinex3_03_EditedObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "igs05atxFile.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp"
#include "GNSSObsPreproc.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{	

		// Ƶ��1��2�ڴ���ʱ����, Ƶ��3�ڽ�����Ƶ����ʱ��ʹ��
		struct PRE_Freq
		{
			double freq_L1;
			double freq_L2;
			double freq_L3;
		};
		// ϵͳ�ṹ
		struct HeoGNSSPRE_MixedSys
		{
			// ��Ҫ��ʼ������
			char                  cSys;             // ϵͳ��ʶ
			string                name_C1;               // α��
			string                name_C2;
			string                name_L1;              // ��λ
			string                name_L2;
		    string                name_S1;             // ����� 
			string                name_S2;
			string                nameFreq_L1;         // Ƶ������"G01"����Ҫ����PCV����
			string                nameFreq_L2;   
			PRE_Freq              freqSys;             // GLONASS��Ƶ����Ϣ�洢��freqSatList��, �������Զ���ֵ

			double                                  freq_L1;
			double                                  freq_L2;
			// ��ϵͳ�йص�Ԥ���岿��
			bool									bOn_GNSSSAT_AntPCOPCV;
			bool									bOn_GNSSSAT_Clk;
			bool									bOn_GNSSSAT_Relativity;

			//Ƶ������
			int					index_C1;
			int					index_C2;
			int					index_L1;
			int					index_L2;
			map<string, PRE_Freq> freqSatList; // ��Ҫ���GLONASS��string����Ϊ������

			HeoGNSSPRE_MixedSys()
			{
				name_C1  = "";
				name_C2  = "";
				name_L1  = "";
				name_L2  = "";
				name_S1  = "";
				name_S2  = "";
				nameFreq_L1 = "";
				nameFreq_L2 = "";
				freq_L1                = GPS_FREQUENCE_L1;
				freq_L2                = GPS_FREQUENCE_L2;
			}
		};

		struct HeoGNSSObsPreprocDefine
		{
			double       max_ionosphere;           // ������ӳ�������ֵ������α��Ұֵ�޳�
	        double       min_ionosphere;           // ������ӳ������Сֵ
			double       min_elevation;            // ��͹۲�����,��
			unsigned int min_arcpointcount;        // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       max_arclengh;             // �����������μ��������ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������(��)
			double       threshold_slipsize_mw;    // melbourne wuebbena �������������̽����ֵ(��)
			double       threshold_rms_mw;         // melbourne wuebbena ������ľ�������ֵ, �Կ�������Ϊ��λ
			double       threshold_slipsize_P1_L1; // P1 - L1 �������������̽����ֵ(��)
			double       threshold_rms_P1_L1;
			double       vondrak_PIF_eps;          // vondrak ��ϲ���
			double       vondrak_PIF_max;          // vondrak ��ϲ���
			double       vondrak_PIF_min;          // vondrak ��ϲ���
			unsigned int vondrak_PIF_width;        // vondrak ��ϲ���
			double       threshold_gap;            // �����ڵ��жϼ����ֵ, ���Ʋ�Ҫ���ֽϴ���ж�(��)
			double       max_pdop;                 // ���ξ���������ֵ, ������ֵ�Ĺ۲�㽫����������
			double       threshold_res_raim;       // RAIM ����в���ֵ
			int			  threshold_gpssatcount;
			double       threshold_editrms_code;   // α��в���ֵ
			double       threshold_editrms_phase;  // ��λ�в���ֵ
			double       threshold_ClockJumpSize;  // ������С��ֵ, ��λ: ��
			bool			bOn_ClockEliminate;       // SY1���ǵ����ݴ�����Ҫ�����Ӳ�
			bool         bOn_ClockJumpDiagnose;    // SY1��������ʶ�𿪹�
			bool         bOn_L2SNRLostDiagnose;    // ���L2ʧ������
			bool         bOn_SlipEditedMWDiagnose; // ��֤��λ��α���ƥ����, ��Ҫ�����sy1������, 2009/12/23 
			bool         bOn_IonosphereFree;       // �Ƿ�������������̽������
			bool         bOn_POutlierAccordingL;   // �����λΪҰֵ, ���ж�α��Ҳ��Ұֵ
			bool         bOn_RaimSPP;              // RAIM���-����
			bool         bOn_RaimArcChannelBias;   // RAIM���-����
			bool         bOn_Slip_L1_L2;           // ���� sy1 ���ֹ���߶Ƚϸߵ����ǿ��Կ���1���� L1-L2 ����, CHAMP �� GRACE ���ǹ���߶�̫��, ������
			bool         bOn_GNSSSAT_AntPCOPCV;    // ����������λ����
			bool         bOn_PhaseWindUp;          // ��λ����  
			int			typ_BDSobs_L1;            // ������һ��Ƶ��Ĺ۲��������� 
			int			typ_BDSobs_L2;            // �����ڶ���Ƶ��Ĺ۲���������

			//���㿪��
			bool    bOn_GNSSSAT_Clock;
			bool	   bOn_GNSSSAT_Relativity;

			 
			HeoGNSSObsPreprocDefine()
			{
				max_ionosphere						= 30.0 + 5;
		        min_ionosphere						= 0.0 - 5;
				min_elevation							= 5.0;
				min_arcpointcount					= 10;
				max_arclengh							= 2000.0;
				threshold_slipsize_mw				= 3.0; // �˴���Ҫ�����༭, �������ȡ��һЩ
				threshold_rms_mw					= 1.0;
				threshold_slipsize_P1_L1			= 10;  // ֻ��̽�������, ��λ m
				threshold_rms_P1_L1				=  5;
				vondrak_PIF_eps						= 1.0E-10;
				vondrak_PIF_max						= 1.5;
				vondrak_PIF_min						= 0.3;
				vondrak_PIF_width					= 30;
				threshold_gap							= 300;
				max_pdop								= 8.0;
				threshold_res_raim					= 3.5;
				threshold_gpssatcount				= 3;      
				threshold_editrms_code			= 2.50;
			    threshold_editrms_phase			= 0.10;
				bOn_ClockEliminate					= false;
				bOn_ClockJumpDiagnose		= false;
				bOn_SlipEditedMWDiagnose	= false;
				bOn_IonosphereFree				= true;
				bOn_POutlierAccordingL			= false;
				bOn_RaimSPP							= true;
				bOn_RaimArcChannelBias    = true;
				bOn_Slip_L1_L2					= false;
				bOn_GNSSSAT_AntPCOPCV = true;
				bOn_PhaseWindUp				= true;
				typ_BDSobs_L1						= TYPE_OBS_L1;
				typ_BDSobs_L2						= TYPE_OBS_L5;		

				bOn_GNSSSAT_Clock			= true ;
				bOn_GNSSSAT_Relativity		= true;
			} 
		};

		struct spp_results
		{
			DayTime t;
			POSCLK   posclk;
			int eyeableGPSCount;
			double pdop;
			double rms_res;
		};

		class HeoGNSSObsPreproc
		{
		public:
			HeoGNSSObsPreproc(void);
		public:
			~HeoGNSSObsPreproc(void);
		public:
			void    setPreprocPath(string strPreprocPath);
			void    setSP3File(SP3File sp3File); 
			void    setCLKFile(CLKFile clkFile); 
			void    setObsFile(Rinex2_1_ObsFile obsFile);
			bool    loadSP3File(string  strSp3FileName);
			bool    loadCLKFile(string  strCLKFileName);
			bool    loadCLKFile_rinex304(string  strCLKFileName);
			bool    loadObsFile(string  strObsFileName);
			bool    loadMixedObsFile(string  strObsFileName);
			void    setAntPhaseCenterOffset(POS3D posRTN);
			void    setAntPhaseCenterOffset(POS3D posBody, Matrix matAxisBody2RTN);
			BYTE    obsPreprocInfo2EditedMark1(int obsPreprocInfo);
			BYTE    obsPreprocInfo2EditedMark2(int obsPreprocInfo);
            // TQ�߹��������ݴ���
			void    setHeoOrbitList(vector<TimePosVel> heoOrbitList);
			bool    getHeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange = 8);

			// sk ���
			bool    mainFuncHeoGNSSObsEdit_new(Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile);	// 
			bool    mixedObsSPP(Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold = 1.0E-003);


			bool    pdopMixedObsSPP(int index_P1_GPS, int index_P2_GPS, int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, int& eyeableSatCount, double& pdop);
			bool    mainFuncTQObs2Edt(Rinex2_1_LeoEditedObsFile  &editedObsFile,int type_obs_L1 = TYPE_OBS_L1,int type_obs_L2 = TYPE_OBS_L2);
			bool    mainFuncHeoGNSSObsEdit(string  strMixedObsFileName,  Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile);	
			bool    TQSinglePointPositioning_PIF(char cSatSystem, int index_P1, int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableGPSCount, double& pdop, double& rms_res, double threshold = 1.0E-002);
            bool    TQMixedSinglePointPositioning_PIF(int index_P1_GPS, int index_P2_GPS, int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold = 1.0E-002);
		private:
			bool    getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist); 
			bool    getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist); // +
			bool    getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
			bool    datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		    bool    datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist);
			vector<Rinex2_1_LeoEditedObsEpoch>  m_editedObsEpochList;
			string  m_strPreprocPath;
		public:
			HeoGNSSObsPreprocDefine		m_PreprocessorDefine;
			CLKFile											m_clkFile; // �����Ӳ������ļ�
			SP3File											m_sp3File; // �������������ļ�
			Rinex2_1_ObsFile							    m_obsFile; // ��ϵͳԭʼ�۲�����		
			Rinex2_1_MixedObsFile                           m_mixedObsFile; // ���ϵͳ�۲�����
			POS3D											m_pcoAnt;  // ����ƫ����		
			POS3D											m_posStation;		 // ��վλ������ 
			svnavMixedFile							m_svnavMixedFile;    // +
			vector<TimePosVel>					m_heoOrbitList; // ����ѧ�ο����
			int												m_countSlip;
			int												m_countRestSlip; // ����Ϊ��ɾ����������
			Matrix											m_matAxisAnt2Body;  // ����ϵ���ǹ�ϵ��ת�������������߿��ܰ�װ�ڷ��춥����
			// 
			vector<HeoGNSSPRE_MixedSys> m_preMixedSysList;
			vector<spp_results>  SPP_resultsList;
		};
	}
}