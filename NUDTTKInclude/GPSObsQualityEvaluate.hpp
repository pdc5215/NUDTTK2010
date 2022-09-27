#pragma once
#include "structDef.hpp"
#include "constDef.hpp"
#include "MathAlgorithm.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#include <direct.h>
using namespace NUDTTK;
using namespace NUDTTK::Math;
// ģ�����ƣ�GPSObsQualityEvaluate
// ģ�鹦�ܣ�ʵ��GPS�۲����ݵ���������
// ����˵����1��α�����������������ྶ���
//           2����λ��������������Vondrak �˲���� L1-L2
//           3������������Ŀ����
// ���ԣ�C++
// �����ߣ��� ��
// ����ʱ�䣺2012/9/12
// �汾ʱ�䣺2012/9/12
// �޸ļ�¼��1��2013/6/14 �����Ż����������������
// ��ע�� 
namespace NUDTTK
{
	namespace GPSPreproc
	{
		// ��Ԫ��Ϣ
		struct QERESIDUAL
		{
			DayTime  t;
			double   Azimuth;    
			double   Elevation; 
			double   obs;
			double   fit;
			double   res;
			int      id_elevation_Inter; // ��ǰ���ǵ�����λ�������б��е�λ�ã� 2013/9/26
			QERESIDUAL()
			{
				Azimuth   = 0;
				Elevation = 0;
				res       = 0;	
				id_elevation_Inter = 0;
			}
		};
		// ������Ϣ
		struct QERESIDUAL_ARC
		{
			vector<QERESIDUAL> resList;
			double rms_arc;
		};
		// ������Ϣ
		struct QERESIDUAL_SAT
		{
			vector<QERESIDUAL_ARC> arcList;
			BYTE                   id_sat;
			double                 rms_sat;
			int                    epochs;   // ��Ч��Ԫ������Ϊ��������ѡ���ṩ����
			int                    slips;    // ��������
			QERESIDUAL_SAT()
			{
				rms_sat = 0;
				epochs  = 0;
				slips   = 0;
			}
		};

		typedef map<BYTE, int>	QEAbNormalObsCountMap;	// �쳣��Ϣ�б�
		struct Q_ELE_STATION
		{// �۲����������ǵĹ�ϵ�� 2013/9/26
			double                 e0;       // ��ʼ����
			double                 e1;       // ��ֹ����
			int                    epochs_P; // ͳ��α��ྶʱʹ�õ�α����Ԫ����
			int                    epochs_L; // ͳ����λ�ྶ(����)ʱʹ�õ���λ��Ԫ����
			double                 rms_P1;   
			double                 rms_P2;
			double                 rms_P5;
			double                 rms_L;
			int                    slips;    // ��������
			double                 CN_L1;    // ��һ��Ƶ�������
			double                 CN_L2;    // �ڶ���Ƶ�������
			double                 CN_L5;    // ������Ƶ�������
			Q_ELE_STATION()
			{
				epochs_P = 0;
				epochs_L = 0;
				rms_P1 = 0;
				rms_P2 = 0;
				rms_P5 = 0;
				rms_L  = 0;
				slips  = 0;
				CN_L1  = 0;  
				CN_L2  = 0; 
				CN_L5  = 0; 
			}
		};
		struct CycleSlip_Info                                  
		{  // ��������ϸ��Ϣ, 2013/9/26
			DayTime                t;                             // ����ʱ��
			BYTE                   preproc_info;                  // ̽��������ķ���
			BYTE                   id_sat;                        // ��������id				
		};

		// ��վ��Ϣ
		struct QERESIDUAL_STATION
		{
			vector<QERESIDUAL_SAT> satInfoList_P1;		// P1 ͳ����Ϣ�б�(QEcode)
			vector<QERESIDUAL_SAT> satInfoList_P2;		// P2 ͳ����Ϣ�б�(QEcode)
			vector<QERESIDUAL_SAT> satInfoList_L;		// L1-L2 ͳ����Ϣ�б�(QEphase)
			vector<Q_ELE_STATION>  satInfolist_ele;        // 2013/9/26
			vector<CycleSlip_Info> satInforlist_CycleSlip; // 2013/9/26
			double		rms_P1;							// P1 ��������(QEcode)
			double      rms_P2;							// P2 ��������(QEcode)
			double      rms_L;							// L1-L2 ��������(QEphase)
			double      ratio_P_normal;					// α��������������(getQEInfo)
			double      ratio_L_normal;					// ��λ������������(getQEInfo)
			double      ratio_cycleslip;				// ��������(getQEInfo)
			int			max_ObsSatNum;				    // ������������(QESatCounts)
			double		mean_ObsSatNum;				    // ƽ������������(QESatCounts)
			QEAbNormalObsCountMap  AbnormalObsCount;	// �쳣��Ϣ�б�(getQEInfo)
			void init(double elevation_Inter = 5.0);    // Ĭ�ϵ����Ǽ��Ϊ5��,2013/9/26
			int  getInterval(double elevation);         // ȷ�������������б��е�λ��,2013/9/26
			QERESIDUAL_STATION()
			{				
				max_ObsSatNum   = 0;
				mean_ObsSatNum  = 0;
				rms_P1          = 0;
				rms_P2          = 0;				
				rms_L           = 0;
				ratio_P_normal  = 0;
				ratio_L_normal  = 0;
				ratio_cycleslip = 0;
			}
		};

		// GPS �۲���������������
		class GPSObsQualityEvaluate
		{
		public:
			GPSObsQualityEvaluate();
		public:
			~GPSObsQualityEvaluate();
		private:
			string  m_strOQEResPath;
			string  m_strStaName;
		public:	
			void setOQEResPath(string strOQEResPath, string strStaName = "");
			bool mainFuncObsQE(string  strEditedObsfilePath, int Freq1, int Freq2); // �� ��������Freq1��Freq2, ���ڱ���ѡ��Ƶ��, 2016/3/24 
			bool getQEInfo(vector<Rinex2_1_EditedObsSat>  editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2);
		private:
			bool QEcode_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1 = GPS_FREQUENCE_L1,double frequence_L2 = GPS_FREQUENCE_L2);
			bool QEphase_vondrak(vector<Rinex2_1_EditedObsSat>  editedObsSatlist, int index_L1, int index_L2, double frequence_L1 = GPS_FREQUENCE_L1,double frequence_L2 = GPS_FREQUENCE_L2, double vondrak_LIF_eps = 1.0E-10, double vondrak_LIF_max = 0.5, double vondrak_LIF_min = 0.05, int vondrak_LIF_width = 100);
			bool QESatCounts(Rinex2_1_EditedObsFile editedObsFile);
		public:
			QERESIDUAL_STATION	m_QEInfo;

		//�� �������о�ʹ��, 2016/3/24
		public:
			bool getMP1MP2(GPST t0);
		public:
			string              m_OQEFilePath; // �洢Ŀ¼, ��getMP1MP2ʹ��
			POS3D               m_staPos;      // �洢��վλ����Ϣ
		};
	}
}