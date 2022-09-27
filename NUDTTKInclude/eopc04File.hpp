#pragma once
#include "structDef.hpp"
#include <string>
#include <vector>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct Eopc04Line
	{
		int     month;    // ��
		int     day;      // ��
		int     mjd;      // ������������
		double  x;        // ����
		double  y;        // ����
		double  ut1_utc;
		double  lod;
		double  psi;
		double  eps; 

		Eopc04Line() 
		{
			month = MONTH_UNKNOWN;
			day   = -1;
		}
	};

	// һ��ĵ�����ת��¼
	struct Eopc04YearRecord
	{
		int                 year;
		int                 interval;        // ��λ����
		int                 mjd_first;       // ��ʼ������
		string              strText[17];
		vector<Eopc04Line>  eopc04Linelist;      
	};

	class Eopc04File
	{
	public:
		Eopc04File(void);
	public:
		~Eopc04File(void);
	public:
		void clear();
		bool isValidEpochLine(string strLine,Eopc04Line& eopc04line);
		bool open(string strEopc04fileName);
		bool getPoleOffset(UTC t, double& x, double& y);
		bool getNutationCorrect(UTC t, double& psi, double& eps);
		bool getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate);
		bool getUT1_UTC(UTC t, double& ut1_utc);
	public:
		vector<Eopc04YearRecord> m_data;
	};
}

