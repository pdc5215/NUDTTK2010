#pragma once
#include "structDef.hpp"
#include <vector>
#include <limits>
#include <windows.h>
#include <map>
#include "Rinex2_1_ObsFile.hpp"

//  Copyright 2014, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct Rinex2_1_MixedObsHeader
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
		vector<string> pstrSatList;                  // ����(���վ)�б�, 2014/03/22, ���Mixed��ʽ���е���
		BYTE           byObsTypes;                   // �۲��������͸���
		vector<BYTE>   pbyObsTypeList;				 // ��ͬ�۲����ݵ�����
		vector<string> pstrCommentList;              // ע��������, 2008/07/27		
		
		Rinex2_1_MixedObsHeader()
		{
			//memset(this,0,sizeof(Rinex2_1_ObsHeader));
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
		void init(Rinex2_1_ObsHeader m_header);
	};
	
	typedef map<string, Rinex2_1_ObsTypeList> Rinex2_1_MixedSatMap; // ����ʱ�̵Ĳ�ͬ���ǹ۲������б�

    struct Rinex2_1_MixedObsEpoch
	{
		DayTime                  t;
        BYTE                     byEpochFlag; 
		BYTE                     bySatCount;		
		Rinex2_1_MixedSatMap     obs;             
	};

	class Rinex2_1_MixedObsFile
	{
	public:
		Rinex2_1_MixedObsFile(void);
	public:
		~Rinex2_1_MixedObsFile(void);
	public:
		void    clear();
        bool    isEmpty();
        int     isValidEpochLine(string strLine, FILE * pObsfile = NULL);
		bool	open(string  strObsfileName, string strSystem = "G+C");
		bool	open_5Obs(string  strObsfileName, string strSystem = "G+C");
		bool    write(string strObsfileName_noExp);
		bool    write_5Obs(string strObsfileName_noExp);
		bool    write(string strObsfileName_noExp, string& strObsfileName_all);
		bool    write_5Obs(string strObsfileName_noExp, string& strObsfileName_all);
	public:
		Rinex2_1_MixedObsHeader          m_header;
		vector<Rinex2_1_MixedObsEpoch>   m_data;
	};
}
