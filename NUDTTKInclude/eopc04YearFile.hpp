#pragma once
#include "structDef.hpp"
#include <string>
#include <vector>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct Eopc04YearLine
	{
		int     year;     // ������
		int     month;    // ��
		int     day;      // ��
		int     mjd;      // ������������
		double  x;        // ����
		double  y;        // ����
		double  ut1_utc;
		double  lod;
		double  psi;
		double  eps; 

		Eopc04YearLine() 
		{
			month = MONTH_UNKNOWN;
			day   = -1;
		}
	};
	class Eopc04YearFile
	{
	public:
		Eopc04YearFile(void);
	public:
		~Eopc04YearFile(void);
	public:
		void clear();
		bool isValidEpochLine(string strLine,Eopc04YearLine& eopc04yearline);
		bool open(string strEopc04YearfileName);
		bool getPoleOffset(UTC t, double& x, double& y);
		bool getNutationCorrect(UTC t, double& psi, double& eps);
		bool getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate);
		bool getUT1_UTC(UTC t, double& ut1_utc);
	public:
		vector<Eopc04YearLine> m_data;
	};
}

