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

using namespace NUDTTK::SpaceborneGPSPreproc;
namespace NUDTTK
{
	namespace TQGNSSPod
	{	
		// ϵͳ�ṹ
		struct TQGNSSPRE_MixedSys
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

			TQGNSSPRE_MixedSys()
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

		struct TQGNSSObsPreprocDefine
		{
			double       max_ionosphere;           // ������ӳ�������ֵ������α��Ұֵ�޳�
	        double       min_ionosphere;           // ������ӳ������Сֵ
			double       min_elevation;            // ��͹۲�����,��
			unsigned int min_arcpointcount;        // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       max_arclengh;             // �����������μ��������ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������(��)
			double       vondrak_PIF_eps;          // vondrak ��ϲ���
			double       vondrak_PIF_max;          // vondrak ��ϲ���
			double       vondrak_PIF_min;          // vondrak ��ϲ���
			unsigned int vondrak_PIF_width;        // vondrak ��ϲ���
			double       threshold_gap;            // �����ڵ��жϼ����ֵ, ���Ʋ�Ҫ���ֽϴ���ж�(��)
			double       max_pdop;                 // ���ξ���������ֵ, ������ֵ�Ĺ۲�㽫����������
			int			 threshold_gpssatcount;
			double       threshold_editrms_code;   // α��в���ֵ
			double       threshold_editrms_phase;  // ��λ�в���ֵ
			bool         bOn_RaimSPP;              // RAIM���-����
			bool         bOn_RaimArcChannelBias;   // RAIM���-����
			bool         bOn_GNSSSAT_AntPCOPCV;    // ����������λ����
			bool         bOn_PhaseWindUp;          // ��λ����  
			bool         bOn_GNSSSAT_Clock;
			bool	     bOn_GNSSSAT_Relativity;
            bool         bOn_ClockEliminate;
	 
			TQGNSSObsPreprocDefine()
			{
				max_ionosphere				= 30.0 + 5;
		        min_ionosphere				= 0.0 - 5;
				min_elevation				= 5.0;
				min_arcpointcount			= 10;
				max_arclengh				= 2000.0;
				vondrak_PIF_eps				= 1.0E-10;
				vondrak_PIF_max				= 1.5;
				vondrak_PIF_min				= 0.3;
				vondrak_PIF_width			= 30;
				threshold_gap				= 300;
				max_pdop					= 8.0;
				threshold_gpssatcount		= 3;      
				threshold_editrms_code		= 2.50;
			    threshold_editrms_phase		= 0.10;
				bOn_RaimSPP					= true;
				bOn_RaimArcChannelBias      = true;
				bOn_GNSSSAT_AntPCOPCV       = true;
				bOn_PhaseWindUp				= true;	
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

		class TQGNSSObsPreproc
		{
		public:
			TQGNSSObsPreproc(void);
		public:
			~TQGNSSObsPreproc(void);
		public:
			void    setPreprocPath(string strPreprocPath);
			void    setSP3File(SP3File sp3File); 
			void    setCLKFile(CLKFile clkFile); 
			void    setObsFile(Rinex2_1_ObsFile obsFile);
			bool    loadSP3File(string  strSp3FileName);
			bool    loadCLKFile(string  strCLKFileName);
			bool    loadCLKFile_rinex304(string  strCLKFileName);
			bool    loadMixedObsFile(string  strObsFileName);
			bool    loadMixedObsFile_5Obs(string  strObsFileName);
			void    setHeoOrbitList(vector<TimePosVel> heoOrbitList);
			bool    getHeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange = 8);

			// �༭����+��λ���������������޸�
			bool    mainFuncHeoGNSSObsEdit(Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile);	// ��ʱ��Ҫ����Ϊת��ΪEDT�ļ�
			bool    mixedObsSPP(Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold = 1.0E-003);
	        // mixedObsSPP �ص㣬��λ��PDOP
		private:
			bool    getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist); 
			bool    getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist); // +
			bool    getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
			bool    datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		    bool    datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist);
			vector<Rinex2_1_LeoEditedObsEpoch>  m_editedObsEpochList;
			string  m_strPreprocPath;
		public:
			TQGNSSObsPreprocDefine		m_PreprocessorDefine;
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
			vector<TQGNSSPRE_MixedSys> m_preMixedSysList;
			vector<spp_results>  SPP_resultsList;
		};
	}
}