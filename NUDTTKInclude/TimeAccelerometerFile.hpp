#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include <string>
#include <vector>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
namespace NUDTTK
{
	struct TimeAccLine
	{
		GPST  t;
		POS3D acc;     // ���Լ��ٶ�     ���ٶȼ���������ϵ��
		POS3D ang;     // �Ǽ��ٶ�
		POS3D res;     // �в�
		int   flag;
	};

	class TimeAccelerometerFile
	{
	public:
		TimeAccelerometerFile(void);
	public:
		~TimeAccelerometerFile(void);
	public:
		bool open(string strAccelerometerFileName);
		bool isValidEpochLine(string strLine, TimeAccLine& line);
		bool getAcc(GPST t, POS3D &lin_acc, POS3D &ang_acc, int nlagrange = 2);    // Flag = 1 ���Բ�ֵ�� Flag = 2 �������ղ�ֵ
		bool write(string  strAccelerometerFileName);
	public:
		vector<TimeAccLine> m_data;
	};
}