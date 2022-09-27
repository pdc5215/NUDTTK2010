#pragma once
#include <string>
#include <vector>
#include "structDef.hpp"
#include "Matrix.hpp"
#include <math.h>

// Copyright 2018, SUN YAT-SEN UNIVERSITY TianQin Research Center at ZhuHai
namespace NUDTTK
{
	namespace TQPod
	{
		struct TQUXBObsLine
		{
			UTC    t;                  // ����ʱ��, ����ʱ, �ҹ���վ���ݴ󲿷ֲ��ñ���ʱ
			double R;                  // ������ֵ
			double V;                  // �����ٶ�

			//double ISL_Code; // �Ǽ伤��α����ֵ   ���Դ���
			//double ISL_Code1;// �Ǽ伤��α��۲�ֵ ���Դ���

			//double ISL_Phase; // �Ǽ伤����λ�����ֵ ���Դ���
			//double ISL_Phase1;// �Ǽ伤����λ���۲�ֵ ���Դ���
			
			double R1;                 // ԭʼ���ֵ
			double R0s;                // ���Ͼ�����ֵ
			double R0g;                // ���������ֵ
			double err_Trop;           // ���������
			double err_Iono;           // ��������

			double V1;                 // ԭʼ����ֵ-ƽ������仯��
            UTC    t0;                 // ����ʱ����� +
            UTC    t1;                 // ����ʱ���յ� + 

			double Azimuth;            // �۲ⷽλ��, ����������ϵ
			double Elevation;          // �۲�߶Ƚǣ�����������ϵ

			// ��վ���������� ��� ����
			double time_Doppler;       // �����ջ���ʱ�䳤��
			string staName;            // ��վ����
			string satName;            // ��������

			//string satName1;   // uxb���ߵ������ ���Դ���
			//string satName2;   // satName1-satName2���Ǽ伤���� ���Դ���

			// ����Ԥ������ز���
			POS3D      pos_J2000;      // ��վ��tʱ��J2000�µ�λ������
			TimePosVel tqOrb;          // ����������
			Matrix     tqRtPartial;    // ƫ����
			double     dR_up;          // ���о���
			double     dR_down;        // ���о���

			POS3D      pos_J2000_t0;    // ��վ��t0ʱ��J2000�µ�λ������
			POS3D      pos_J2000_t1;    // ��վ��t1ʱ��J2000�µ�λ������
			TimePosVel tqOrb_t0;        // ����getEphemeris_PathDelay�����ڴ洢�����վ������ƫ���� + 
			TimePosVel tqOrb_t1;        // ����getEphemeris_PathDelay�����ڴ洢�����վ������ƫ���� + 
			Matrix     tqRtPartial_t0;  // ����getEphemeris_PathDelay�����ڴ洢�����վ������ƫ���� + 
			Matrix     tqRtPartial_t1;  // ����getEphemeris_PathDelay�����ڴ洢�����վ������ƫ���� +
			double     dR_trop;         // ����������
			double     dR_satpco;       // ������������ƫ��
			double     dR_GraRelativity;// �������������
			double     dR_SolidTides;   // ���峱����
			double     dR_OceanTides;   // ��������
			double     corrected_value; // ����ֵ, ���ھ�������

			// ���ܶ�����ز���
			double weight_range;        // ���Ȩ��
			double weight_doppler;      // ����Ȩ��
			double oc_range;            // ���в�
			double oc_doppler;          // ���ٲв�
			double rw_range;            // ���³��Ȩֵ
			double rw_doppler;          // ����³��Ȩֵ
			


			TQUXBObsLine()
			{
				R               = 0.0;
				V               = 0.0;

				Elevation       = 0.0;
				Azimuth         = 0.0;   

				time_Doppler    = 0.0;

				R0s             = 0.0;
				R1              = 0.0;
				R0g             = 0.0;
				err_Trop        = 0.0;
				err_Iono        = 0.0;

				oc_range        = 0.0;
				oc_doppler      = 0.0;
				rw_range        = 1.0;
				rw_doppler      = 1.0;
				weight_range    = 1.0;
				weight_doppler  = 1.0;

				V1              = 0.0;

				dR_trop         = 0.0;
				dR_satpco       = 0.0;
				dR_GraRelativity= 0.0;
				dR_SolidTides   = 0.0;
                dR_OceanTides   = 0.0;
				corrected_value = 0.0;

			}

			double getRange()
			{
				return R1 - R0s - R0g + err_Trop + err_Iono;
			}

			double getVel()
			{
				return V1;
			}
		};

		class TQUXBObsFile
		{
		public:
			TQUXBObsFile(void);
		public:
			~TQUXBObsFile(void);
		public:
			bool write(string  strUXBObsFileName);
			bool open(string strUXBObsFileName); // �����Ƿ���Ӷ�ȡ�۲�����
		public:
			vector<TQUXBObsLine> m_data;
		};
	}
}
