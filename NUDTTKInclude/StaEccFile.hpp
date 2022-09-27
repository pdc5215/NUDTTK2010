#pragma once
#include "structDef.hpp"
#include <vector>

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SLR
	{
		// ��¼�����վ��ƫ������
		struct StaEccRecord
		{
			unsigned int id;   // ��վ�������������վַ¼�еı��
			UTC          t0;   // ��ʼʱ��
			UTC          t1;   // ��ֹʱ��
			ENU          ecc;
		};

		struct EccLine
		{
			int      id;
			char     szPT[3];
			char     szSoln[5];
			char     szT[2];
			UTC      t0;
			UTC      t1;
			char     szUNE[4];
			ENU      ecc;
			char     szCDP[9];
		};

		struct StaEccEnsemble
		{
			int                id;
			vector<EccLine>    eccLineList;
		};

		// �����վ��ƫ�������ļ�, �ɹȵ·崴��
		class StaEccFile
		{
		public:
			StaEccFile(void);
		public:
			~StaEccFile(void);
		public:
			UTC  getTime(int year, int day, int second);
			bool readEccLine(string strLine, EccLine& line);
			bool open(string strEccFileName);
			bool getStaEccRecordList(vector<StaEccRecord> &staEccList);
		public:
			vector<StaEccEnsemble> m_data;
		};
	}
}
