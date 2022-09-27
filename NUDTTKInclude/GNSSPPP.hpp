#pragma once
#include "Rinex3_03_EditedObsFile.hpp"
#include "Rinex3_0_ObsFile.hpp"
#include "dynPODStructDef.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"
#include "igs05atxFile.hpp"
#include "StaOceanLoadingDisplacementFile.hpp"
#include "ParaEliminationNEQ.hpp"

//  Copyright 2017, The National University of Defense Technology at ChangSha
using namespace NUDTTK::Math;
using namespace NUDTTK::Geodyn;
namespace NUDTTK
{
	namespace GNSSPrecisePointPositioning
	{
		// �۲ⷽ����ԪԪ��, ��Ҫ���ڴ洢α������
		struct PPP_ObsEqElement 
		{ 
			//int  id_Ambiguity;  // ģ�������
			double obs;           
			float  oc;
			float  rw;            // ³��Ȩ

			PPP_ObsEqElement()
			{
				rw = 1.0f;
				oc = 0.0f;
			}
		};

		struct PPP_ObsEqEpoch
		{
			int                          id_Epoch;    // �۲�ʱ�����, ���������ؼ�����
			map<string, PPP_ObsEqElement>  obsList;     // ��ͬ���ǹ۲��б�
		};

		struct PPP_CleanArcElement
		{   
			// ���ڹ��취����
			int    nObsTime;     // �۲�ʱ�����
			double LIF;
			float  wL;           // ����۲�Ȩֵ, ��߶Ƚ��й�
			float  rw;           // �༭���Ȩֵ
			float  oc;
            // ��������̽�����޸�
			double dt;
			double MW;           // ���ڼ������ģ����
			double iono_L1_L2;
			//int  flagEdited;   // ������������
			//double clock;      // �Ӳ�

			PPP_CleanArcElement()
			{
				rw = 1.0f;
				oc = 0.0f;
				wL = 1.0f;
			}
		};

		// ��Ҫ�������ݽṹ, ��Ԫ�۲����ݵĹ�����Ҫ���νṹ
		struct PPP_CleanArc 
		{
			string    nameSat;
			string    nameFreq;     // ���ڲ���Ƶ������� 
			double    ambiguity; 
			double    ambiguity_MW; 
			short int id_Ambiguity; 
			int       count; 
			int       markSlip;     // 0-�»���; 1-������δ�޸���; 2-���������޸���
			int       slip_L1;
			int       slip_L2;
			double    freq_L1;      // ��¼Ƶ����Ϣ, ���ں���ʹ��
			double    freq_L2;

			map<int, PPP_CleanArcElement> obsList;

			int  updateRobustWeight(vector<int> &slipIndexList, double threshold_slip, double threshold_rms, size_t threshold_count = 20, double ratio_ocEdit = 3, double max_ocPhaseEdit = DBL_MAX);
			bool backwordIonoPolyfit(double t_Extra, double &iono_Extra, int m = 3, double max_Span = 1200.0);
			bool forwordIonoPolyfit(double &iono_0, int m = 3, double max_Span = 1200.0);
		};

		// ������ⷨ����
		struct PPP_NEQElement
		{
			double  obscorrected_value;      // �۲����ݸ�����-��λ
			//double  obscorrected_value_code; 
			POSCLK  vecLos;                  // ����ʸ��, ���Ƕ���ʱʹ��
			float   Elevation;
			float   Azimuth;
			float   weightCode;              // α��۲�Ȩֵ
			float   weightPhase;             // ��λ�۲�Ȩֵ
			float   gmfh;                    // ������ɷ���ӳ�亯����ƫ������
			float   gmfw;                    // ������ʪ����ӳ�亯����ƫ�����������ڶ�����ʪ��������
			BYTE    mark_GNSSSatShadow;      // �����Ӱ���, Ĭ�� 0 �� δ�����Ӱ
			bool    bEphemeris;
			bool    bEdited;
			
