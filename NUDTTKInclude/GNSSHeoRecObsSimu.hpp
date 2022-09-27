#pragma once
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "Rinex2_1_ObsFile.hpp"
#include "Rinex2_1_MixedObsFile.hpp"
#include "Rinex3_03_EditedObsFile.hpp"
#include "Rinex3_0_ObsFile.hpp"
#include "TimeCoordConvert.hpp"
#include "jplEphFile.hpp"
#include <time.h>
#include "LeoGPSObsPreproc.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "igs05atxFile.hpp"
#include "AntPCVFile.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSYawAttitudeModel.hpp"

namespace NUDTTK
{
	namespace Simu
	{
		// HEO��GNSSϵͳ�ṹ
		struct HeoGNSS_MixedSys
		{
			// ��Ҫ��ʼ������
			char                                    cSys;                         // ϵͳ��ʶ
			string                                  name_C1;    // α��
			string                                  name_C2;
			string                                  name_L1;    // ��λ
			string                                  name_L2;
			string                                  nameFreq_L1;                  // ����PCV����
            string                                  nameFreq_L2;
			bool                                    bOn_P1; 
			bool                                    bOn_P2; 
			bool                                    bOn_L1; 
			bool                                    bOn_L2; 
			double                                  freq_L1;
			double                                  freq_L2;
			double									noiseSigma_P1;         // P1 �۲�������С
			double									noiseSigma_P2;         // P2 �۲�������С
			double									noiseSigma_L1;         // L1 �۲�������С
			double									noiseSigma_L2;         // L2 �۲�������С
			bool									bOn_GNSSSAT_AntPCOPCV;
			bool									bOn_GNSSSAT_Clk;
			bool									bOn_GNSSSAT_Relativity;
			// ����(Main Lobe)+�԰�(Side Lobe)
			double                                  coverAngleMainlobe;   // �����źŸ��ǽ�
			bool									bOn_SidelobeSignal;   // �Ƿ�ʹ���԰��ź�
			double                                  coverAngleSidelobe;   // �԰��źŸ��ǽ�
			double                                  transPowerMainlobe;   // �����źŷ��书��
			double                                  transPowerSidelobe;   // �԰��źŷ��书��
			// 
			double                                  angel_meo;
			double                                  angel_geo;
			// ����Ҫ��ʼ������
			int                                     index_C1;
			int                                     index_C2;
			int                                     index_L1;
			int                                     index_L2;
			int                                     iSys;                         // ��¼������m_editedMixedObsFile�洢��λ��
			string getSysFreqName()
			{
				// ���3_03��ϸ�ʽ�޸�, �ȵ·�, 2018/06/14 
				char sysFreqName[4];
				sprintf(sysFreqName, "%1c%1c%1c", cSys, name_L1[1], name_L2[1]); // "Gij" "Cij" "Eij" "Rxx"
				sysFreqName[3] = '\0';
				return sysFreqName;
			}
			HeoGNSS_MixedSys()
			{
				index_C1 = -1;
				index_C2 = -1;
				index_L1 = -1;
				index_L2 = -1;
				bOn_P1   = false;
				bOn_P2   = false;
				bOn_L1   = false;
				bOn_L2   = false;
				noiseSigma_P1          = 0.5;    
				noiseSigma_P2          = 0.5;    
				noiseSigma_L1          = 0.002;   
				noiseSigma_L2          = 0.002;  
				freq_L1                = GPS_FREQUENCE_L1;
				freq_L2                = GPS_FREQUENCE_L2;
				bOn_GNSSSAT_AntPCOPCV  = false;
				bOn_GNSSSAT_Relativity = false;
				bOn_GNSSSAT_Clk        = false;
				coverAngleMainlobe     = 42.0;
				transPowerMainlobe     = 0.0; //  ?ȷ��
				bOn_SidelobeSignal     = false;
				coverAngleSidelobe     = 50.0;
				transPowerSidelobe     = 0.0; //  ?ȷ��
				name_C1  = "";
				name_C2  = "";
				name_L1  = "";
				name_L2  = "";
				nameFreq_L1 = "";
				nameFreq_L2 = "";
				angel_meo   = 13.23;
				angel_geo   = 8.7;

			}
		};
		struct GNSSHeoRecObsSimuPara
		{ 			
			bool    bOn_Rec_AntPCO;
			bool    bOn_Rec_AntPCV;
			bool    bOn_Rec_Clock;
			double  recMaximumGain;      // ���ջ��������
			double  recClockAllanVar_h_0;
			double 	recClockAllanVar_h_2;
			bool    bOn_Rec_ObsNoise;
			bool    bOn_Rec_Relativity;
			bool    bOn_Ionosphere; 
			bool    bOn_PhaseWindUp;       // ��λ����       
			double  min_elevation;         // �����߶Ƚ���ֵ
			bool    bOn_SysBias;           // 2015/01/07,�����꣬�Ƿ�����ϵͳƫ��
			double  sysBias;               // 2015/01/07,�����꣬ϵͳƫ���С

