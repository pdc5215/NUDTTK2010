#pragma once
#include "Rinex3_03_EditedObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "Matrix.hpp"
#include "SP3File.hpp"
#include "svnavMixedFile.hpp"
#include "SinexBiasDCBFile.hpp"

using namespace NUDTTK::Math;

namespace NUDTTK
{
	namespace GPSPreproc
	{
		enum TYPE_FREQ_ID
		{
			TYPE_FREQ_TWO   = 0, // ˫Ƶģʽ
			TYPE_FREQ_THREE = 1
		};
		
		// Ƶ��1��2�ڴ���ʱ����, Ƶ��3�ڽ�����Ƶ����ʱ��ʹ��
		struct PRE_Freq
		{
			double freq_L1;
			double freq_L2;
			double freq_L3;
		};

		// ϵͳ�۲����Ͷ�Ӧ�����
		struct SysObsTypeNum                                               
		{
			char                        cSatSys;                            // ����ϵͳ
			vector<size_t>              obsTypeNumList;                            // �۲��������
		};
		struct PRE_MixedSys
		{
			// ��Ҫ��ʼ������
			char                  cSys;             // ϵͳ��ʶ
			TYPE_FREQ_ID          type_Freq;             // ˫Ƶ/��Ƶ����̽��Ŀ���
			string                name_C1;               // α��
			string                name_C2;
			string                name_C3;
			string                name_L1;              // ��λ
			string                name_L2;
			string                name_L3;
		    string                name_S1;             // ����� 
			string                name_S2;
			string                name_S3;
			string                nameFreq_L1;         // Ƶ������"G01"����Ҫ����PCV����
			string                nameFreq_L2;   
			string                nameFreq_L3;
			PRE_Freq              freqSys;             // GLONASS��Ƶ����Ϣ�洢��freqSatList��, �������Զ���ֵ
			string                signalPriority_L1;   // L1Ƶ�ʶ�Ӧ�����ȼ�
			string                signalPriority_L2;
			string                signalPriority_L3;

			// ��ϵͳ�йص�Ԥ���岿��
			double                min_Elevation;            // �۲�������ֵ, ���ǵ���min_Elevation�Ĺ۲����ݲ�ʹ��
			double			      max_Ionosphere;           // α�����������ֵ�����ֵ��
			double			      min_Ionosphere;           // α�����������ֵ����Сֵ��
			bool                  on_L1_L2_SlipDetect;      // �Ƿ�ʹ�� L1-L2 ��Ԫ��̽���������ϱ���2016/2/24
            bool                  on_IF_SlipDetect;         // �Ƿ�ʹ�� A_IF ���̽���������ϱ�,2016/3/3
			double			      max_L1_L2_Difference;		// L1-L2��Ԫ����ֵ, С�ڴ���ֵ�ĵ㰴��ֵ��С����, ��Ϊ��ѡ�޳���, �� ��,  2016/3/17
			double			      max_L1_L2_SlipSize;	    // L1-L2̽��������ֵ(��λ����)
			double			      max_MW_SlipSize;	    	// MW�������̽����ֵ(��λ����)
			double                max_IF_SlipSize;		    // A_IF�������̽����ֵ(��λ����)
			double                width_L1_L2_SubSecFit;    // L1-L2���зֶ���ϵ�������ڿ��(��λ����)
			BYTE                  order_L1_L2_SubSecFit;	// L1-L2���зֶ���ϵĶ���ʽ����������ʽ���� + 1��
			double			      exten_L1_L2_SubSecFit;    // L1_L2���зֶ���ϵ����Ҷ˵�������չ���(��λ����)
			BYTE				  count_GrossPoint_PreDel;	// Ԥ���޳��ĳ�������
			bool                 bOnTripleFreqsCSD;			// ��Ƶ����̽�⿪��
			double       max_thrfrecodecom;        // ��Ƶα��������ֵ  
			double       threshold_slipsize_PLGIF; // ��Ƶα����λ�޼��ξ��롢�޵�����������̽����ֵ
			double       threshold_slipsize_LGIF;  // ��Ƶ��λ�޼��ξ��롢�޵�����������̽����ֵ
			double       threshold_slipsize_LGF;   // ��Ƶ��λ�޼��ξ����������̽����ֵ
			double       threshold_LGIF_slipcheck; // ��Ƶ��λGIF��������龯�����ֵ����Ԫ�����һֵ���򲻽��������龯���
			double       threshold_LGF_slipcheck;  // ��Ƶ��λGF��������龯�����ֵ����Ԫ�����һֵ���򲻽��������龯���
			double       threshold_gap;            // �����ڵ��жϼ����ֵ, ���Ʋ�Ҫ���ֽϴ���ж�(��)
			double       max_arclengh;             // �����������μ��������ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������(��)