			PPP_NEQElement()
			{
				gmfh = 0.0f;
				gmfw = 0.0f;
				mark_GNSSSatShadow = 0;
				bEphemeris = false;
				bEdited = false;
			}
		};

		struct PPP_NEQEpoch
		{
			int                         validIndex;      // ��Чʱ���ǩ  (������λ�۲ⷽ����ƾ���������Чλ�ò����ţ�
			int                         eyeableSatCount;
			// �˶�ѧ��λ����
			bool                        sppMark;         // ���㶨λ�ɹ����
			double                      pdop;            // ���ξ�������
			
			map<string, PPP_NEQElement> mapDatum;
		};

		struct PPP_SOLEpoch
		{
			// ��̬����
			GPST    t;
			double  clock;
			int     pppMark;         // ����: 0-��ֵ���, δ��������; 1-α���; 2-��λ��
			double  pdop;  
            // ��̬��չ����
			POS3D   pos;
			int     eyeableSatCount; 
			double  sigma;
		};

		struct PPP_SOL
		{
			POS3D pos0; 
			POS3D posEst;  
			vector<PPP_SOLEpoch> solEpochEstList; 
			vector<TropZenithDelayEstPara>   zenithDelayEstList; // �������춥�ӳٲ�����ÿ 2-6 Сʱ���� 1 ��
			vector<TropGradEstPara>         troGradEstList;     // ������ˮƽ�ݶȲ�����ÿ24Сʱ���� 1 ��,2014/10/28	
			map<string, double>             freqBiasList;       // Ƶ��ƫ�����, ����Ƶ������, string����ΪnameFreq
			int getIndex_ZenithDelayEstList(GPST t);
			int getIndex_TroGradEstList(GPST t);                 
		};

		struct SPP_SOLEpoch
		{
			GPST    t;
			POS3D   pos;       // ÿһʱ��һ��λ��
			double  clock;
			int     sppMark;  // ��¼��ʱ�̵ĵ��㶨λ������, 0: ��ֵ���; 1: ��Ч��(α��Լ����)
			double  pdop;  
			double  sigma;
			int     eyeableSatCount;
		};

		struct SPP_SOL
		{
			vector<TropZenithDelayEstPara>   zenithDelayEstList; // �������춥�ӳٲ�����ÿ 2-6 Сʱ���� 1 ��
			vector<TropGradEstPara>         troGradEstList;     // ������ˮƽ�ݶȲ�����ÿ24Сʱ���� 1 ��,2014/10/28	
			vector<SPP_SOLEpoch>            posclkEstList;                 // �Ӳ����ÿ����Чʱ�̹���һ��
			int getIndexZenithDelayEstList(GPST t);
			int getIndexTroGradEstList(GPST t);                 // 2014/10/28
		};
		struct PPP_Freq
		{
			double                                  freq_L1;
			double                                  freq_L2;
			//string                                  nameFreq;   // Ƶ������ "G" "C" "E" "Rxx"
		};

		struct PPP_MixedSys
		{
			// ��Ҫ��ʼ������
			char                                    cSys;                         // ϵͳ��ʶ
			double                                  wSys;                         // ϵͳȨֵ, Ĭ�� 1.0
			//char                                  recType;                      // ���ջ����ͣ���Ҫ���GPS����
			string                                  name_C1;    // α��
			string                                  name_C2;
			string                                  name_L1;    // ��λ
			string                                  name_L2;
			string                                  nameFreq_L1;                  // ����PCV����
            string                                  nameFreq_L2;
			PPP_Freq                                freqSys;
			map<string, PPP_Freq>                   freqSatList;                  // ��Ҫ���GLONASS��string����Ϊ������
			float                                   priorsigma_PIF;               // ����α���Ȩ����

