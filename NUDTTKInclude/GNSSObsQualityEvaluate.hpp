#pragma once
#include "structDef.hpp"
#include "constDef.hpp"
#include "MathAlgorithm.hpp"
#include "Rinex3_03_EditedObsFile.hpp"
#include "GNSSObsPreproc.hpp"
#include <direct.h>
using namespace NUDTTK;
using namespace NUDTTK::Math;

namespace NUDTTK
{
	namespace GPSPreproc
	{
		struct QE_Res
		{
			DayTime  t;
			float    Elevation; 
			float    Azimuth;    
			float    res;
			float    SNR;      // �����
			int      id_Epoch; // ��ǰ���ǵ�����λ�������б��е�λ�ã� 2013/9/26			
			
			QE_Res()
			{
				Azimuth   = 0.0f;
				Elevation = 0.0f;
				res       = 0.0f;
				SNR       = 0.0f;
				id_Epoch  = 0;				
			}
		};

		struct QE_Arc
		{
			vector<QE_Res> resList;
			float          rms;
			int            countEpochs;   // ��Ч��Ԫ����
			int            countSlips;    // ��������

			QE_Arc()
			{
				rms = 0.0f;
				countEpochs = 0;
				countSlips = 0;
			}
		};

		struct QE_Sat
		{
			string                 satName;
			vector<QE_Arc>         arcList;
			float                  rms;
			int                    countEpochs;   // ��Ч��Ԫ����
			int                    countSlips;    // ��������

			QE_Sat()
			{
				rms = 0.0f;
				countEpochs = 0;
				countSlips = 0;
			}
		};

		struct QE_SlipInfo                                  
		{
			string                 satName;
			DayTime                t;       // ����ʱ��
			int                    preInfo; // ����̽��Ԥ������Ϣ		
		};

		struct QE_Elevation
		{
			float                  e0;         // ��ʼ����
			float                  e1;         // ��ֹ����
			int                    count_P1;   // ͳ��α��ྶʱʹ�õ�α����Ԫ����
			int                    count_P2;   // ͳ��α��ྶʱʹ�õ�α����Ԫ����
			int                    count_P3;   // ͳ��α��ྶʱʹ�õ�α����Ԫ����
			int                    count_L;    // ͳ����λ����ʱʹ�õ���λ��Ԫ����
			float                  rms_P1;   
			float                  rms_P2;
			float                  rms_P3;
			float                  rms_L;
			float                  meanS1;     // ��һ��Ƶ��ƽ�������
			float                  meanS2;     // �ڶ���Ƶ��ƽ�������
			float                  meanS3;     // ������Ƶ��ƽ�������
			int                    countSlips; // ��������

			QE_Elevation()
			{
				count_P1   = 0;
				count_P2   = 0;
				count_P3   = 0;
				count_L    = 0;
				rms_P1     = 0.0f;
				rms_P2     = 0.0f;
				rms_P3     = 0.0f;
				rms_L      = 0.0f;
				meanS1     = 0.0f;  
				meanS2     = 0.0f; 
				meanS3     = 0.0f; 
				countSlips = 0;
			}
		};

		struct QE_MixedSys
		{
			// ��Ҫ��ʼ������
			char                  cSys;        // ϵͳ��ʶ
			TYPE_FREQ_ID          type_Freq;   // ˫Ƶ/��Ƶ����̽��Ŀ���
			string                name_C1;     // α��
			string                name_C2;
			string                name_C3;
			string                name_L1;     // ��λ
			string                name_L2;
			string                name_L3;
		    string                name_S1;     // ����� 
			string                name_S2;
			string                name_S3;
			string                nameFreq_L1;         // Ƶ������"G01"����Ҫ����PCV����
			string                nameFreq_L2;   
			string                nameFreq_L3;
			PRE_Freq              freqSys;     // GLONASS��Ƶ����Ϣ�洢��freqSatList��, �������Զ���ֵ
			double			      vondrak_LIF_eps;     // vondrak�˲�����:�⻬����
			double                vondrak_LIF_max;     // vondrak�˲�����
			double			      vondrak_LIF_min;     // vondrak�˲�����
			int 			      vondrak_LIF_width;   // vondrak�˲�����

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

			// �������ɲ���
			map<string, QE_Sat>       satInfoList_P1;
			map<string, QE_Sat>       satInfoList_P2;
			map<string, QE_Sat>       satInfoList_P3;
			map<string, QE_Sat>       satInfoList_L;
			map<string, QE_SlipInfo>  satInfoList_Slip;  
			map<BYTE, int>            satInfoList_Abnormal; // �쳣��Ϣ�б�
			vector<QE_Elevation>      satInfoList_Elevation; 
			int                   countEpochs;   // ��Ԫ����
			int                   countSlips;
			int                   count_P1;   
			int                   count_P2;   
			int                   count_P3;   
			int                   count_L; 
			double                ratio_P_normal;			 // α��������������
			double                ratio_L_normal;			 // ��λ������������
			double                ratio_cycleslip;		 // ��������
			double                thrfre_coefficient;   // ��Ƶ���ϵ�� gama
			float                 meanCount_Sat; // ƽ������������
			float                 rms_P1;
			float                 rms_P2;
			float                 rms_P3;
			float                 rms_L;

			void InitSatInfoList_Elevation(float Interval = 5.0f);
			int  getIndex_Elevation(float Elevation);

			QE_MixedSys()
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
				vondrak_LIF_eps	  = 1.0E-10; 
				vondrak_LIF_max	  = 0.5;    
				vondrak_LIF_min	  = 0.05;    
				vondrak_LIF_width = 100;  
				thrfre_coefficient = 1.0;
			}
		};

		class GNSSObsQualityEvaluate
		{
		public:
			GNSSObsQualityEvaluate(void);
		public:
			~GNSSObsQualityEvaluate(void);
		private:
			string  m_rootPathQE;
			string  m_staName;
			vector<Rinex3_03_EditedObsSat> m_editedObsSatList;
		public:	
			void setRootPath(string strRootPath, string staName = "");
			bool mainQE_MixedGNSS(string strEditedObsFilePath);
			bool getQEInfo(QE_MixedSys &qe_MixedSys);
		private:
			bool QE_C_Multipath(QE_MixedSys &qe_MixedSys);
			bool QE_L_poly(QE_MixedSys &qe_MixedSys, double poly_halfwidth, int poly_order);
			bool QE_L_Vondrak(QE_MixedSys &qe_MixedSys);
			bool QE_C_L_thrfre(QE_MixedSys &qe_MixedSys);
		public:	
			vector<QE_MixedSys>   m_qeMixedSysList;
			svnavMixedFile       m_svnavMixedFile;   // ���ڻ�ȡGLONASS��Ƶ����Ϣ
		};
	}
}
