#pragma once
#include "structDef.hpp"
#include <string>
#include <vector>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	namespace Geodyn
	{
		struct SolarFluxLine
		{
			int    year;
			int    month;
			int    day;
			double flux; // ̫�������ĵ�λ�� solar flux units (s. f. u.)

			SolarFluxLine() 
			{
				year  = -1;
				month = -1;
				day   = -1;
			}

			DayTime getTime()
			{
				// �� 0h Ϊ�ο�ʱ��, ����һ��ȷ��
				DayTime t(year, month, day, 0, 0, 0);
				return t;
			}
		};

		class solarFluxFile
		{
		public:
			solarFluxFile(void);
		public:
			~solarFluxFile(void);
		public:
			bool isValidEpochLine(string strLine, SolarFluxLine& solarFluxLine);
			bool open(string  strSolarFluxFileName);
			bool getSolarFlux(DayTime t, double& flux);
			bool getLatestSolarFlux_day(DayTime t,double& flux_day);
			bool getLatestSolarFlux_mean(DayTime t,double& flux_mean, int n = 27);
		public:
			vector<SolarFluxLine> m_data;
		};
	}
}
