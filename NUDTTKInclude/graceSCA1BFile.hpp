#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include <string>
#include <vector>

namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		// Star Camera Assembly
        /* SCA1A ��������������������������ݣ�������������1s�������ʲ������ݺ͵ڶ���������5s��������
           
		   ���������������༭, �ο���Ԫ��ͨ�����ù�����߱�����, �вÿ��ӹ�һ��.
		   Ұֵ����ͨ��һ��ȫ�ֵ�3sigma׼��ɾ��, Ȼ���һ��ͨ��һ���ֲ���3sigma׼���޳�
		   �����Ԫ���Է���ģ������У��

		   ��������������, �����ݵļ��С�ڵ���10s, ��ʱ�м䶪ʧ�����ݲ���2�β�ֵ���, ��ÿ��������2�����������ʱ
           ���ÿ�߲���2����, ��������Բ�ֵ�����
		   ���ݼ������10���ӵĿ�ȱ���������. 
		   �ڶ������������ݿ�ȱ�������
		  
		   ʱ���У��������ݣ��������������ز�����1s���, �ڶ��������������ز�����5s���,�������Բ�ֵ
		   ��������������Ȼ��ѹ����5s���,ͨ����1s�����ݲ���2�ζ���ʽ���

		   �������͵ڶ���������������Ԫ������Ȩ���, �������ݲ���5s������б���
        */
		#pragma pack(1)
		struct SCA1BRecord
		{// 47���ֽ�
			int           gps_time;
			char          grace_id;
            char          sca_id;
			double        quatangle;
			double        quaticoeff;
			double        quatjcoeff;
			double        quatkcoeff;
			double        qual_rss;
			unsigned char qualflg;
			/*
				Quality flags that indicate data gaps filled, according to severity:
					qualflg bit 0=1 filled data at T
					qualflg bit 1=1 filled data at T +/- 1 second
					qualflg bit 2=1 filled data at T +/- 2 seconds
				Additional quality flags include:
					qualflg bit 3=1 only one star camera enabled
					qualflg bit 4=1 extrapolated clock correction used
					qualflg bit 6=1 low rate data from 2nd star camera
					qualflg bit 7=1 low rate data from 1st star camera
			*/

			GPST gettime()
			{
				GPST t0(2000, 1, 1, 12, 0, 0);
				return t0 + gps_time;
			};
		};
		#pragma pack()

		class graceSCA1BFile
		{
		public:
			graceSCA1BFile(void);
		public:
			~graceSCA1BFile(void);
		public:
			bool open(string strSCA1BfileName);
			//bool grace_B_SRF_XYZ();
			bool exportTimeAttitudeFile(string  strTimeAttitudeFileName);
		public:
			vector<SCA1BRecord> m_data;
		};
	}
}
