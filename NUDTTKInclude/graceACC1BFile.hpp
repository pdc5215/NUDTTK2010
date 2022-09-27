#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include <string>
#include <vector>

// GRACE���ǵļ��ٶȼ������а��������Լ��ٶ���Ǽ��ٶȣ���ֵ���ڿ�ѧ�ο�����ϵ�и�����
// ��ѧ�ο�����ϵ->��������ϵ->��������ϵ->��������ϵ���ο�GRACE level 1B Data Product HandBook����
// ���Լ��ٶȵ�λΪm/s^2���Ǽ��ٶȵĵ�λΪrad/s^2��
// �ٷ�������GRACE���Ǽ��ٶȼ����ݵ�ƫ��ͳ߶����Ӳμ�PDF�ĵ���Recommendation for a-priori Bias & Scale Parameters for Level-1B ACC Data
// 2018.12.10 Τ����

namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		#pragma pack(1)     // ʹ���������ṹ������ǿ����������
		struct ACC1BRecord
		{
			int           gps_time;
			char          grace_id;
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
		

		class graceACC1BFile
		{
		public:
			graceACC1BFile(void);
		public:
			~graceACC1BFile(void);
		public:
			bool    open(string strACC1BFileName);
			bool    isEmpty();
			bool    getAcc(GPST t, POS3D &lin_acc, POS3D &ang_acc, int nlagrange = 8);
			bool    exportTimeAccFile(string strTimeAccFileName);
		public:
			vector<ACC1BRecord> m_data;
		};
	}
}