			// ����Ҫ��ʼ������
			int                                     index_C1;
			int                                     index_C2;
			int                                     index_L1;
			int                                     index_L2;
			int                                     iSys;                         // ��¼������m_editedMixedObsFile�洢��λ��
			vector<Rinex3_03_EditedObsEpoch>        editedObsEpochList;           // ���3_03��ϸ�ʽ�޸�, �ȵ·�, 2018/06/14 
			vector<Rinex3_03_EditedObsSat>          editedObsSatlist;             // ���3_03��ϸ�ʽ�޸�, �ȵ·�, 2018/06/14 
			map<int, PPP_ObsEqEpoch>                P_IFEpochList;                // ���������������α�����ݣ���Ԫ�ṹ��������map��ʽ����������intΪnObsTime
			vector<PPP_CleanArc>                    L_IFArcList;                  // �����������������λ���ݣ����νṹ�� 
			map<int, PPP_NEQEpoch>                  mapNEQEpochList;              // ��Ԫ����������, ��editedObsEpochlist���Ӧ��intΪnObsTime
			int                                     id_Ambiguity_0;               // ģ������ʼ���
			map<string, double>                     mapSlipFixing_L1;             // ÿ�����ǵ������޸�ֵ
			map<string, double>                     mapSlipFixing_L2;
			map<string, int>                        mapObsCount;                  // ÿ�����ǵĹ۲����ݸ���

			string getSysFreqName()
			{
				// ���3_03��ϸ�ʽ�޸�, �ȵ·�, 2018/06/14 
				char sysFreqName[4];
				sprintf(sysFreqName, "%1c%1c%1c", cSys, name_L1[1], name_L2[1]); // "Gij" "Cij" "Eij" "Rxx"
				sysFreqName[3] = '\0';
				return sysFreqName;
			}

			// ע: oc�в��, α��O-C�в���P_IFEpochList��, ��λO-C�в���L_IFArcList��
			PPP_MixedSys()
			{
				wSys = 1.0;
				//recType = 'N';
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
				priorsigma_PIF = 0.50f;
			}
		};

		struct PPP_DEF
		{
			bool                         on_KineSolving;        // �Ƿ�����˶�ѧ
			bool                         on_RINEX_3_02;
			bool                         on_WeightElevation;
			bool                         on_WeightGEO;
			bool                         on_PhaseWindUp;
			bool                         on_SolidTides;
			bool                         on_OceanTides;
			bool                         on_GnssRelativity;
			bool                         on_GnssPCV;
			bool                         on_RecARP;
			bool                         on_RecPCV;
			bool                         on_GraRelativity;
			bool                         on_UsedInShadow;
			bool                         on_EstTropZenithDelay;
			bool                         on_EstTropGradient;
			bool                         on_SlipEditedInfo;
			float                        weightGEO;
			BYTE                         min_EyeableSatCount;   // ��С�������Ǹ���
			double                       max_ArcInterval;
			size_t                       min_ArcPointCount;
			float                        min_Elevation;
			double                       max_ArcLengh;
			double                       max_WeightPDOP;         // ���ξ���������ֵ(��Ȩ��), ������ֵ�Ĺ۲�㽫����������, �˶�ѧ��λ��ʹ��
			double                       max_Sigma;              // ������λ���ξ�������Ȩֵ, ��Ҫ������λ
			double                       max_ocPhaseEdit;
			float                        weightInShadow;
			double                       period_WetTropZenithDelay;
			double                       period_TropGradient;
			double                       ratio_ocEdit;
			float                        priorsigma_LIF;         // ������λ��Ȩ����
			float                        priorsigma_TZD_abs;     // ������������Լ��
			float                        priorsigma_TZDGrad_abs;
			bool                         spp_sf; // ��Ƶ
			