			// ����Ҫ��ʼ������
			int                   index_C1; // Ƶ��1��2�ڴ���ʱ����, Ƶ��3�ڽ�����Ƶ����ʱ��ʹ��
			int                   index_C2;
			int                   index_C3;
			int                   index_L1;
			int                   index_L2;
			int                   index_L3;
			int                   index_S1;
			int                   index_S2;
			int                   index_S3;
			map<string, PRE_Freq> freqSatList; // ��Ҫ���GLONASS��string����Ϊ������

			PRE_MixedSys()
			{
				type_Freq = TYPE_FREQ_TWO;
				index_C1 = -1;
				index_C2 = -1;
				index_C3 = -1;
				index_L1 = -1;
				index_L2 = -1;
				index_L3 = -1;
				index_S1 = -1;
				index_S2 = -1;
				index_S3 = -1;
				name_C1  = "";
				name_C2  = "";
				name_C3  = "";
				name_L1  = "";
				name_L2  = "";
				name_L3  = "";
				name_S1  = "";
				name_S2  = "";
				name_S3  = "";
				nameFreq_L1 = "";
				nameFreq_L2 = "";
				nameFreq_L3 = "";
				signalPriority_L1 = "";
				signalPriority_L2 = "";
				signalPriority_L3 = "";

				on_L1_L2_SlipDetect     = true;
				on_IF_SlipDetect        = true; // Ĭ��ʹ�� A_IF ���̽�� N1,N2 �ȴ�С������
				bOnTripleFreqsCSD       = false; // ��Ƶ����̽�⿪�أ�Ĭ��Ϊ �� 
				min_Elevation           = 7;	// ��λ���ȣ�2016/3/3, �ϱ��޸�, (GAMIT����д���ֵȡΪ 10��2013/05/27)
				max_L1_L2_Difference    = 0.03; // ��λ���ף�Ĭ��ȡ 3 cm
				max_L1_L2_SlipSize      = 0.05; // ��λ���ף���֤������Ƶ��Ĳ�����С��L1����19cm��L2����24cm.��ȡ15cmʱ,BRFT��PRN1��PRN6����©����2013/6/27��
				max_MW_SlipSize	        = 5;	// ��λ���ܣ�����ֵ��С�ڵ�����ʱ���ײ����龯, ����������sigma=60cm, MW�������Ϊ 0.7*sigma/86cm = 0.5 ��
				max_IF_SlipSize         = 50;	// ��λ���ף�N1,N2ͬʱ����1�ܵ�����A_IF�����Ծ10.7cm(խ����ϲ���c/(f1+f2))������P_IF��Ͼ���Ϊ1m��̽����ֵ���˹�С��ȡ10m��Լ100�ܣ�
				width_L1_L2_SubSecFit   = 600;  // ��λ����
				order_L1_L2_SubSecFit   = 3;	// ����ʽ������Ĭ�ϲ���2�ζ���ʽ���
				exten_L1_L2_SubSecFit   = 0;	// ��λ����
				count_GrossPoint_PreDel = 3;	// ���ǰ�޳������ĸ�����Ĭ��3��
				max_Ionosphere		    =  200; // ��alpha - 1) * I����Щ���ǵ�����ӳٽϴ� 60m+ (2016/03/30)
				min_Ionosphere		    = -200; // ��alpha - 1) * I��һ�������ӳٸ�ֵ��������ֵ��С, ��YEBE��վ2012/1/1������ʾ,�������ϸ�ֵ�ϴ�(DCB����)
				max_thrfrecodecom        = 1.0e5;  // XIA1վ����Ƶα�����ֵ�ﵽ��1e4����������վ��С��200
				threshold_slipsize_PLGIF = 3.0;   
				threshold_slipsize_LGIF  = 0.03; 
				threshold_slipsize_LGF   = 0.20; 
				threshold_LGIF_slipcheck = 0.05;   //��Ԫ�����ֵС�ڸ���ֵ��������жϳ��������Ƿ�Ϊ�龯
				threshold_LGF_slipcheck  = 0.5;	   //��Ԫ�����ֵС�ڸ���ֵ��������жϳ��������Ƿ�Ϊ�龯	
				threshold_gap            =  600;
				max_arclengh             =  3600;
			}

			void InitSignalPriority();
		};

