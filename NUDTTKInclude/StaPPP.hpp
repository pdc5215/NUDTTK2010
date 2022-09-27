#pragma once
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "igs05atxFile.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#include "dynPODStructDef.hpp"
#include "AntPCVFile.hpp"
#include "StaOceanLoadingDisplacementFile.hpp"
#include "Troposphere_model.hpp"
#include "TROZPDFile.hpp"
#include "GPSYawAttitudeModel1995.hpp"
#include "Rinex2_1_MixedEditedObsFile.hpp"
#include "Rinex3_0_ObsFile.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"

//  Copyright 2013, The National University of Defense Technology at ChangSha
using namespace NUDTTK::Geodyn;
namespace NUDTTK
{
	namespace GNSSPrecisePointPositioning
	{
		struct StaticPPPSolution_Epoch
		{
			DayTime t;
			double  clock;
			int     pppMark;  // ��¼��ʱ�̵ĵ��㶨λ������, 0: ��ֵ���; 1: ��Ч��(α��Լ����); 2: ��λԼ����
		    double  pdop;  
		};

		struct StaticPPPSolution
		{
			POS3D pos_Init; 
			POS3D pos_Est;                                      // ��̬����ֻ����һ��λ�ò���
			vector<TropZenithDelayEstPara>  zenithDelayEstList; // �������춥�ӳٲ�����ÿ 2-6 Сʱ���� 1 ��
			vector<TropGradEstPara>         troGradEstList;     // ������ˮƽ�ݶȲ�����ÿ24Сʱ���� 1 ��,2014/10/28			
			vector<StaticPPPSolution_Epoch> clkEstList;         // �Ӳ����ÿ����Чʱ�̹���һ��
			int getIndexZenithDelayEstList(GPST t);
			int getIndexTroGradEstList(GPST t);                 // 2014/10/28
		};

		struct KinematicPPPSolution_Epoch
		{
			DayTime t;
			POS3D   pos;
			double  clock;
			int     pppMark;  // ��¼��ʱ�̵ĵ��㶨λ������, 0: ��ֵ���; 1: ��Ч��(α��Լ����); 2: ��λԼ����
			double  pdop;  
			double  sigma_L;
			int     eyeableGPSCount_L;
		};

		struct KinematicPPPSolution
		{
			vector<TropZenithDelayEstPara>     zenithDelayEstList; // �������춥�ӳٲ���ÿ 2 Сʱ���� 1 ��
			vector<TropGradEstPara>            troGradEstList;     // ������ˮƽ�ݶȲ�����ÿ24Сʱ���� 1 ��,2014/10/28	
			vector<KinematicPPPSolution_Epoch> posclkEstList;      // �Ӳ����ÿ����Чʱ�̹���һ��
			int getIndexZenithDelayEstList(GPST t);
			int getIndexTroGradEstList(GPST t);                 // 2014/10/28
		};

		struct pppClean_ObsEqArcElement
		{      
			int    nObsTime;     // �۲�ʱ�����
			double obs;          // ԭʼ��λ�޵������Ϲ۲���
			double res;   
			double robustweight; // ³�����Ƶ���Ȩ
			double prioriweight; // ����Ȩֵ
			double obs_MW;       // ���ڼ���MWģ����
			int    prepro_flag;  // ������������
			double obs_L1_L2;
			double t;
			double recClk;       // ���ջ��Ӳ��������������2015/01/10,������

			pppClean_ObsEqArcElement()
			{
				robustweight = 1.0;
				res = 0.0;
				prioriweight = 1.0;
				recClk = 0.0;
			}
		};

		typedef map<int, pppClean_ObsEqArcElement> pppClean_ObsEqArcElementMap;

		struct pppClean_ObsEqArc
		{
			double                      ambiguity_LIF;   // �������ģ����
			double                      ambiguity_MW;    // MWģ����
			int                         id_Sat;          // ���Ǳ��
			int                         slip_L1;
			int                         slip_L2;
			int                         bSlipMarked;     // 0, �»���; 1, ������δ�޸���;  2, ���������޸���
			pppClean_ObsEqArcElementMap obsList;

			int  updateRobustWeight(vector<int> &slipindexlist, double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double factor = 3, double threshold_max = DBL_MAX);
			int  updateRobustWeight_SlipCheck(vector<int> &slipindexlist, double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double factor = 3, double threshold_max = DBL_MAX);
			bool getLast_L1_L2_polyfit(double t_extra, double &L1_L2_extra, int m = 3, double max_span = 1200.0);
			bool getFirst_L1_L2_polyfit(double &L1_L2, int m = 3, double max_span = 1200.0);

			pppClean_ObsEqArc()
			{
				slip_L1 = 0;
				slip_L2 = 0;
				bSlipMarked = 0; 
			}
		};

