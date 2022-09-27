#pragma once
#include <string>
#include <vector>
#include "structDef.hpp"
#include "Matrix.hpp"
#include <math.h>

namespace NUDTTK
{
	namespace TQPod
	{
		struct TQISLObsLine
		{
			UTC    t;                    // ����ʱ��
			double range;
			double ISL_Code;            // �Ǽ伤��α��۲�ֵ
			double ISL_Phase;            // �Ǽ伤����λ�۲�ֵ
			

			// ��������
			string satNameA;             // ����A
			string satNameB;             // ����B


			// ����Ԥ������ز���
			TimePosVel tqOrb_A;            // ����A��tʱ�̵�������
			TimePosVel tqOrb_B;            // ����B��tʱ�̵�������
			Matrix     tqRtPartial_A;      // ����A��tʱ�̵�ƫ����
			Matrix     tqRtPartial_B;      // ����B��tʱ�̵�ƫ����

			double     dR_satpco;        // ������������ƫ��,����λ������
			double     dR_lightTime;     // ����ʱ����
		    //double      dR_sun;          // ̫����Ӱ�죿 ���һ��ȷ��
			double     corrected_value;  // ����ֵ, ���ھ�������:�������������͹���ʱ����

			// ���ܶ�����ز���
			double weight_code;        // α����Ȩ��
			double weight_phase;       // ��λ���Ȩ��
			double oc_code;            // α����в�
			double oc_phase;           // ��λ���в�
			double rw_code;            // α����³��Ȩֵ
			double rw_phase;           // ��λ���³��Ȩֵ

			double ambiguity;          // ��λģ������
			                           // ��λģ��������һ���̶�ֵ��ÿ�����ǵ�ÿ�����ζ���һ����
			                           // �ڷ�������ʱ�����������Ļ��μ��ɣ�
			                           // ģ������������ֱ�Ӹ�����Ҳ�����������һ����
			
			TQISLObsLine()
			{
				ISL_Code          = 0.0; // �Ǽ伤��α������ֵ
				ISL_Phase         = 0.0; // �Ǽ伤����λ�����ֵ

				dR_satpco         = 0.0; // ��������ƫ��
				dR_lightTime      = 0.0; // �Ǽ����ʱ

				weight_code       = 0.0; // α����Ȩ��
				weight_phase      = 0.0; // ��λ���Ȩ��
				oc_code           = 0.0; // α����в�
				oc_phase          = 0.0; // ��λ���в�
				rw_code           = 0.0; // α����³��Ȩֵ
				rw_phase          = 0.0; // ��λ���³��Ȩֵ
				ambiguity         = 0.0;
			}

			double getCode()
			{
				return ISL_Code + dR_satpco + dR_lightTime;
			}

			double getPhase()
			{
				return ISL_Phase + dR_satpco + dR_lightTime;
			}
		};

		class TQISLObsFile
		{
		public:
			TQISLObsFile(void);
		public:
			~TQISLObsFile(void);
		public:
			bool write(string strISLObsFileName);
			bool open(string strISLObsFileName); 
		public:
			vector<TQISLObsLine> m_data;
		};
	}
}
