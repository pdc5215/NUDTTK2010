#pragma once
#include "structDef.hpp"
#include <vector>

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SLR
	{
		// ��¼���漤���վ�Ļ�����Ϣ, ������վ��λ���ٶ�
		struct StaSscRecord_14
		{
			unsigned int id;        // ��վ�������������վַ¼�еı��
			string       name;      // ����
			UTC          t;         // ����ο�ʱ��
			UTC          t0;        // ��ʼʱ��
			UTC          t1;        // ��ֹʱ��
			double       x;         // ��λ��m��
			double       y;         // ��λ��m��
			double       z;         // ��λ��m��
			double       vx;        // ��λ��m/s��
			double       vy;        // ��λ��m/s��
			double       vz;        // ��λ��m/s��
		};

		struct SscLine_14
		{
			char     szStationFlag[10]; // '10002S001 '
			char     szName[18];        // 'GRASSE            '
			char     szUse[4];          // 'SLR '
			int      id;                // '7835 '
			UTC      t0;
			UTC      t1;
			//int      sat_id;            // ���
			double   x;
			double   y;
			double   z;
			double   sigma_x;
			double   sigma_y;
			double   sigma_z;
		};

		struct  StaSscLine
		{
			string       name;      // ����
			int      id;
			UTC      t0;
			UTC      t1;
			POS6D    posvel;
			POS6D    sigma;
		};
		struct StaSscEnsemble_14
		{
			int                   id;
			vector<StaSscLine>    sscLineList;
		};

		class StaSscFile_14
		{
		public:
			StaSscFile_14(void);
		public:
			~StaSscFile_14(void);
		public:
			UTC  getTime(int year, int day, int second);
			int  isValidSscLine(string strLine, FILE * pSscFile = NULL);
			bool readSscLine(string strLine, SscLine_14& line);
			void setEpoch(double epoch);
			bool getStaPosVel(UTC t, int id, POS6D& posvel);
			bool getStaSscList(vector<StaSscRecord_14> &staSscList);
			bool open(string  strSscFileName);
		public:
			double                    m_Epoch;
			vector<StaSscEnsemble_14> m_data;

		};
	}
}
