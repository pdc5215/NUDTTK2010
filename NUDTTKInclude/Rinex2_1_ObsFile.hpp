#pragma once
#include "structDef.hpp"
#include <vector>
#include <limits>
#include <windows.h>
#include <map>

//  Copyright 2012, The National University of Defense Technology at ChangSha

using namespace std;
namespace NUDTTK
{
	struct Rinex2_1_MaskString
	{
		static const char szVerType[];
		static const char szComment[];
		static const char szPgmRunDate[];
		static const char szMarkerName[];
		static const char szMarkerNum[];
		static const char szObservAgency[];
		static const char szRecTypeVers[];
		static const char szAntType[];
		static const char szApproxPosXYZ[];
		static const char szAntDeltaHEN[];
		static const char szWaveLenFact[];
		static const char szTypeOfObserv[];
		static const char szInterval[];
		static const char szTmOfFirstObs[];
		static const char szTmOfLastObs[];
		static const char szLeapSec[];
		static const char szLeapSecGNSS[];   // +
		static const char szNumsOfSv[];
		static const char szPRNNumOfObs[];
		static const char szEndOfHead[];
		static const char szIonAlpha[];
		static const char szIonBeta[];
		static const char szDeltaUTC[];
		static const char szMetSensorModType[];
		static const char szMetSensorPos[];
		static const char szDataTypes[];  
		static const char szAnalysisCenter[];
		static const char szTRFofSolnSta[];
		static const char szSolnStaNameNum[];
		static const char szSolnSatsNum[];
		static const char szPRNList[];
	};

	struct Rinex2_1_ObsHeader
	{
		char           szRinexVersion[20 + 1];        // �ļ�����
		char           szFileType[20 + 1];            // A1,19X  'O' for Observation Data 
		char           szSatlliteSystem[20 + 1];      // A1,19X  blank or 'G': GPS
		char           szProgramName[20 + 1];         // ��������
		char           szProgramAgencyName[20 + 1];   // �����������
		char           szFileDate[20 + 1];            // �ļ���������
		char           szObserverName[20 + 1];        // �۲������ƣ�Ĭ�� OBSERVER
		char           szObserverAgencyName[40 + 1];  // �۲��������
		char           szRecNumber[20 + 1];           // ���ջ����
		char           szRecType[20 + 1];             // ���ջ�����
		char           szRecVersion[20 + 1];          // ���ջ�����汾
		char		   szMarkNumber[20 + 1];
		char		   szMarkName[60 + 1];
		char           szAntNumber[20 + 1];           // ���߱��
		char           szAntType[20 + 1];             // ��������
		char           szTimeType[3 + 1];             // ʱ������
		POS3D          ApproxPos;                     // ����λ��
		POS3D          AntOffset;                     // ����ƫ��
		DayTime		   tmStart;                       // ��ʼʱ��
		DayTime		   tmEnd;                         // ��ֹʱ��
		BYTE		   bL1WaveLengthFact;             // 1-������, 2-�벨��,0-(in L2): Single frequency instrument 
		BYTE		   bL2WaveLengthFact;
		BYTE		   bL5WaveLengthFact;
		double         Interval;                     // �۲����ݲ������
		int            LeapSecond;                   // ��1980��1��6�յ�����
		BYTE           bySatCount;                   // ����(���վ)����
		vector<BYTE>   pbySatList;                   // ����(���վ)�б�
		BYTE           byObsTypes;                   // �۲��������͵ĸ��� ����һ��������8
		vector<BYTE>   pbyObsTypeList;              // ��ͬ�۲����ݵ�����,2014/09/02,�����������͸�������9�������
		vector<string> pstrCommentList;              // ע��������, 2008/07/27		
		
		Rinex2_1_ObsHeader()
		{
			memset(this,0,sizeof(Rinex2_1_ObsHeader));
			LeapSecond = INT_MAX;
			Interval   = DBL_MAX;
			bL1WaveLengthFact = 100;
		}

		char getSatSystemChar() // 2012/04/09, ���ӱ���ϵͳ�Ŀ���
		{
			if(szSatlliteSystem[0] == 'G' || szSatlliteSystem[0] == ' ')
				return 'G';
			else if(szSatlliteSystem[0] == 'M')	// 2013/06/26, ���ӻ��ϵͳ�Ŀ���
				return 'M'; 
			else
				return 'C';
		}
	};

	struct Rinex2_1_ObsDatum
	{
		double data;   
		char   lli;
		char   ssi;

		Rinex2_1_ObsDatum()
		{
			data = DBL_MAX;
			lli  = ' ';
			ssi  = ' ';
		}

		bool operator == (Rinex2_1_ObsDatum a)
		{
			if ( this->data == a.data && this->lli == a.lli && this->ssi == a.ssi )
			{
				return true;
			}
			else
				return false;
		}
	 };

	typedef vector<Rinex2_1_ObsDatum>       Rinex2_1_ObsTypeList; // �������ǵĲ�ͬ���͹۲���������
	typedef map<BYTE, Rinex2_1_ObsTypeList> Rinex2_1_SatMap;      // ����ʱ�̵Ĳ�ͬ���ǹ۲������б�

    struct Rinex2_1_ObsEpoch
	{
		DayTime                  t;
        BYTE                     byEpochFlag; 
		BYTE                     bySatCount;		
		Rinex2_1_SatMap          obs;             
	};

	class Rinex2_1_ObsFile
	{
	public:
		Rinex2_1_ObsFile(void);
	public:
		~Rinex2_1_ObsFile(void);
	public:
		void    clear();
		bool    isEmpty();
		bool    cutdata(DayTime t_Begin,DayTime t_End);
        int     isValidEpochLine(string strLine, FILE * pObsfile = NULL);
		bool    open(string  strObsfileName);
		bool	openMixedFile(string  strObsfileName, char cSystem = 'G');
		bool    write(string strObsfileName_noExp);
		bool    write(string strObsfileName_noExp, string& strObsfileName_all);
		bool    Rinex2_1_ObsFile::downSampling(int nSampleSpan, int flag_method = 0, bool flag_Int = 1);
	public:
		Rinex2_1_ObsHeader          m_header;
        vector<Rinex2_1_ObsEpoch>   m_data;
	};
}