			GNSSHeoRecObsSimuPara()
			{
				min_elevation          = 10.0;
				recClockAllanVar_h_0   = 0.05;
				recClockAllanVar_h_2   = 1.0E-15;
				recMaximumGain         = 0.0;  // ?ȷ��
				bOn_Rec_AntPCO         = false;
				bOn_Rec_AntPCV         = false;
				bOn_Ionosphere         = false; 
				bOn_Rec_Clock          = false;
				bOn_Rec_ObsNoise       = false;
				bOn_Rec_Relativity     = false;
				bOn_PhaseWindUp        = false;
				bOn_SysBias            = false;
				sysBias                = 1.0;
			}
		};

		class GNSSHeoRecObsSimu
		{
		public:
			GNSSHeoRecObsSimu(void);
		public:
			~GNSSHeoRecObsSimu(void);
		public:
			void   setSP3File(SP3File sp3File);
			void   setCLKFile(CLKFile clkFile);
			bool   loadSP3File(string  strSP3FileName);
			bool   loadCLKFile(string  strCLKFileName);
			bool   loadCLKFile_rinex304(string  strCLKFileName);
			void   setAntPCO(double x,double y,double z);
			bool   simuRecClock(double& p_t, double& q_t, double delta, double AllanVar_h_0 = 0.05, double AllanVar_h_2 = 1.0E-15);
		public: 
			// ����strSatName-G01��C01���������Ǳ����ѡ�����������źŽǶȣ��滻ԭ��judgeGPSSignalCover
			bool   judgeGNSSSignalCover(HeoGNSS_MixedSys heoGNSSsys_i, string strSatName, POS3D posGNSSSat, POS3D posRec, double freq_i, double cut_elevation = 10.0);
			// ��ע�������⣺ʹ�õ�Ƶ or ˫Ƶ����λ������������ʹ�ø�˹�������������辡���ӽ���ʵ�������������Ӱ���
			bool   simuGNSSMixedObsFile(Rinex2_1_MixedObsFile &obsFile);
		private:
			SP3File                  m_sp3File;    // GPS�������������ļ�
			CLKFile                  m_clkFile;
		public:
			GNSSHeoRecObsSimuPara     m_simuParaDefine;
			JPLEphFile				  m_JPLEphFile;       // JPL DE405 ���������ļ�
			TimeCoordConvert		  m_TimeCoordConvert;  // ʱ������ϵת��
			svnavMixedFile            m_svnavMixedFile;    // +
			GNSSYawAttitudeModel      m_gymMixed;	       // +
			vector<TimePosVel>        m_pvOrbList;
			igs05atxFile	          m_AtxFile;           // ���������ļ�
			AntPCVFile                m_pcvFile;           // ������λ����
			POS3D                     m_pcoAnt;            // ���ջ�����ƫ��ʸ��	
			vector<HeoGNSS_MixedSys>  m_dataMixedSysList;  // +
		};
	}
}
