#pragma once
#include "structDef.hpp"
#include <vector>

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SLR
	{
		// ��¼���漤���վ�Ļ�����Ϣ, ������վ��λ���ٶ�
		struct StaSscRecord
		{
			unsigned int id;        // ��վ�������������վַ¼�еı��
			string       name;      // ����
			UTC          t0;        // ����ο�ʱ��
			double       x;         // ��λ��m��
			double       y;         // ��λ��m��
			double       z;         // ��λ��m��
			double       vx;        // ��λ��m/s��
			double       vy;        // ��λ��m/s��
			double       vz;        // ��λ��m/s��
		};

		struct SscLine
		{
			char     szStationFlag[10]; // '10002S001 '
			char     szName[18];        // 'GRASSE            '
			char     szUse[4];          // 'SLR '
			int      id;                // '7835 '
			double   x;
			double   y;
			double   z;
			double   sigma_x;
			double   sigma_y;
			double   sigma_z;
		};

		struct  StaSscEnsemble
		{
			int      id;
			string   name;
			POS6D    posvel;
			POS6D    sigma;
		};

		class StaSscFile
		{
		public:
			StaSscFile(void);
		public:
			~StaSscFile(void);
		public:
			int  isValidSscLine(string strLine, FILE * pSscFile = NULL);
			bool readSscLine(string strLine, SscLine& line);
			bool readSscLine_14(string strLine, SscLine& line);
			void setEpoch(double epoch);
			bool getStaPosVel(UTC t, int id, POS6D& posvel);
			bool getStaSscList(vector<StaSscRecord> &staSscList);
			bool open(string  strSscFileName);
			bool open_14(string  strSscFileName);
		public:
			double                 m_Epoch;
			vector<StaSscEnsemble> m_data;

		};
	}
}