		struct StaPPPPara
		{
			double       max_pdop;                  // ���ξ���������ֵ(��Ȩ��), ������ֵ�Ĺ۲�㽫����������
	        int          min_eyeableGPSCount;       // ��С�������Ǹ���
			int          sampleInterval;            // �������ݵĲ������(s)
			double       apriorityRms_PIF;          // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double       apriorityRms_LIF;          // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double       max_arclengh; 
			unsigned int min_arcpointcount;         // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       min_elevation;
			double       apriorityRms_TZD_abs;      // �����վ������춥�ӳپ���, ���ڵ�����춥�ӳپ���Լ�����̼�Ȩ����
			double       apriorityRms_TZD_rel;      // �������ڵ�����춥�ӳٲ��������Լ�����̼�Ȩ����
			double       apriorityWet_TZD;          // ��վ������ʪ������������ֵ
			double       apriorityWet_TZD_period;   // ��վ������ʪ������������
			double       apriorityRms_Grad_abs;     // ˮƽ�ݶȾ���Լ��			
			double       apriorityRms_Grad_period;  // ˮƽ�ݶȹ�������
			int          min_Wet_TZD_ncount;        // �������������ϲ���ֵ
			int          zpdProductInterval;        // ����Ķ������Ʒ�������(s)			
			bool         bOn_WeightElevation;       // �Ƿ���и߶ȽǼ�Ȩ
			bool         bOn_Clk_GPSSAT;
			bool         bOn_GPSRelativity;         // �Ƿ���� GPS ��������۸���,���ƫ�ĸ���
			bool         bOn_GraRelativity;         // �Ƿ�����������������۸���
			bool         bOn_GPSAntPCOPCV;          // �Ƿ���� GPS ��������PCO/PCV����
			bool         bOn_RecAntARP;             // �Ƿ���н������߲ο�������
			bool         bOn_RecAntPCOPCV;          // �Ƿ���н���������λ��������
			bool         bOn_PhaseWindUp;
			bool         bOnEst_StaTropZenithDelay; // �Ƿ���в�վ�������춥�ӳٹ���
			bool         bOnEst_StaTroGradient;     // �Ƿ���в�վ������ˮƽ�ݶȹ���
			bool         bOn_SolidTides;
			bool         bOn_OceanTides;
			bool         bOn_TROProduct;            // �Ƿ�����������Ʒ
			bool         bOn_GYM95;                 // �Ƿ����GYM95ģ�ͼ���������̬
			int          troProduct_doy;            // ����������Ʒ������գ��ƻ��������������
			GPST         troProductName_Time;       //��Ƶ���ļ������ο���Ԫ���ϱ���2016/1/19
			double       robustfactor_OC_edited;
			double       threshold_OC_max;
			bool         bOn_DebugInfo_Shadow;      // �Ƿ������Ӱ��Ϣ
			
			bool         bOn_GEOSatWeight;          // �Ƿ�Ա���GEO���Ǽ�Ȩ����
			double       GEOSatWeight;              // ����GEO����Ȩ��
			bool         bOn_AntPCV;                // 2015/01/14,����PCV��������

			StaPPPPara()
			{
				max_pdop                  = 4.5;
				min_eyeableGPSCount       = 5;
				sampleInterval            = 300;
				apriorityRms_PIF          = 0.50;
				apriorityRms_LIF          = 0.005;
				bOn_WeightElevation       = false;
				min_elevation             = 10.0;
				max_arclengh              = 3600.0;
				min_arcpointcount         = 20;                    
				apriorityRms_TZD_abs      = 0.5;
				apriorityRms_TZD_rel      = 0.04;
				apriorityWet_TZD          = 0;        // ��վ������ʪ������������ֵ
				apriorityWet_TZD_period   = 3600 * 2; // ����(2-6h)
				apriorityRms_Grad_abs     = 0.03;     // GAMIT�������ԣ����������Լ��(Reza Ghoddousi-Fard,2009)				
				apriorityRms_Grad_period  = 3600 * 24;// GAMIT,Bernese
				min_Wet_TZD_ncount        = 20;       // ���ڴ�ֵ��ϲ���������
				zpdProductInterval        = 300;
				bOn_Clk_GPSSAT            = true;
				bOn_GPSRelativity         = true;
				bOn_GraRelativity         = false;
				bOn_GPSAntPCOPCV          = true;
				bOn_PhaseWindUp           = true;
				bOn_RecAntARP             = true;
				bOn_RecAntPCOPCV          = true;
				bOnEst_StaTropZenithDelay = true;
				bOnEst_StaTroGradient     = false;
				bOn_SolidTides            = true;
				bOn_OceanTides            = true;
				bOn_TROProduct            = false;
				bOn_GYM95                 = true;
				troProduct_doy            = 0;
				robustfactor_OC_edited    = 3.0;
				threshold_OC_max          = DBL_MAX;
				bOn_GEOSatWeight          = true;
				GEOSatWeight              = 1.0/3.0;
				bOn_AntPCV                = true;
				bOn_DebugInfo_Shadow      = false;
			}
		};
		
