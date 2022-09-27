#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include <string>
#include <vector>

// GRACE-FO���ǵļ��ٶȼ������а��������Լ��ٶ���Ǽ��ٶȣ���ֵ���ڿ�ѧ�ο�����ϵ�и�����
// ��ѧ�ο�����ϵ->��������ϵ->��������ϵ->��������ϵ���ο�GRACE-FO_L1_Data_Product_User_Handbook����
// ���Լ��ٶȵ�λΪm/s^2���Ǽ��ٶȵĵ�λΪrad/s^2��
// �ٷ�������GRACE���Ǽ��ٶȼ����ݵ�ƫ��ͳ߶����Ӳμ�PDF�ĵ���Recommendation for a-priori Bias & Scale Parameters for Level-1B ACC Data
// 2020.4.8 Τ����

namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		#pragma pack(1)     // ʹ���������ṹ������ǿ����������
		struct ACT1BRecord
		{
			int           gps_time;
			char          gracefo_id;
			double        lin_accl_x;
			double        lin_accl_y;
			double        lin_accl_z;
			double        ang_accl_x;
			double        ang_accl_y;
			double        ang_accl_z;
			double        acl_x_res;   // ??
			double        acl_y_res;
			double        acl_z_res;
			unsigned char qualflg;

			GPST gettime()
			{
				GPST t0(2000, 1, 1, 12, 0, 0.0);
				return t0 + gps_time;
			};
		};
		#pragma pack()
		

		class gracefoACT1BFile
		{
		public:
			gracefoACT1BFile(void);
		public:
			~gracefoACT1BFile(void);
		public:
			bool    isValidEpochLine(string strLine, ACT1BRecord& act1bRecord);
			bool    open(string strACT1BFileName);
			bool    isEmpty();
			bool    getAcc(GPST t, POS3D &lin_acc, POS3D &ang_acc, int nlagrange = 8);
			bool    exportTimeAccFile(string strTimeAccFileName);
		public:
			vector<ACT1BRecord> m_data;
		};
	}
}