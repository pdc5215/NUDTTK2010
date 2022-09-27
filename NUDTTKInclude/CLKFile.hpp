#pragma once
#include "structDef.hpp"
#include <vector>
#include <windows.h>
#include <map>
#include <limits>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	// ��վ�����ֺ�����
	struct CLKStaNamePos
	{
		char    szName[4 + 1];
		char    szID[20 + 1];
		__int64 lnX; // ��λ����, 11λ, ��ͨ���� INT_MAX = 2147483647 �����, ����ʹ��64λ����
		__int64 lnY;
		__int64 lnZ;
	};

	// �Ӳ��ļ�ͷ��Ϣ
	struct CLKHeader
	{
		char                    szRinexVersion[20 + 1]; // �ļ�����
		char                    cFileType;
		char                    szProgramName[20 + 1];
		char                    szAgencyName[20 + 1];
		char                    szFileDate[20 + 1];
		int                     LeapSecond;             // ��1980��1��6�յ�����
		int                     ClockDataTypeCount;     // �Ӳ��������͸���
		vector<string>          pstrClockDataTypeList;
		char                    szACShortName[3 + 1];   // AC - ��������
		char                    szACFullName[55 + 1];
		int                     nStaCount;              
		char                    szStaCoordFrame[50 + 1];
		vector<CLKStaNamePos>   pStaPosList;     
		BYTE                    bySatCount;
		vector<string>          pszSatList; 
		CLKHeader()
		{
			//memset(this, 0, sizeof(CLKHeader));
			LeapSecond = INT_MAX;
			ClockDataTypeCount = INT_MAX;
			nStaCount = INT_MAX;
			bySatCount = 0;
		}
	};

	struct CLKDatum
	{
		string name; 
		int    count;
		double clkBias;
		double clkBiasSigma;
		double clkRate;
		double clkRateSigma;
		double clkAcc;
		double clkAccSigma;
		CLKDatum()
		{
			/*  
				memset ʹ�� string �ڲ�ָ�� _Bx._Ptrr ֵΪ 0, _Myres Ϊ 0, ����������µ� string ���󱻸�ֵΪС�ַ���(�ֽ���С�ڵ���16���ַ���)ʱ,
				����������ڴ��ں����ò����ͷ�, ��������ڴ汻й¶��,���� string ���ڴ�����㷨��֪����ڴ��С���� 16 ���ֽ�.
				��������ֵΪ���ַ���(�ֽ�������16���ַ���)ʱ, ����û���ڴ�й¶, ������Ϊ��������ڴ����������´θ�ֵʱ���ܱ��ͷ�.
			*/

			//memset(this, 0, sizeof(CLKDatum)); // 2014/10/19 

			name = "";
			count = 0;
			clkBias = DBL_MAX;
			clkBiasSigma = DBL_MAX;
			clkRate = DBL_MAX;
			clkRateSigma = DBL_MAX;
			clkAcc = DBL_MAX;
			clkAccSigma = DBL_MAX;
		}
	};

	typedef map<string, CLKDatum>  CLKMap;

	struct CLKEpoch
	{
		GPST     t;
		CLKMap   ASList; 
		CLKMap   ARList;
	};

	class CLKFile
	{
	public:
		CLKFile(void);
	public:
		~CLKFile(void);
	public:
		void   clear();
		bool   isEmpty();
		int    isValidEpochLine(string strLine, FILE * pCLKFile = NULL);
		int    isValidEpochLine_rinex304(string strLine, FILE * pCLKFile = NULL);
		double getEpochSpan();
		bool   open(string  strCLKFileName);
		bool   open_rinex304(string  strCLKFileName);
		bool   open_LeoClk(string  strCLKFileName);  // �͹������Ӳ���ڴ�ļ������
		bool   write(string strCLKFileName);
		bool   getSatClock(GPST t, int nPRN, CLKDatum& clkDatum, int nLagrange = 3, char cSatSystem = 'G');
		bool   getSatClock(GPST t,   string name, CLKDatum& clkDatum, int nLagrange = 3);
		bool   getSatClock_0(GPST t, string name, CLKDatum& clkDatum, int nLagrange = 3);
		
		bool   getStaClock(GPST t, string name, CLKDatum& clkDatum, int nLagrange = 3);
		bool   getLeoClock(GPST t, string name, CLKDatum& clkDatum, int nLagrange = 3); // �͹������Ӳ���ڴ�ļ������
	public:
		CLKHeader          m_header;
		vector<CLKEpoch>   m_data;
	};
}
