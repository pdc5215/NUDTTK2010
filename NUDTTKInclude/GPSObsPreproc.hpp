#pragma once
#include "Rinex2_1_ObsFile.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#include "Rinex2_1_MixedEditedObsFile.hpp"
#include "Rinex2_1_NavFile.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "structDef.hpp"
#include "TimeCoordConvert.hpp"
#include "Troposphere_model.hpp"
#include <direct.h>
using namespace NUDTTK;
using namespace NUDTTK::Math;
// ģ�����ƣ�GPSObsPreproc
// ģ�鹦�ܣ�ʵ��GPS�۲����ݵ�Ԥ����
// ����˵����1��GPS����Ԥ�������Ұֵ�޳�������̽��
//           2����Ҫ����ʱ�����з�����α�뼰��λ������Ԥ����
//           3���Ա༭�����ļ����н����������������ӳ�����
// ���ԣ�C++
// �����ߣ��� ��
// ����ʱ�䣺2012/9/12
// �汾ʱ�䣺2012/9/12
// �޸ļ�¼��1��������λ����Ԥ������ԣ��ۺ����� M-W ��ϡ�L_IF ��Ϻ� L1-L2 ��ϣ�2013/5/20
//			 2����ӳ�Ա���� strObsFilePath ������ȡ�����ļ�·�������ڴ洢Ԥ��������2013/9/13
// ��ע�� 
namespace NUDTTK
{
	namespace GPSPreproc
	{
		// ����Ԥ���������漰������ֵ�������ṹ��
		struct GPSObsPreprocParam
		{
			double		    interval;                   // ���ݲ�����������ڴ����Ƶ�������ݣ�supice, 2015-11-27
			double			max_ThredIonosphere;		// α�����������ֵ�����ֵ��
			double			min_ThredIonosphere;		// α�����������ֵ����Сֵ��
			unsigned int	min_arcpointcount;			// ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double		    max_arclengh;				// ��Ԫ�����ֵ����������ֵ��Ϊ���»���(��λ����)
			double			min_elevation;				// �۲�������ֵ,���ǵ��� min_elevation �Ĺ۲����ݲ�ʹ��
			double			vondrak_PIF_eps;			// Vondrak�˲��������⻬����
			double          vondrak_PIF_max;			// Vondrak�˲�����
			double			vondrak_PIF_min;			// Vondrak�˲�����
			int 			vondrak_PIF_width;			// Vondrak�˲�����
			double			threshold_slipsize_mw;		// M-W�������̽����ֵ(��λ����)
			double			threshold_maxD_L1_L2;		// L1-L2 ��Ԫ����ֵ��С�ڴ���ֵ�ĵ㰴��ֵ��С���У���Ϊ��ѡ�޳��㣬�� ����2016/3/17
			double			threshold_slipsize_L1_L2;	// L1-L2̽��������ֵ(��λ����)
			double			priori_rms_mw;				// M-W�������RMS(��λ����)
			int             order_L1_L2;				// ���L1-L2�Ķ���ʽ����
			double          nwidth_L1_L2;				// L1-L2���зֶ���ϵ�������ڿ��(��λ����)
			double			extendwidth_L1_L2;			// L1_L2���зֶ�������Ҷϵ�������չ���(��λ����)
			//����Ϊ�ϱ����A_IF��Ͻ�������̽������ӵĳ�Ա����  2017/01/04����
			int				GrossPointNum_PreDelete;	// Ԥ���޳���������
			bool            bOnTripleFreqsCSD;			// ��Ƶ����̽�⿪��
			double          threshold_slipsize_IF;		// A_IF�������̽����ֵ(��λ����)
			bool            bOn_IF;						// �Ƿ�ʹ�� A_IF ���̽��������supice,2016/3/3
			bool            bOn_L1_L2_PhaseSlipDetect;	// �Ƿ�ʹ�� L1-L2 ��Ԫ��̽��������supice��2016/2/24
			
