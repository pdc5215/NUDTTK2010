#pragma once
#include "Rinex2_1_ObsFile.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#include "Rinex2_1_NavFile.hpp"
#include "Rinex3_0_NavFile.hpp"
#include "MathAlgorithm.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "TimeCoordConvert.hpp"
#include "Troposphere_model.hpp"
#include <direct.h>


namespace NUDTTK
{
	namespace BDPreproc
	{
		struct BDObsPreprocDefine
		{
			double       max_ionosphere;           // ������ӳ�������ֵ������α��Ұֵ�޳�
	        double       min_ionosphere;           // ������ӳ������Сֵ
			double       min_elevation;            // ��͹۲�����,��
			unsigned int min_arcpointcount;        // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       max_arclengh;             // �����������μ��������ֵ, ѡ������������������ٻ��ε�ʱ��, һ�㲻�ᳬ������������(��)
			double       threshold_slipsize_mw;    // melbourne wuebbena �������������̽����ֵ(��)
			double       threshold_rms_mw;         // melbourne wuebbena ������ľ�������ֵ, �Կ�������Ϊ��λ
			double       vondrak_PIF_eps;          // vondrak ��ϲ���
			double       vondrak_PIF_max;          // vondrak ��ϲ���
			double       vondrak_PIF_min;          // vondrak ��ϲ���
			unsigned int vondrak_PIF_width;        // vondrak ��ϲ���
			double       interval;                 // ���ݲ������
			double       threshold_gap;            // �����ڵ��жϼ����ֵ, ���Ʋ�Ҫ���ֽϴ���ж�(��)
			double       threshold_gap_L1_L2;      // L1-L2�ж����������Ʋ�Ҫ���ֽϴ���ж�(��)���ж�̫�󣬵����Ӱ���ϴ�
			double       threshold_slipsize_L1_L2; // L1-L2�����̽��������ֵ(m)
			double       threshold_outliersize_L1_L2; // L1-L2�����̽��Ұֵ��ֵ(m)
			double       max_thrfrecodecom;        // ��Ƶα��������ֵ  
			double       threshold_slipsize_PLGIF; // ��Ƶα����λ�޼��ξ��롢�޵�����������̽����ֵ
			double       threshold_slipsize_LGIF;  // ��Ƶ��λ�޼��ξ��롢�޵�����������̽����ֵ
			double       threshold_slipsize_LGF;   // ��Ƶ��λ�޼��ξ����������̽����ֵ
			double       threshold_LGIF_slipcheck; // ��Ƶ��λGIF��������龯�����ֵ����Ԫ�����һֵ���򲻽��������龯���
			double       threshold_LGF_slipcheck;  // ��Ƶ��λGF��������龯�����ֵ����Ԫ�����һֵ���򲻽��������龯���		

	        bool         bOn_IonosphereFree;       // �Ƿ�������������̽������(m)
			double       threshold_recClk;     // ������ջ��Ӳ���Ƿ��������ֵ(m)

			int          order_L1_L2;				// ���L1-L2�Ķ���ʽ����
			double       nwidth_L1_L2;				// L1-L2��ϵ�������ڿ��(��λ����)
			double	     extendwidth_L1_L2;			// L1_L2��ϲ���(��λ����)

			BDObsPreprocDefine()
			{
				max_ionosphere           =  100;
		        min_ionosphere           = -20;
				min_elevation            =  10;
				min_arcpointcount        =  20;
				max_arclengh             =  3600;
				threshold_slipsize_mw    =  4.0;   //������ֵ����̫С�����ȡ2��̽����ܶ��龯
				threshold_rms_mw         =  1.0;
				vondrak_PIF_eps          =  1.0E-18;
				vondrak_PIF_max          =  1.5;
				vondrak_PIF_min          =  0.5;
				vondrak_PIF_width        =  400;
				interval                 =  30;  // Ĭ�����ݲ������Ϊ30s 
				threshold_gap            =  600;
				threshold_gap_L1_L2      =  60;
				threshold_slipsize_L1_L2 =  0.15;  // ����̽������Ϊ1�ܣ�
				threshold_outliersize_L1_L2 = 0.08; 
				max_thrfrecodecom        = 1.0e5;  // XIA1վ����Ƶα�����ֵ�ﵽ��1e4����������վ��С��200
				threshold_slipsize_PLGIF = 3.0;   
				threshold_slipsize_LGIF  = 0.03; 
				threshold_slipsize_LGF   = 0.20; 
				threshold_LGIF_slipcheck = 0.05;   //��Ԫ�����ֵС�ڸ���ֵ��������жϳ��������Ƿ�Ϊ�龯
				threshold_LGF_slipcheck  = 0.5;	   //��Ԫ�����ֵС�ڸ���ֵ��������жϳ��������Ƿ�Ϊ�龯	


				bOn_IonosphereFree       =  false;
				threshold_recClk         = 100;

				order_L1_L2				  =         4;	// ����ʽ����
				nwidth_L1_L2              =       600;  // ��λ����
				extendwidth_L1_L2         =       120;	// ��λ����
			}
		};		
		class BDObsPreproc
		{
		public:
			BDObsPreproc(void);
		public:
			~BDObsPreproc(void);
		public:
			void    setObsFile(Rinex2_1_ObsFile obsFile);
			bool    loadObsFile(string  strObsfileName);
			bool    loadNavFile(string  strNavfileName);
			bool    loadSp3File(string  strSp3fileName);
			bool    loadClkFile(string  strClkfileName);
			void    setStationPosition(POS3D pos);			
			BYTE    obsPreprocInfo2EditedMark1(int obsPreprocInfo);
			BYTE    obsPreprocInfo2EditedMark2(int obsPreprocInfo);

