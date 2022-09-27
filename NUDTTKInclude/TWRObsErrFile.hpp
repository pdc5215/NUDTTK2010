#pragma once
#include <string>
#include <vector>
#include "structDef.hpp"

//  Copyright 2017, The National University of Defense Technology at ChangSha
using namespace NUDTTK;
namespace NUDTTK
{
	namespace TwoWayRangingPod
	{
		struct TWRObsErrLine
		{
			UTC    t;             // ʱ��, ����ʱ, �ҹ���վ���ݴ󲿷ֲ��ñ���ʱ
			double correct_R;     // ���������, �������������㡢�����
		};

		class TWRObsErrFile
		{
		public:
			TWRObsErrFile(void);
		public:
			~TWRObsErrFile(void);
		public:
			bool isValidEpochLine(string strLine, TWRObsErrLine &obserrLine);
			bool open(string strTWRObsErrFileName);
			bool write(string strTWRObsErrFileName);
			bool getCorrect_R(UTC t, double &correct_R, int nlagrange = 2);
		public:
			vector<TWRObsErrLine> m_data;
		};
	}
}
