#include "StaOceanLoadingDisplacementFile.hpp"

namespace NUDTTK
{
	StaOceanLoadingDisplacementFile::StaOceanLoadingDisplacementFile(void)
	{
	}

	StaOceanLoadingDisplacementFile::~StaOceanLoadingDisplacementFile(void)
	{
	}

	// ����ֵ: 0 - EOF; 1 - ��Ч��; 2 - ��Ч��
	int StaOceanLoadingDisplacementFile::isValidLine(string strLine,FILE * pStaOldFile)
	{
		if(pStaOldFile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pStaOldFile))
				return 0;
		}
		char szHead[3];
		sscanf(strLine.c_str(),"%2c",szHead);
		szHead[2] = '\0';
		// ����ע����
		if(strcmp(szHead,"$$") == 0)
			return 2;
		return 1;
	}

	// �����һ����Ч�ļ�¼��(���ԡ�$$����)
	bool StaOceanLoadingDisplacementFile::nextValidLine(string &strLine, FILE * pStaOldFile)
	{
		if(pStaOldFile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pStaOldFile))
				return false;
		}
		char szline[200]; 
		int  bFlag = 1;
		fgets(szline, 200, pStaOldFile);
		while(bFlag)
		{
			int nLineFlag = isValidLine(szline, pStaOldFile);
			if(nLineFlag == 0)
			{// �ļ�����
				bFlag = false;
				return  false;
			}
			else if(nLineFlag == 1)
			{// ������Ч��¼
				strLine = szline;
				return  true;
			}
			else
			{// ��Ч��
				fgets(szline, 200, pStaOldFile);
			}
		}
		return false;
	}

    // ����������
	bool StaOceanLoadingDisplacementFile::readOceanTideWaveLine(string strLine, OceanTideWave& line)
	{
		if(strLine.length() < 78)
			return false;
		OceanTideWave OTWLine;
		// M2  S2  N2  K2  K1  O1  P1  Q1  MF  MM SSA
		sscanf(strLine.c_str(), "%*2c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf%*c%6lf",
								&OTWLine.M2,
								&OTWLine.S2,
								&OTWLine.N2,
								&OTWLine.K2,
								&OTWLine.K1,
								&OTWLine.O1,
								&OTWLine.P1,
								&OTWLine.Q1,
								&OTWLine.MF,
								&OTWLine.MM,
								&OTWLine.SSA);
		line = OTWLine;
		return true;
	}

	bool StaOceanLoadingDisplacementFile::open(string  strStaOldFileName)
	{
		FILE * pStaOldFile = fopen(strStaOldFileName.c_str(),"r+t");
		if(pStaOldFile == NULL) 
			return false;
		string strLine; 
		int k = 0;
		m_data.clear();
		StaOceanTide sotDatum;
		while(nextValidLine(strLine, pStaOldFile))
		{
			switch(k%7)
			{
			case 0:
				if(k > 0)
				{// �����һ����վ�ĳ�ϫ����
					m_data.push_back(sotDatum);
				}
				// ��¼�²�վID
				char szName_4c[4+1];
				sscanf(strLine.c_str(),"%*2c%4c", szName_4c);
				szName_4c[4] = '\0';
                sotDatum.id = atoi(szName_4c);
				sotDatum.name_4c = szName_4c;
				//sscanf(strLine.c_str(),"%*2c%4d", &sotDatum.id); // 2013/10/22, ����Ϊ��վ����
				break;
			case 1:
				// ���
				readOceanTideWaveLine(strLine, sotDatum.amplitude[0]);
				break;
			case 2:
				// ���
				readOceanTideWaveLine(strLine, sotDatum.amplitude[1]);
				break;
			case 3:
				// ���
				readOceanTideWaveLine(strLine, sotDatum.amplitude[2]);
				break;
			case 4:
				// ��λ
				readOceanTideWaveLine(strLine, sotDatum.phase[0]);
				break;
			case 5:
				// ��λ
				readOceanTideWaveLine(strLine, sotDatum.phase[1]);
				break;
			case 6:
				// ��λ
				readOceanTideWaveLine(strLine, sotDatum.phase[2]);
				break;
			}
			k++;
		}
		if(k > 0)
		{// ������һ����վ�ĳ�ϫ����, 2014/10/12
			m_data.push_back(sotDatum);
		}
		fclose(pStaOldFile);
		return true;
	}

	// ��ø�����ŵĲ�վ�ĳ�ϫ����
	bool StaOceanLoadingDisplacementFile::getStaOceanTide(int id, StaOceanTide& sotDatum)
	{
		bool bFind = false;
		size_t count = m_data.size();
		if(count <= 0)
			return false;
		int i;
		for(i =  int(count) - 1; i >= 0; i--)
		{// �����������Ĳ�վ��Ϣ, 2008/01/21
			if(m_data[i].id == id)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
			return false;
		sotDatum = m_data[i];
		return true;
	}

	// ��ø������ƵĲ�վ�ĳ�ϫ����
	bool StaOceanLoadingDisplacementFile::getStaOceanTide(string name, StaOceanTide& sotDatum)
	{
		bool bFind = false;
		size_t count = m_data.size();
		if(count <= 0)
			return false;
		// ��վ����ת���ɴ�д
        char scNAME[5];
		for(int i = 0; i < 4; i++)
			scNAME[i] = toCapital(name[i]);
		scNAME[4] = '\0';
		int i;
		for(i =  int(count) - 1; i >= 0; i--)
		{// �����������Ĳ�վ��Ϣ, 2008/01/21
			if(m_data[i].name_4c.find(scNAME) != -1)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
			return false;
		sotDatum = m_data[i];
		return true;
	}
}
