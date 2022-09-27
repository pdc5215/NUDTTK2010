#pragma once
#include "structDef.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#pragma warning(disable: 4996)

namespace NUDTTK
{
	namespace BDPreproc
	{
		struct QERESIDUAL
		{
			DayTime  t;
			double   Azimuth;    
			double   Elevation; 
			double   res;
			//double   CN;                 // �����
			int      id_elevation_Inter; // ��ǰ���ǵ�����λ�������б��е�λ�ã� 2013/9/26			
			QERESIDUAL()
			{
				Azimuth   = 0;
				Elevation = 0;
				res       = 0;
				//CN        = 0;
				id_elevation_Inter = 0;				
			}
		};

		struct QERESIDUAL_ARC
		{
			vector<QERESIDUAL> resList;
			double rms_arc;
			QERESIDUAL_ARC()
			{
				rms_arc = 0;
			}
		};

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

		typedef map<BYTE,    int> QEAbNormalObsCountMap; // �쳣��Ϣ�б�

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

		struct QERESIDUAL_STATION
		{
			vector<QERESIDUAL_SAT> satInfoList_P1;
			vector<QERESIDUAL_SAT> satInfoList_P2;
			vector<QERESIDUAL_SAT> satInfoList_P5;
			vector<QERESIDUAL_SAT> satInfoList_L;
			vector<Q_ELE_STATION>  satInfolist_ele;        // 2013/9/26
			vector<CycleSlip_Info> satInforlist_CycleSlip; // 2013/9/26
			int                    total_Epochs;           // �ܵ���Ԫ����
			double                 mean_VisibleSat;        // ƽ������������
			double                 rms_P1;
			double                 rms_P2;
			double                 rms_P5;
			double                 rms_L;
			double                 ratio_P_normal;
			double                 ratio_L_normal;
			double                 ratio_SLip;
			QEAbNormalObsCountMap  AbnormalObsCount;
			void init(double elevation_Inter = 5.0);  // Ĭ�ϵ����Ǽ��Ϊ5��,2013/9/26
			int  getInterval(double elevation);       // ȷ�������������б��е�λ��,2013/9/26
			QERESIDUAL_STATION()
			{
				total_Epochs    = 0;
				mean_VisibleSat = 0;
				rms_P1          = 0;
				rms_P2          = 0;
				rms_P5          = 0;
				rms_L           = 0;
				ratio_P_normal  = 0;
				ratio_L_normal  = 0;
				ratio_SLip      = 0;
			}
		};			

		// BD�۲���������������
		class BDObsQualityEvaluate
		{
		public:
			BDObsQualityEvaluate(void);
		public:
			~BDObsQualityEvaluate(void);
		public:	
			QERESIDUAL_STATION m_QEInfo;		
		public:		
			bool mainFunc(string  strEdtedObsfilePath);
			bool mainFunc_ThrFreObs(string  strEdtedObsfilePath);
		private:
			bool evaluate_code_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2);
			bool evaluate_phase_vondrak(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_L1, int index_L2, double vondrak_L1_L2_eps = 5.0E-13, double vondrak_L1_L2_max = 1.5,double vondrak_L1_L2_min = 0.5,unsigned int vondrak_L1_L2_width = 300);

			bool evaluate_thrfre_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_P1, int index_P2, int index_P5, int index_L1, int index_L2, int index_L5);
			//bool evaluate_thrfrephase_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_L1, int index_L2, int index_L5);
		    bool statBDSatCount(Rinex2_1_EditedObsFile m_editedObsFile, int &max_count, double &mean_count);
			//bool statpdop(double &mean_pdop);	
		};
	}
}