		struct PRE_DEF
		{
			double		                 obsEpochInterval;    // ���ݲ������, ���ڴ����Ƶ��������,�ϱ�, 2015/11/27
			size_t                       min_ArcPointCount;   // ��С���������, ����С�� min_ArcPointCount �Ļ��ν���ɾ��
			double                       max_ArcInterval;     // ��Ԫ�����ֵ, ��������ֵ��Ϊ���»���(��λ����)
			double			             vondrak_PIF_eps;     // vondrak�˲�����:�⻬����
			double                       vondrak_PIF_max;     // vondrak�˲�����
			double			             vondrak_PIF_min;     // vondrak�˲�����
			int 			             vondrak_PIF_width;   // vondrak�˲�����
			bool                         on_OutputTempFile;

			PRE_DEF()
			{
				// ��ϵͳ�޹ص�Ԥ���岿��
				obsEpochInterval  = 30.0;	// Ĭ��Rinex�ļ��������Ϊ30s
				min_ArcPointCount = 20;
				max_ArcInterval	  = 180;    // 2016/3/3���ϱ��޸�Ϊ3���ӣ�Bernese 5.0 �� arc �� arc �ļ������Ϊ 180 �룻��ǰ����1800s, ʵ�����ݲ��Ե�ǰ��ֵ�ɿ�(2013-05-21)
				vondrak_PIF_eps	  = 1.0E-14;// �˲���ԽС�������Խ�⻬����ȡֵ���ܹ�С�����򷽳̻Ს̬, �� ��, 2016/3/3
				vondrak_PIF_max	  = 1.5;    // ����......ͨ��
				vondrak_PIF_min	  = 0.5;    // ����......ͨ��
				vondrak_PIF_width = 100;    // ����......ͨ��
				on_OutputTempFile = false;
			}
		};

		class GNSSObsPreproc
		{
		public:
			GNSSObsPreproc(void);
		public:
			~GNSSObsPreproc(void);
		private:
			bool RobustPolyFit_L1_L2(PRE_MixedSys &preMixedSys, double x[], double y[], int n, int offset, int n_out, double y_fit[], double N0, int m = 3); // ��Ҫʹ�ô˺�����2016/2/23��supice
		public:
			BYTE obsPreprocInfo2EditedMark1(int obsPreprocInfo);
			BYTE obsPreprocInfo2EditedMark2(int obsPreprocInfo);
			bool getEditedObsEpochList(vector<Rinex3_03_EditedObsEpoch> &editedObsEpochList);
			bool getEditedObsSatList(vector<Rinex3_03_EditedObsSat> &editedObsSatList);
			bool getEditedObsFile(Rinex3_03_EditedObsFile &editedObsFile);
			bool datalist_epoch2sat(vector<Rinex3_03_EditedObsEpoch> editedObsEpochList, vector<Rinex3_03_EditedObsSat> &editedObsSatList);
			bool datalist_sat2epoch(vector<Rinex3_03_EditedObsSat> editedObsSatList, vector<Rinex3_03_EditedObsEpoch> &editedObsEpochList);
			bool detectCodeOutlier_ionosphere(PRE_MixedSys &preMixedSys, DayTime t0, Rinex3_03_EditedObsSat& editedObsSat);
			bool detectPhaseSlip_MW(PRE_MixedSys &preMixedSys, DayTime t0, Rinex3_03_EditedObsSat& editedObsSat);
			bool detectPhaseSlip_suice(PRE_MixedSys &preMixedSys, DayTime t0, Rinex3_03_EditedObsSat& editedObsSat);
			// ��Ƶ���ݴ���
			bool detectThrFreCodeOutlier(PRE_MixedSys &preMixedSys, DayTime T0, Rinex3_03_EditedObsSat& editedObsSat,bool bOutTempFile = false);	
			bool detectThrFrePhaseSlip(PRE_MixedSys &preMixedSys, DayTime T0, Rinex3_03_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			// ������
			bool mainPRE_MixedGNSS(Rinex3_03_EditedObsFile &editedObsFile);
		public:
			PRE_DEF              m_preDefine;
			vector<PRE_MixedSys> m_preMixedSysList;
			SP3File				 m_sp3File;			 // GNSS���ǹ��
			POS3D				 m_posStation;		 // ��վλ������ 
			Rinex3_03_ObsFile	 m_obsFile;			 // �۲�����
			svnavMixedFile       m_svnavMixedFile;   // ���ڻ�ȡGLONASS��Ƶ����Ϣ
			SinexBiasDCBFile     m_sinexBiasDCBFile; // ��ϵͳDCB�ļ�
			string              m_pathPreFileFolder;
		};
	}
}