			GPSObsPreprocParam()
			{
				interval                  =      30.0;	// Ĭ�� Rinex �ļ��������Ϊ 30 s
				max_ThredIonosphere		  =		  200;	// ��alpha - 1)*I����Щ���ǵ�����ӳٽϴ�60m+(2016-03-30)
				min_ThredIonosphere		  =		 -200;	// ��alpha - 1)*I��һ�������ӳٸ�ֵ��������ֵ��С,��YEBE��վ2012/1/1������ʾ���������ϸ�ֵ�ϴ�(DCB����)
				min_arcpointcount		  =		   20;  // ��̻���Ҫ��10min��Bernese 5.0 �� arc �����ٵ���Ϊ 10(5min)
				max_arclengh			  =		  180;	// 2016/3/3���ϱ��޸�Ϊ 3min��Bernese 5.0 �� arc �� arc �ļ������Ϊ 180 �룻��ǰ����1800s, ʵ�����ݲ��Ե�ǰ��ֵ�ɿ�(2013-05-21)
				min_elevation             =		    7;	// ��λ���ȣ�2016/3/3, �ϱ��޸ģ�(gamit����д���ֵȡΪ10��2013-05-27)
				vondrak_PIF_eps			  =   1.0E-14;	// �˲���ԽС�������Խ�⻬����ȡֵ���ܹ�С�����򷽳̻Ს̬���� ����2016/3/3
				vondrak_PIF_max			  =		  1.5;	// ����......ͨ��
				vondrak_PIF_min			  =		  0.5;	// ����......ͨ��
				vondrak_PIF_width         =		  100;	// ����......ͨ��
				threshold_slipsize_mw	  =		    5;	// ��λ���ܣ�����ֵ��С�ڵ�����ʱ���ײ����龯, ���������� sigma = 60cm, MW�������Ϊ 0.7*sigma / 86cm = 0.5 ��
				threshold_maxD_L1_L2	  =      0.03;	// ��λ���ף�Ĭ��ȡ 3 cm
				threshold_slipsize_L1_L2  =		 0.05;	// ��λ���ף���֤������Ƶ��Ĳ�����С��L1����19cm��L2����24cm(ȡ 15cm brft_PRN1��PRN6©����2013/6/27)
				priori_rms_mw			  =       0.5;	// ��λ���ܣ�����TurboEdit(1990)�����е�����ѡȡ
				order_L1_L2				  =         3;	// ����ʽ������Ĭ�ϲ���2�ζ���ʽ���
				nwidth_L1_L2              =       600;  // ��λ����
				extendwidth_L1_L2         =         0;	// ��λ����
				
				GrossPointNum_PreDelete   =         3;	// ���ǰ�޳������ĸ�����Ĭ��ȡ3
				bOnTripleFreqsCSD         =     false;
				threshold_slipsize_IF     =        50;	// ��λ���ף�N1,N2ͬʱ����1�ܵ�����A_IF�����Ծ10.7cm(խ����ϲ���c/(f1+f2))������P_IF��Ͼ���Ϊ1m��̽����ֵ���˹�С��ȡ10m��Լ100�ܣ�
				bOn_IF					  =      true;	// Ĭ��ʹ�� A_IF ���̽�� N1,N2 �ȴ�С������
				bOn_L1_L2_PhaseSlipDetect =     false;  // Ĭ�ϲ�ʹ�� L1-L2 ��Ԫ��̽��С������һ����������ʱ�仯����ƽ��(������������)���龯�ʹ��ߣ���һ����ʵ�����ݷ���С����ʱL1-L2��ϲ�������
			}
		};

		// ����GPS�۲�����Ԥ������
		class GPSObsPreproc
		{
		public:
			GPSObsPreproc(void);
			GPSObsPreproc(Rinex2_1_ObsFile obsFile, Rinex2_1_NavFile navFile, POS3D posStation, char cSatSystem = 'G', char RecType = 'N');
			GPSObsPreproc(Rinex2_1_ObsFile obsFile, SP3File sp3File, POS3D posStation, char cSatSystem = 'G', char RecType = 'N');
		public:
			~GPSObsPreproc(void);
		public:
			void setObsFile(Rinex2_1_ObsFile obsFile);
			bool loadObsFile(string strObsFileName);
			bool loadNavFile(string strNavFileName);
			bool loadSp3File(string strSp3FileName);
			void setStationPosition(POS3D pos);
			void getPreprocFilePath(string strObsFileName);
			bool getEditedObsEpochList(vector<Rinex2_1_EditedObsEpoch> &editedObsEpochList);
			bool getEditedObsSatList(vector<Rinex2_1_EditedObsSat> &editedObsSatList);
			BYTE obsPreprocInfo2EditedMark1(int obsPreprocInfo);
			BYTE obsPreprocInfo2EditedMark2(int obsPreprocInfo);

			bool detectCodeOutlier_ionosphere(int index_P1, int index_P2, double frequence_P1, double frequence_P2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			bool detectPhaseSlip_MW(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			bool detectPhaseSlip(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat);
			bool detectPhaseSlip_suice(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			bool detectPhaseSlipTripleFreqs(int index_L1, int index_L2, int index_L5, double frequence_L1, double frequence_L2, double frequence_L5, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			bool mainFuncObsPreproc(Rinex2_1_EditedObsFile &editedObsFile, int Freq1 = 1, int Freq2 = 2, bool bOutTempFile = false);

		private:
			bool RobustPolyFitL1_L2(double x[], double y[], int n, int offset, int n_out, double y_fit[], double N0, int m = 3); // ��Ҫʹ�ô˺�����2016/2/23��supice
		public:
			static bool downSampling(POS3D posStaion, Rinex2_1_EditedObsFile editedObsFile, Rinex2_1_EditedObsFile &downSamplingFile, bool bOnTropCorret = true, int nSampleSpan = 120);
			static bool downSampling_new(POS3D posStaion, Rinex2_1_EditedObsFile editedObsFile, Rinex2_1_EditedObsFile &downSamplingFile, bool bOnTropCorret = true, int nSampleSpan = 120);
			//���������أ��ں�ϵͳ���  ��̍ 20170921
			static bool downSampling(POS3D posStaion, Rinex2_1_MixedEditedObsFile editedObsFile, Rinex2_1_MixedEditedObsFile &downSamplingFile, bool bOnTropCorret = true, int nSampleSpan = 300);
		public:
			bool datalist_epoch2sat(vector<Rinex2_1_EditedObsEpoch> editedObsEpochList, vector<Rinex2_1_EditedObsSat> &editedObsSatList);
			bool datalist_sat2epoch(vector<Rinex2_1_EditedObsSat> editedObsSatList, vector<Rinex2_1_EditedObsEpoch> &editedObsEpochList);	
		public: 
			GPSObsPreprocParam  m_ObsPreprocParam;
			string              m_strPreprocFilePath;
		private:
			Rinex2_1_ObsFile			m_obsFile;			// �۲�����
			Rinex2_1_NavFile			m_navFile;			// �㲥��������
			POS3D						m_posStation;		// ��վλ������ 
			SP3File						m_sp3File;			// ���ǹ��
			char                        m_recType;
			// ���ݱ���ϵͳ���� ����2016/3/3
			char						m_cSatSystem;		
			double                      FREQUENCE_L1;
			double                      FREQUENCE_L2;
			double						FREQUENCE_L5;
		};
	}
}