			PPP_DEF()
			{
				spp_sf                    = false;
				on_KineSolving            = false;
				on_WeightElevation        = true;
				on_WeightGEO              = true;
				on_PhaseWindUp            = true;
				weightGEO                 = 1.0f / 3.0f;
				min_EyeableSatCount       = 2;
				if(on_KineSolving)
					min_EyeableSatCount   = 4;
				max_ArcInterval           = 3600.0;
				min_ArcPointCount         = 20;
				on_UsedInShadow           = false;
				weightInShadow            = 0.2f;
				on_EstTropZenithDelay     = true;
				period_WetTropZenithDelay = 3600.0 * 2;
				on_EstTropGradient        = true;
				period_TropGradient       = 3600.0 * 24;
				on_SolidTides             = true;
				on_OceanTides             = true;
				min_Elevation             = 5.0f;
				on_GnssRelativity         = true;
				on_GnssPCV                = true;
				on_RecARP                 = true;
				on_RecPCV                 = true;
				on_RINEX_3_02             = true;
				on_GraRelativity          = true;
				ratio_ocEdit              = 3.0;
				max_ocPhaseEdit           = DBL_MAX;
				on_SlipEditedInfo         = false;
				max_ArcLengh              = 3600.0;
				max_WeightPDOP            = 4.5;
				max_Sigma                 = 4.5;
				priorsigma_LIF            = 0.005f;
				priorsigma_TZD_abs        = 0.2f;   // ͨ������ģ�ͼ���Ķ�����ʪ�����춥�ӳ��п��ܳ���10cm
				priorsigma_TZDGrad_abs    = 0.004f; // �ݶȵĹ���������Сһ�㲻����2mm
			}
		};

		// ��ϵͳ(BDS+GPS)��GLONASSƵ�ֶ�ַ����̬�;�̬�ܷ�ͳһ? ����ʱ�̲�����Լ����
		class GNSSPPP
		{
		public:
			GNSSPPP(void);
		public:
			~GNSSPPP(void);
		public:
			void weighting_Elevation(float Elevation, float& weight_P_IF, float& weight_L_IF);
			bool pdopEpoch_MixedGNSS(double& pdop, vector<POSCLK> vecLosList, vector<float> wList);
			bool mainPPP_MixedGNSS(PPP_SOL& pppSol, string outputEditedObsFilePath = "");
			bool mainPPP_MixedGNSS_ParaElim(PPP_SOL& pppSol, string outputEditedObsFilePath = "");
			bool mainSPP_MixedGNSS(SPP_SOL& sppSol, string outputEditedObsFilePath = "", char cSystem = 'G');  // ��ϵͳ��׼���㶨λ,����������Ԥ����
			bool SinglePointPositioning_PIF(int index_P1, int index_P2, POSCLK& posclk, Rinex3_03_EditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop, double& rms_res, double FREQUENCE_L1 = GPS_FREQUENCE_L1, double FREQUENCE_L2 = GPS_FREQUENCE_L2, char cSatSystem = 'G', double threshold = 1.0E-002);
			bool SinglePointPositioning_SF(int index_P1, POSCLK& posclk, Rinex3_03_EditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop, double& rms_res, char cSatSystem = 'G', double threshold = 1.0E-002);

		public:
			JPLEphFile                   m_jplEphFile;         // JPL DE405���������ļ�
			TimeCoordConvert             m_timeCoordConvert;   // ʱ������ϵת��
		public:
			PPP_DEF                      m_pppDefine;
			CLKFile                      m_clkFile;		       // �����Ӳ������ļ�
			SP3File                      m_sp3File;		       // �������������ļ�
            svnavMixedFile               m_svnavMixedFile;
			GNSSYawAttitudeModel         m_gymMixed;
			igs05atxFile			     m_atxFile;		       // ���������ļ�
			StaOceanTide                 m_staOceanTide;       // ��������
			Rinex3_03_EditedObsFile      m_editedMixedObsFile; // ���3_03��ϸ�ʽ�޸�, �ȵ·�, 2018/06/14 
			vector<PPP_MixedSys>         m_dataMixedSysList;
		};
	}
}
