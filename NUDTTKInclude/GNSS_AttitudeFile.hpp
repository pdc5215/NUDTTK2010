#pragma once
#include "structDef.hpp"
#include <vector>
#include <windows.h>
#include <map>
#include <limits>
#include "Matrix.hpp"
//  Copyright 2021, Sysu at zhuhai
//  COD�ṩ����̬����Ϊ ECEF->SAT. BODY FRAME
//  COORD_SYSTEM        IGS14
//  FRAME_TYPE          ECEF
//  TIME_SYSTEM         GPS
using namespace std;
namespace NUDTTK
{
	// ��̬�ļ�ͷ����
	struct AttHeader
	{
		char           szTimeSystem[4+1];   // ��9�У�ʱ��ϵͳ
		GPST           tmStart;             // ��10�У��ļ���ʼʱ��
		GPST           tmEnd;               // ��11�У��ļ�����ʱ��
		double         EpochSpan;           // ��12�У�EPOCH_INTERVAL
		char           szCoordinateSys[5+1];// ��13�У�Coord_system
		char           szFrameType[4+1];    // ��14�У�Frame_type
		char           szOrbit_Type[4+1];   // ��15��
		vector<string> pstrSatNameList;     // �����б�
		AttHeader()
		{
			 pstrSatNameList.clear();
		}
		~AttHeader()
		{
		}	
	};

	// ��̬���ݲ���
	typedef map<string, ATT_Q4> AttSatMap;      // ����ʱ�̣���ͬ������̬�����б�

	struct AttEpoch
	{
		GPST t;
		AttSatMap attMap;
	};

	class GNSS_AttFile
	{
	public:
		GNSS_AttFile(void);
	public:
		~GNSS_AttFile(void);
	public:
		static int getSatPRN(string strSatName);
	public:
		void   clear();
		bool   isEmpty();
		int    isValidEpochLine(string strLine, FILE *pAttFILE = NULL);
		double getEpochSpan();
		bool   open(string strAttFileName);
		bool   write(string strAttFileName);
		bool   getQ4(GPST t, string name, ATT_Q4 &Q4, int nlagrange = 9);
		bool   getAttMatrix(GPST t, string name, Matrix& matATT, int nlagrange = 9);//ʱ������һ���㣬��ֵ�����͵øߡ�LEO���ҿ�4�ף�GNSS��9��
	public:
		AttHeader        m_header;
		vector<AttEpoch> m_data;

	};
}