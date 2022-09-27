#pragma once
#include "structDef.hpp"
#include <string>
#include <vector>

namespace NUDTTK
{
	struct EopRapidFileIAU2000Line
	{
		int     year;    // ��
		int     month;   // ��
		int     day;     // ��
		double  mjd;     // ������������
		double  pm_x;    // ��λ: ����     
		double  pm_y;    // ��λ: ����    
		double  ut1_utc; // ��λ: ��

		EopRapidFileIAU2000Line() 
		{
			month =  MONTH_UNKNOWN;
			day   = -1;
		}
	};

	class eopRapidFileIAU2000
	{
	public:
		eopRapidFileIAU2000(void);
	public:
		~eopRapidFileIAU2000(void);
	public:
		void clear();
		bool isValidEpochLine(string strLine,EopRapidFileIAU2000Line& eopLine);
		bool open(string strEopRapidFileName);
		bool getPoleOffset(UTC t, double& x, double& y);
		bool getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate);
		bool getUT1_UTC(UTC t, double& ut1_utc);
	public:
		vector<EopRapidFileIAU2000Line> m_data;
	};
}
