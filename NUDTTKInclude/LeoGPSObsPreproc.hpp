#pragma once
#include "Rinex2_1_ObsFile.hpp"
#include "Rinex2_1_MixedObsFile.hpp"
#include "Rinex2_1_LeoMixedEditedObsFile.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp" 
#include "Rinex2_1_NavFile.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "igs05atxFile.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		enum TYPE_LEOGPSOBSPREPROC_INFO
		{   
			LEOGPSOBSPREPROC_UNKNOWN              = 00,
			LEOGPSOBSPREPROC_NORMAL               = 10,
			// Ұֵ����
			LEOGPSOBSPREPROC_OUTLIER_BLANKZERO    = 20, // �۲�����ȱʧ
			LEOGPSOBSPREPROC_OUTLIER_COUNT        = 21, // �۲����̫�ٱ�ǲ�����
			LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION = 22, // �����+�߶Ƚ�ֹ��
			LEOGPSOBSPREPROC_OUTLIER_IONOMAXMIN   = 23, // �����̽��α��Ұֵ
			LEOGPSOBSPREPROC_OUTLIER_MW           = 24, // MW���̽��Ұֵ
			LEOGPSOBSPREPROC_OUTLIER_VONDRAK      = 25, // Vondrak�˲�������ΪҰֵ
			LEOGPSOBSPREPROC_OUTLIER_RAIM         = 26, // RAIM����
			LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS    = 27, // �������ݲ���
			LEOGPSOBSPREPROC_OUTLIER_MWRMS        = 28, // MW�����������׼���
			LEOGPSOBSPREPROC_OUTLIER_L1_L2        = 29, // L1 - L2̽��Ұֵ  ��������ĵ������Ԫ����ֵ 0.10
			// ��������
			LEOGPSOBSPREPROC_NEWARCBEGIN          = 30,
			LEOGPSOBSPREPROC_SLIP_MW              = 31,//MW���̽������
			LEOGPSOBSPREPROC_SLIP_IFAMB           = 32,//�޵����ģ����
			LEOGPSOBSPREPROC_SLIP_L1_L2           = 33 //�޵�������̽������
		};

		enum TYPE_LEOGPSOBSEDIT_INFO
		{   
			LEOGPSOBSEDIT_UNKNOWN              = 00,
			LEOGPSOBSEDIT_NORMAL               = 10,
			// Ұֵ����
			LEOGPSOBSEDIT_OUTLIER_BLANKZERO    = 20,
			LEOGPSOBSEDIT_OUTLIER_COUNT        = 21, 
			LEOGPSOBSEDIT_OUTLIER_SNRELEVATION = 22, // �����+�߶Ƚ�ֹ��
			LEOGPSOBSEDIT_OUTLIER_EPHEMERIS    = 23, // ����GPS������LEO��������
			LEOGPSOBSEDIT_OUTLIER_PIF          = 24, // α��༭
			LEOGPSOBSEDIT_OUTLIER_LIF          = 25, // ��λ�༭
			LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT  = 26, // �༭������, GPS���ǵĸ�������
			// ���sy1��α����λ��ƥ������, ���
			LEOGPSOBSEDIT_OUTLIER_MW           = 27, 
			LEOGPSOBSEDIT_OUTLIER_IFAMB        = 28, 
			LEOGPSOBSEDIT_OUTLIER_MWRMS        = 29,

			// ��������
			LEOGPSOBSEDIT_NEWARCBEGIN          = 30,
			LEOGPSOBSEDIT_SLIP_LIF             = 31,
			// ���sy1��α����λ��ƥ������, ���
			LEOGPSOBSEDIT_SLIP_MW              = 32,
			LEOGPSOBSEDIT_SLIP_IFAMB           = 33
		};
		struct LeoGPSObsPreprocDefine
		{
			double       max_ionosphere;           // ������ӳ�������ֵ������α��Ұֵ�޳�
	        double       min_ionosphere;           // ������ӳ������Сֵ
			double       min_elevation;            // ��͹۲�����,��
			unsigned int min_arcpointcount;        // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       max_arclengh;             // �����������μ��������ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������(��)
			double       threshold_slipsize_mw;    // melbourne wuebbena ����� ��������̽����ֵ(��) Ĭ��3.0 //Tslip ����ʦ����
			double       threshold_rms_mw;         // melbourne wuebbena ����� �ľ�������ֵ Ĭ��1.0
			double       threshold_slipsize_P1_L1; // P1 - L1 ����� ��������̽����ֵ(��)10.0
			double       threshold_rms_P1_L1;      // P1 - L1 ����� �ľ�������ֵ(��)5.0
			double       vondrak_PIF_eps;          // vondrak ��ϲ���
			double       vondrak_PIF_max;          // vondrak ��ϲ���
			double       vondrak_PIF_min;          // vondrak ��ϲ���
			unsigned int vondrak_PIF_width;        // vondrak ��ϲ���
			double       threshold_gap;            // �����ڵ��жϼ����ֵ, ���Ʋ�Ҫ���ֽϴ���ж�(��) Ĭ��300s
			double       max_pdop;                 // ���ξ���������ֵ, ������ֵ�Ĺ۲�㽫����������
			double       threshold_res_raim;       // RAIM ����в���ֵ
			int          threshold_gpssatcount;    // gps����������ֵ
			double       threshold_editrms_code;   // α��в���ֵ
			double       threshold_editrms_phase;  // ��λ�в���ֵ
			double       threshold_ClockJumpSize;  // ������С��ֵ, ��λ: ��
			bool         bOn_ClockEliminate;       // SY1���ǵ����ݴ�����Ҫ�����Ӳ�
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
			int          typ_BDSobs_L1;            // ������һ��Ƶ��Ĺ۲��������� 
			int          typ_BDSobs_L2;            // �����ڶ���Ƶ��Ĺ۲���������
			 
			LeoGPSObsPreprocDefine()
			{
				max_ionosphere           = 30.0 + 5;
		        min_ionosphere           = 0.0 - 5;
				min_elevation            = 5.0;
				min_arcpointcount        = 10;
				max_arclengh             = 2000.0;
				threshold_slipsize_mw    = 3.0; //  ��λ �ܣ����//���� threshold_slipsize_wm �� threshold_outlier ���Ͻ���п���
				threshold_rms_mw         = 1.0; // ����ʦ��ʿ����44ҳ  1@P //���������ε��ڷ������, ��Ҫ��� CHAMP ����
				threshold_slipsize_P1_L1 = 10;  // ֻ��̽�������, ��λ m
				threshold_rms_P1_L1      = 5.0;  // Ӧ��ͬʱ�̶�����������MW����޷�̽�⣬�����޵����������Ͻ����������� 3@P
				vondrak_PIF_eps          = 1.0E-10;//ƽ�����ӣ�ԽСƽ���̶�Խǿ
				vondrak_PIF_max          = 1.5;
				vondrak_PIF_min          = 0.3;
				vondrak_PIF_width        = 30;
				threshold_gap            = 300; // Ĭ��300s==5min
				max_pdop                 = 8.0;
				threshold_res_raim       = 3.0; // RAIM�����ֵĬ��3m ���BD2\BD3��Ҫȡ��һЩ���ĳ�7.0-15.0���ң�GPS��Ϊ3.5����
				threshold_gpssatcount    = 3;   // gps������������ֵ 
				threshold_editrms_code   = 2.50;
			    threshold_editrms_phase  = 0.10;
				bOn_ClockEliminate       = false;
				bOn_ClockJumpDiagnose    = false;
				bOn_SlipEditedMWDiagnose = false;
				bOn_IonosphereFree       = true;//�Ƿ�ʹ������������̽��������Ĭ���ǣ�
				bOn_POutlierAccordingL   = false;
				bOn_RaimSPP              = true;
				bOn_RaimArcChannelBias   = true;
				bOn_Slip_L1_L2           = false;
				bOn_GNSSSAT_AntPCOPCV    = true;
				bOn_PhaseWindUp          = true;
				typ_BDSobs_L1            = TYPE_OBS_L1;
				typ_BDSobs_L2            = TYPE_OBS_L5;				
				unloadPreprocessParaSy1();
			} 

			void loadPreprocessParaSy1()
			{
				max_ionosphere           = 10.0;
		        min_ionosphere           =-10.0;
				bOn_ClockEliminate       = true;
				bOn_ClockJumpDiagnose    = true;
				threshold_ClockJumpSize  = 4000;    // 2009/01/01, �����Ĵ�С��ֵ
				bOn_L2SNRLostDiagnose    = true;
				bOn_SlipEditedMWDiagnose = true;    // sy1���Ǳ��뿼��
				bOn_POutlierAccordingL   = true;
				bOn_RaimSPP              = false;
				bOn_RaimArcChannelBias   = false;
				bOn_Slip_L1_L2           = true;
			}

			void unloadPreprocessParaSy1()
			{
				max_ionosphere           = 30.0 + 5;
		        min_ionosphere           = 0.0 - 5;
				bOn_ClockEliminate       = false;
				bOn_ClockJumpDiagnose    = false;
				threshold_ClockJumpSize  = 4000;    // 2009/01/01, �����Ĵ�С��ֵ
				bOn_L2SNRLostDiagnose    = false;
				bOn_SlipEditedMWDiagnose = true;
				bOn_POutlierAccordingL   = false;
				bOn_RaimSPP              = true;//RAIM��鵥��
				bOn_RaimArcChannelBias   = true;//RAIM��黡��
				bOn_Slip_L1_L2           = false;//��Թ���Ƚϸߵ����ǿ�
			}
		};

		/*struct SLIPINFO
		{
			GPST                   t;
			BYTE                   id_sat;
			double                 size;
		};*/

		class LeoGPSObsPreproc
		{
		public:
			LeoGPSObsPreproc(void);
		public:
			~LeoGPSObsPreproc(void);
		public:
			// ˫Ƶ GPS �۲�����Ԥ����
			void    setPreprocPath(string strPreprocPath);
			void    setPreprocSatName(string strSatName);
			void    setSP3File(SP3File sp3File); 
			void    setCLKFile(CLKFile clkFile); 
			void    setObsFile(Rinex2_1_ObsFile obsFile);
			bool    loadSP3File(string  strSp3FileName);
			bool    loadCLKFile(string  strCLKFileName);
			bool    loadObsFile(string  strObsFileName);
			void    setAntPhaseCenterOffset(POS3D posRTN);
			void    setAntPhaseCenterOffset(POS3D posBody, Matrix matAxisBody2RTN);
			BYTE    obsPreprocInfo2EditedMark1(int obsPreprocInfo);
			BYTE    obsPreprocInfo2EditedMark2(int obsPreprocInfo);
			bool    SinglePointPositioning_PIF(int index_P1, int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableGPSCount, double& pdop, double& rms_res, double threshold = 1.0E-002);
			bool    RaimEstChannelBias_PIF(int index_P1,int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, int nPRN, POSCLK& posclk, double& channelBias, double threshold = 1.0E-002);
			int     RaimSPP_PIF(int index_P1, int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch& obsEpoch, POSCLK& posclk, double& pdop, double& rms_res);
			bool    detectRaimArcChannelBias_PIF(int index_P1,int index_P2, vector<Rinex2_1_LeoEditedObsEpoch> &obsEpochList, double threshold = 0.20);
			bool    detectL2SNRLost(int index_S2, int index_P1, int index_P2, int index_L1, int index_L2, Rinex2_1_EditedObsSat& obsSat);
			bool    detectCodeOutlier_ionosphere(int index_P1, int index_P2, Rinex2_1_EditedObsSat& obsSat,double frequence_L1 = GPS_FREQUENCE_L1,double frequence_L2 = GPS_FREQUENCE_L2);
			bool    detectPhaseSlip(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1,double frequence_L2, Rinex2_1_EditedObsSat& obsSat);
			bool    mainFuncDFreqGPSObsPreproc(Rinex2_1_LeoEditedObsFile  &editedObsFile,double FREQUENCE_L1 = GPS_FREQUENCE_L1,double FREQUENCE_L2 = GPS_FREQUENCE_L2);
            // TQ�߹��������ݴ���
			bool    mainFuncTQObs2Edt(Rinex2_1_LeoEditedObsFile  &editedObsFile,int type_obs_L1 = TYPE_OBS_L1,int type_obs_L2 = TYPE_OBS_L2);
			bool    TQSinglePointPositioning_PIF(int index_P1, int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableGPSCount, double& pdop, double& rms_res, double threshold = 1.0E-002);
			// ˫Ƶ GPS �۲����ݱ༭��Ҫ����
			void    setLeoOrbitList(vector<TimePosVel> leoOrbitList);
			bool    getLeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange = 8);
			bool    pdopSPP(int index_P1, int index_P2, Rinex2_1_LeoEditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop);
			bool    obsEdited_LIF(int index_L1, int index_L2, double frequence_L1,double frequence_L2,int nPRN, Rinex2_1_LeoEditedObsEpoch epoch_j_1, Rinex2_1_LeoEditedObsEpoch epoch_j, double &res, bool &slipFlag);
			bool    mainFuncDFreqGPSObsEdit(Rinex2_1_LeoEditedObsFile  &editedObsFile,int type_obs_L1 = TYPE_OBS_L1,int type_obs_L2 = TYPE_OBS_L2);
			// ��ϵͳ�۲�����Ԥ����
			bool    pdopMixedObsSPP(int index_P1_GPS, int index_P2_GPS, int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, int& eyeableSatCount, double& pdop);
			bool    mainFuncMixedObsPreproc(string  strMixedObsFileName, Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile,bool bOn_edit = true,int FLAG_BDS = 1);	
			// ��Ƶ���ݴ���
			bool    detectPhaseSlip_L1(int index_P1, int index_L1, Rinex2_1_EditedObsSat& obsSat);
			bool    mainFuncSFreqGPSObsPreproc(Rinex2_1_LeoEditedObsFile  &editedObsFile);
			bool    obsEdited_GRAPHIC(int index_P1, int index_L1, int nPRN, Rinex2_1_LeoEditedObsEpoch epoch_j_1, Rinex2_1_LeoEditedObsEpoch epoch_j, double &res, bool &slipFlag);
			bool    mainFuncSFreqGPSObsEdit(Rinex2_1_LeoEditedObsFile  &editedObsFile);
		private:
			bool    getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist); 
			bool    getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
			bool    datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		    bool    datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist);
			vector<Rinex2_1_LeoEditedObsEpoch>  m_editedObsEpochList;
			string  m_strPreprocPath;
			string  m_SatName;
		public:
			LeoGPSObsPreprocDefine m_PreprocessorDefine;
			CLKFile                m_clkFile; // �����Ӳ������ļ�
			SP3File                m_sp3File; // �������������ļ�
			Rinex2_1_ObsFile       m_obsFile; // ��ϵͳԭʼ�۲�����			
			POS3D                  m_pcoAnt;  // ����ƫ����		
			vector<TimePosVel>     m_leoOrbitList; // ����ѧ�ο����
			int                    m_countSlip;
			int                    m_countRestSlip; // ����Ϊ��ɾ����������
			Matrix                 m_matAxisAnt2Body;  // ����ϵ���ǹ�ϵ��ת�������������߿��ܰ�װ�ڷ��춥����
		};
	}
}