		//Ϊ�����ںϹ۲�������ӵ����ݽṹ   2017/3/31  ��̍
		struct MixedGNSSPPPDatum
		{
			char                                  recType_CPN;                  //���ջ����ͣ���Ҫ���GPS����
			char                                  cSystem;                      //ϵͳ��ʶ
	 		vector<Rinex2_1_MixedEditedObsEpoch>  editedObsEpochlist;           //��ϵͳ��Ԫ��ʽ�۲����ݣ���ϸ�ʽ
			vector<Rinex2_1_MixedEditedObsSat>    editedObsSatlist;             //��ϵͳ���Ǹ�ʽ�۲����ݣ���ϸ�ʽ
			map<int, PODEpoch>                    mapDynEpochList;              //��Ԫ����������
			vector<ObsEqEpoch>                    P_IFEpochList;                //�������α�����ݣ���Ԫ�ǣ�
			vector<ObsEqEpoch>                    L_IFEpochList;                //�������α�����ݣ���Ԫ�ǣ�
			vector<pppClean_ObsEqArc>             L_IFArcList;                  //���������λ���ݣ����μǣ�, ������ں϶������õ����ݽṹ��ͬ
			int                                   ambiguityIndexBegin;          //��ϵͳģ������ʼ��ţ������ں����ݴ���ʱ��¼���εĳ�ʼ����
			map<int, double>                      mapWindupPrev;                //��¼Windup��������
			vector<int>                           mixedEpochIdList;             //��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ��ţ������໥����
			vector<int>                           epochIdList;                  //��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ��ţ������໥����
			vector<O_CResEpoch>                   ocResP_IFEpochList;           //�������α��O-C�в�
			vector<O_CResArc>                     ocResL_IFArcList;             //���������λO-C�в�
			double                                FREQUENCE_L1;                 //�˴������¶����еĵ�Li,Pi��i=1,2��ֻ����ʽ�ϵļǷ���ʵ�ʿ��ܶ�Ӧ��1,2,5����Ƶ���е�ĳһ��
			double                                FREQUENCE_L2;
			double                                WAVELENGTH_L1;
			double                                WAVELENGTH_L2;
			double                                coefficient_L1;
			double                                coefficient_L2;
			int                                   index_P1;
			int                                   index_P2;
			int                                   index_L1;
			int                                   index_L2;
			double                                pSlipRepairValue_L1[MAX_PRN];
			double                                pSlipRepairValue_L2[MAX_PRN];
			double                                pRobustWeight_code[MAX_PRN];
			BYTE                                  pbySatList[MAX_PRN];
			double                                weightSystem;                 //ϵͳ���Ȩϵ��֮�ȣ�Ĭ��1.0
			double                                sysBias;                      //��GPSΪ�ο�ϵͳ��ϵͳ��ƫ�Ĭ��Ϊ0

			MixedGNSSPPPDatum(double frequence1 = GPS_FREQUENCE_L1, double frequence2 = GPS_FREQUENCE_L2)
			{
				ambiguityIndexBegin = 0;
				FREQUENCE_L1        = frequence1;
				FREQUENCE_L2        = frequence2;
				WAVELENGTH_L1       = SPEED_LIGHT / FREQUENCE_L1;
				WAVELENGTH_L2       = SPEED_LIGHT / FREQUENCE_L2;
				coefficient_L1      = 1 / (1-pow(FREQUENCE_L2 / FREQUENCE_L1, 2));
				coefficient_L2      = 1 / (1-pow(FREQUENCE_L1 / FREQUENCE_L2, 2));
				memset(pSlipRepairValue_L1, 0, sizeof(double) * MAX_PRN);
				memset(pSlipRepairValue_L2, 0, sizeof(double) * MAX_PRN);
				memset(pRobustWeight_code , 0, sizeof(double) * MAX_PRN);
				memset(pbySatList         , 0, sizeof(BYTE) * MAX_PRN);
				weightSystem        = 1.0;
				sysBias             = 0.0;
			}
		};

		typedef vector<MixedGNSSPPPDatum>  MixedGNSSPPPDatumList;

		//Ϊ�����ں��������Ӳ����  ��̍
		struct  MixedClkSP3Datum
		{
			char      cSystem;
			SP3File   sp3File;
			CLKFile   clkFile;
		};

		struct  MixedAtxDatum
		{
			char          cSystem;
			igs05atxFile  atxFile;
		};

