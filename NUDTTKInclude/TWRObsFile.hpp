#pragma once
#include <string>
#include <vector>
#include "structDef.hpp"
#include "Matrix.hpp"
#include <math.h>

//  Copyright 2017, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
namespace NUDTTK
{
	namespace TwoWayRangingPod
	{
		struct TWRObsLine
		{
			UTC    t;               // ����ʱ��, ����ʱ, �ҹ���վ���ݴ󲿷ֲ��ñ���ʱ
			double R;               // �۳��豸��ֵ�Ĳ��ֵ��������������ֵ��
			double R0s;             // ���Ͼ�����У��ֵ
			double R1;              // ԭʼ���ֵ
			double R0g;             // ���������ֵ
			double R01g;            // ���������ֵ����������

			// ����Ԥ������ز���
			POS3D      pos_J2000;
			TimePosVel geoOrb;
			Matrix     geoRtPartial;
			double     dR_trop;         // ����������
			double     dR_satpco;       // ������������ƫ��
			double     dR_GraRelativity;
			double     dR_SolidTides;
			double     dR_OceanTides;
			double     dR_up;
			double     dR_down;
			double     Azimuth;         // �۲ⷽλ��, ����������ϵ
			double     Elevation;       // �۲�߶Ƚǣ�����������ϵ
			double     corrected_value; // ����ֵ, ���ھ�������

			// ���ܶ�����ز���
			double weight;
			double oc;
			double rw;              // ³��Ȩֵ

			TWRObsLine()
			{
				R               = 0.0;
				R0s             = 0.0;
				R1              = 0.0;
				R0g             = 0.0;
				R01g            = 0.0;
				dR_trop         = 0.0;
				dR_satpco       = 0.0;
				dR_GraRelativity= 0.0;
				dR_SolidTides   = 0.0;
                dR_OceanTides   = 0.0;
				corrected_value = 0.0;
				oc              = 0.0;
				weight          = 1.0;
				rw              = 1.0;
			}
			double getRange()
			{
				return R1 - R0s - R0g + R01g;
			}
		};

		class TWRObsFile
		{
			public:
				TWRObsFile(void);
			public:
				~TWRObsFile(void);
			public:
				bool isValidEpochLine(string strLine, TWRObsLine &obsLine);
				bool open(string strFileName);
			public:
				vector<TWRObsLine> m_data;
		};
	}
}
