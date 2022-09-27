#pragma once
#include "structDef.hpp"
#include <vector>

using namespace std;
namespace NUDTTK
{
	// 11�������������ṹ
	struct OceanTideWave
	{// M2  S2  N2  K2  K1  O1  P1  Q1  MF  MM SSA
		double M2;
		double S2;
		double N2;
		double K2;
		double K1;
		double O1;
		double P1;
		double Q1;
		double MF;
		double MM;
		double SSA;

		OceanTideWave()
		{// Ĭ�ϸ�������Ϊ 0
			memset(this, 0, sizeof(OceanTideWave));
		}
	};

	struct StaOceanTide
	{
		int           id;
		string        name_4c;   // ����
		OceanTideWave amplitude[3];
		OceanTideWave phase[3];
	};

    // ͨ�� http://www.oso.chalmers.se/~loading/ ����, ����BLQ��ʽ, �ȵ·崴��
	class StaOceanLoadingDisplacementFile
	{
	public:
		StaOceanLoadingDisplacementFile(void);
	public:
		~StaOceanLoadingDisplacementFile(void);
	public:
		int  isValidLine(string strLine,FILE * pStaOldFile = NULL);
		bool nextValidLine(string &strLine, FILE * pStaOldFile = NULL);
		bool readOceanTideWaveLine(string strLine, OceanTideWave& line);
		bool open(string  strStaOldFileName);
		bool getStaOceanTide(int id, StaOceanTide& sotDatum);
		bool getStaOceanTide(string name, StaOceanTide& sotDatum);
	public:
		vector<StaOceanTide> m_data;
	};
}