		class StaPPP
		{
		public:
			StaPPP(void);
		public:
			~StaPPP(void);
		public:
			void setSP3File(SP3File sp3File); 
			void setCLKFile(CLKFile clkFile); 
			bool loadSP3File(string  strSp3FileName);
			bool loadCLKFile(string  strCLKFileName);
			void weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			bool pdopSPP(int index_P1, int index_P2, POS3D recPos, Rinex2_1_EditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop, double FREQUENCE_L1 = GPS_FREQUENCE_L1, double FREQUENCE_L2 = GPS_FREQUENCE_L2);
			bool SinglePointPositioning_PIF(int index_P1, int index_P2, POSCLK& posclk, Rinex2_1_EditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop, double& rms_res, double FREQUENCE_L1 = GPS_FREQUENCE_L1, double FREQUENCE_L2 = GPS_FREQUENCE_L2, char cSatSystem = 'G', double threshold = 1.0E-002);
			bool pdopSPP_BDS(int index_P1, int index_P2, POS3D recPos, Rinex2_1_EditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop, double FREQUENCE_L1 = GPS_FREQUENCE_L1, double FREQUENCE_L2 = GPS_FREQUENCE_L2);
		    bool pppclean(Rinex2_1_EditedObsFile &editedObsFile,  StaticPPPSolution& pppSolution, string outputPath = "");
			bool pppclean_GPS(Rinex2_1_EditedObsFile &editedObsFile,  StaticPPPSolution& pppSolution, string outputPath = "");
			bool pppclean_kinematic(Rinex2_1_EditedObsFile &editedObsFile, KinematicPPPSolution& pppSolution, string outputPath = "");
			bool pppclean_BDS(Rinex2_1_EditedObsFile &editedObsFile,  StaticPPPSolution& pppSolution, string outputPath = "");
			bool staticPPP_phase(string editedObsFilePath,  StaticPPPSolution& pppSolution, bool bResEdit = true);
			bool staticPPP_phase_BDS(string editedObsFilePath,  StaticPPPSolution& pppSolution, bool bResEdit = true);
			bool kinematicPPP_phase(string editedObsFilePath,  KinematicPPPSolution& pppSolution, bool bResEdit = true);
			int  getFlagIGSPCVInfo();
			//��ϵͳ�ں�PPP���
			bool loadSP3CLKFileSet(char *strSys, vector<string> strSp3FileNameSet, vector<string> strClkFileNameSet, int num_Sys);
			bool pppMixedObsPreproc(string  strMixedObsFileName, Rinex2_1_MixedEditedObsFile  &mixedEditedObsFile);	
			bool pppclean_Mixed(Rinex2_1_MixedEditedObsFile  &mixedEditedObsFile, StaticPPPSolution& pppSolution, MixedGNSSPPPDatumList &multiGNSSData, bool &bOnGPS, bool &bOnBDS, string outputPath = "");
		private:
			bool loadEditedObsFile(string  strEditedObsFileName);
			int  m_flagIGSPCVInfo; // 0: ȱʧ; 1: ��ȷ����; 2: ���, ���ڷ���			
		public:
			StaPPPPara                        m_pppParaDefine;
			CLKFile                           m_clkFile;		   // �����Ӳ������ļ�
			SP3File                           m_sp3File;		   // �������������ļ�
			Rinex2_1_EditedObsFile            m_editedObsFile;	   // ԭʼ�۲�����
			ENU                               m_arpAnt;			   // ����ƫ����
			svnavFile                         m_svnavFile;		   // GPS����ƫ��
			igs05atxFile			          m_AtxFile;		   // ���������ļ�(2013/04/18, �ϱ�)
			StaOceanTide                      m_sotDatum;          // �����ļ�
			vector<O_CResEpoch>               m_ocResP_IFEpochList;// �޵����α��O-C�в�
			vector<O_CResArc>                 m_ocResL_IFArcList;  // �޵������λO-C�в�
		public:
			TimeCoordConvert                  m_TimeCoordConvert;  // ʱ������ϵת��
			JPLEphFile                        m_JPLEphFile;        // JPL DE405���������ļ�
			GPSYawAttitudeModel1995           m_GYM95;             // ��ȡ��̬����ģʽ�µ�������̬

		   // �ۿ���2019/09/30����ģ���ļ�
			svnavMixedFile                  m_svnavMixedFile;
			GNSSYawAttitudeModel            m_gymMixed;						
			map<string, AntCorrectionBlk>   m_mapGnssPCVBlk;// ��ȡ����PCV��Ϣ

			//�����ں��������ӵı���  ��̍
		public:
			typedef vector<MixedClkSP3Datum>  clksp3FileList;      // ��ϵͳ�洢�������Ӳ�����
			clksp3FileList                    m_clksp3FileList;
			typedef vector<MixedAtxDatum>     atxFileList;         // ��ϵͳ�洢������������
			atxFileList                       m_AtxFileList;

		};
	}
}
