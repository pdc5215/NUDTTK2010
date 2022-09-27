#pragma once
#include "structDef.hpp"
#include "TimeCoordConvert.hpp"
using namespace std;

namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		struct CHASCLine
		{
			char   szStarcameraflag[5];
			double q1;
			double q2;
			double q3;
			double q4;
			double accuracy;
		};

		struct CHASCEpoch
		{
			UTC       t; // reference utc cis
			CHASCLine attline;
		};

		// CHAMP���ǵ���̬�����ļ�, ASC��ʽ
		class champASCFile
		{
		public:
			champASCFile(void);
		public:
			~champASCFile(void);
		public:
			bool open(string strCHASCFileName);
			int  isValidEpochLine(string strLine,FILE * pCHASCFile = NULL);
			bool exportTimeAttitudeFile(string  strTimeAttitudeFileName);
		public:
			TimeCoordConvert   m_TimeCoordConvert;  // ʱ������ϵת��
			vector<CHASCEpoch> m_data;
		};
	}
}