			// ˫Ƶ�۲����ݴ������ڰ汾(������)
			bool    detectCodeOutlier_ionosphere(int index_P1, int index_P2, double frequence_L1, double frequence_L2, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile = false);// ������
			bool    detectPhaseSlip(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile = false);			
			bool    mainFuncObsPreproc(Rinex2_1_EditedObsFile &editedObsFile,bool bOutTempFile = false); 
			
			// ˫Ƶ�۲����ݴ���GPS�������ݴ���汾(�ϱ�)
			bool    detectCodeOutlier_ionosphere_GPS(int index_P1, int index_P2, double frequence_P1, double frequence_P2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);			
			bool    detectPhaseSlip_GPS(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile = false);
			bool    mainFuncDualFreObsPreproc_GPS(Rinex2_1_EditedObsFile &editedObsFile, bool bOutTempFile = false);
			bool    RobustPolyFitL1_L2(double x[], double y[], double y_fit[], int n, int offset, int n_out, double N0, int m = 3);

            
			//  ��Ƶ����Ԥ����(������)
			bool    detectThrFreCodeOutlier(int index_P1, int index_P2,int index_P5, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile = false);			 
			bool    detectThrFrePhaseSlip(int index_L1, int index_L2,int index_L5, int index_P1, int index_P2, int index_P5, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile = false);	 
			bool    mainFuncThrFreObsPreproc(Rinex2_1_EditedObsFile &editedObsFile,bool bOutTempFile = false); 
			
			bool    receiverClkEst(Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &editedObsFile_clk);
			static bool    desampling_unsmoothed(POS3D posStaion,Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &desampleFile,bool btroCor = false,int nSampleSpan = 120, int Freq1 = 1, int Freq2 = 2);
			static bool    desampling_unsmoothed_GPS(POS3D posStaion, Rinex2_1_EditedObsFile &editedObsFile, Rinex2_1_EditedObsFile &desampleFile,bool btroCor = false,int nSampleSpan = 120);
			static bool    exportSP3File_GPST(string strnavFilePath,GPST t0, GPST t1,double interval = 5 * 60);
			static bool    exportCLKFile_GPST(string strnavFilePath,GPST t0, GPST t1,double interval = 5 * 60);
			//static bool    desampling_unsmoothed(Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &desampleFile,int nSampleSpan = 120);
			bool    mainFuncObsEdit(Rinex2_1_EditedObsFile &editedObsFile);    // ��ʱ����
		public:
			BDObsPreprocDefine            m_PreprocessorDefine;		     
			string                        m_strPreprocFilePath;// �������Ԥ�������������������Ϣ
			vector<TYPE_OBSPREPROC_INFO>  m_obsPreprocInfoList; // Ԥ��������Ϣ
			//Rinex2_1_ObsFile              m_obsFile;         // �۲�����
		private:
			bool getEditedObsEpochList(vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist); 
			bool getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
			bool datalist_epoch2sat(vector<Rinex2_1_EditedObsEpoch> &editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		    bool datalist_sat2epoch(vector<Rinex2_1_EditedObsSat> &editedObsSatlist, vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist);			
		private:
			// ��������
			Rinex2_1_ObsFile              m_obsFile;         // �۲�����
			Rinex2_1_NavFile              m_navFile;         // �㲥�����ļ�
			Rinex3_0_NavFile              m_navFile_3_0;     // �㲥�����ļ�
			POS3D                         m_posStation;      // ��վλ��
			SP3File                       m_sp3File;         // ���ǹ�������༭ʹ��
			CLKFile                       m_clkFile;         // ���������Ӳ�����
		};
	}
}