#include "Rinex3_0_NavFile.hpp"
#include "Rinex3_0_ObsFile.hpp"
#include "TimeCoordConvert.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"

namespace NUDTTK
{
	Rinex3_0_NavFile::Rinex3_0_NavFile(void)
	{
		m_typeSatSystem = 0;         // Ĭ��GPSϵͳ
		if(m_typeSatSystem == 1)
		{
			m_EARTH_W  = CGCS2000_EARTH_W;  // CGCS2000
			m_GM_EARTH = CGCS2000_GM_EARTH; // CGCS2000
		}
		else
		{
			m_EARTH_W  = WGS84_EARTH_W;  // WGS84
			m_GM_EARTH = WGS84_GM_EARTH; // WGS84
		}
	}

	Rinex3_0_NavFile::~Rinex3_0_NavFile(void)
	{
	}

	void Rinex3_0_NavFile::clear()
	{
		m_header = Rinex3_0_NavHeader::Rinex3_0_NavHeader();
		m_data.clear();
	}

	bool Rinex3_0_NavFile::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}

	int Rinex3_0_NavFile::isValidEpochLine(string strLine, FILE * pNavfile)
	{
		DayTime tmEpoch;
		// ���漸��������int��, ������ΪstrLine�ĸ�ʽ��������sscanf������������
		int nSatPRN = -1;  	
		if(pNavfile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pNavfile))
				return 0;
		}
		sscanf(strLine.c_str(),"%*1c%2d%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2lf",
			                   &nSatPRN,
							   &tmEpoch.year,
							   &tmEpoch.month,
							   &tmEpoch.day,
							   &tmEpoch.hour,
							   &tmEpoch.minute,
							   &tmEpoch.second);
		int nFlag = 1;
		if(tmEpoch.month > 12 || tmEpoch.month < 0)
			nFlag = 2;
		if(tmEpoch.day > 31 || tmEpoch.day < 0)
			nFlag = 2;
		if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
			nFlag = 2;
		if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
			nFlag = 2;
		if(tmEpoch.second > 60 || tmEpoch.second < 0)
			nFlag = 2;
		if(nSatPRN > MAX_PRN || nSatPRN < 0)
			nFlag = 2;
		return nFlag;
	}

	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strNavfileName : �۲������ļ�·��
	// ���룺strNavfileName
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/12
	// �汾ʱ�䣺2013/06/12
	// �޸ļ�¼��
	// ��ע�� �ο�Rinex2_1_NavFile��д
	bool Rinex3_0_NavFile::open(string  strNavfileName)
	{// ͨ����׺�����ж�
		if(isWildcardMatch(strNavfileName.c_str(), "*.*N", true))
		{
			m_typeSatSystem = 0;         // GPSϵͳ
			m_EARTH_W  = WGS84_EARTH_W;  // WGS84
			m_GM_EARTH = WGS84_GM_EARTH; // WGS84
		}
		if(isWildcardMatch(strNavfileName.c_str(), "*.*r", true))
		{
			m_typeSatSystem = 1;            // BDϵͳ
			m_EARTH_W  = CGCS2000_EARTH_W;  // CGCS2000
			m_GM_EARTH = CGCS2000_GM_EARTH; // CGCS2000
		}

		if(!(isWildcardMatch(strNavfileName.c_str(), "*.*N", true) || isWildcardMatch(strNavfileName.c_str(), "*.*r", true)))
		{
			return false;
		}

		FILE * pNavfile = fopen(strNavfileName.c_str(), "r+t");
		if(pNavfile == NULL) 
			return false;
		m_header = Rinex3_0_NavHeader::Rinex3_0_NavHeader();
		// ��ȡ�ļ�ͷ
		int bFlag = 1;
		while(bFlag)
		{
			char line[100];
			fgets(line, 100, pNavfile);
			string strLineMask = line;
			string strLine     = line;
			strLineMask.erase(0, 60);     // �ӵ�0��Ԫ�ؿ�ʼ��ɾ��60��
			// �޳�\n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n, 1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length ��������'\0'
				strLineMask.append(20 - strLineMask.length(), ' ');
			if(strLineMask == Rinex3_0_MaskString::szVerType)
			{
				strLine.copy(m_header.szRinexVersion, 20, 0);
				strLine.copy(m_header.szFileType, 20, 20);
				strLine.copy(m_header.szSatSystem, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szPgmRunDate)
			{
				strLine.copy(m_header.szProName, 20, 0);
				strLine.copy(m_header.szAgencyName, 20, 20);
				strLine.copy(m_header.szFileDate, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szIonoCorr)
			{				
				if(strLine.find("BD2A") != -1 || strLine.find("GPSA") != -1)
				{
					stringRaplaceA2B(line,'D','E');
					sscanf(line,"%*5c%12lf%12lf%12lf%12lf",
								&m_header.pdIonAlpha[0],
								&m_header.pdIonAlpha[1],
								&m_header.pdIonAlpha[2],
								&m_header.pdIonAlpha[3]);
				}
				else if(strLine.find("BD2B") != -1 || strLine.find("GPSB") != -1 )
				{					
					stringRaplaceA2B(line, 'D', 'E');
					sscanf(line,"%*5c%12lf%12lf%12lf%12lf",
								&m_header.pdIonBeta[0],
								&m_header.pdIonBeta[1],
								&m_header.pdIonBeta[2],
								&m_header.pdIonBeta[3]);
				}
			}			
			else if(strLineMask == Rinex3_0_MaskString::szTimeSysCorr)
			{
				strLine.copy(m_header.szTimeToUTC, 4, 0);
				stringRaplaceA2B(line, 'D', 'E');
				sscanf(line,"%*4c%17lf%16lf%7d%5d",
							&m_header.deltaUTC.dA0,
							&m_header.deltaUTC.dA1,
							&m_header.deltaUTC.lnT,
							&m_header.deltaUTC.lnW);			
			}//
			else if(strLineMask == Rinex3_0_MaskString::szLeapSec)
			{
				sscanf(line,"%6d", &m_header.lnLeapSeconds);
			}
			else if(strLineMask == Rinex3_0_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else // Comment�Ȳ�������
			{
			}
		}
		// �۲�����
		bFlag = TRUE;
		m_data.clear();
		int k = 0;
		char line[100];
		fgets(line, 100, pNavfile);
		NavDatumList navDatumList;
		while(bFlag)
		{
			string strLine = line;
			int nFlag = isValidEpochLine(strLine, pNavfile);
			if(nFlag == 0)      // �ļ�ĩβ
			{
				bFlag=false;
			}
			else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
			{
				k++;
				Rinex2_1_NavDatum navDatum;
				// PRN / EPOCH / SV CLK
				stringRaplaceA2B(strLine, 'D', 'E');
				sscanf(strLine.c_str(),"%*1c%2d%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2lf%19lf%19lf%19lf",
					                   &navDatum.bySatPRN,
									   &navDatum.tmTimeOfClock.year,
					                   &navDatum.tmTimeOfClock.month,
									   &navDatum.tmTimeOfClock.day,
									   &navDatum.tmTimeOfClock.hour,
									   &navDatum.tmTimeOfClock.minute,
									   &navDatum.tmTimeOfClock.second,
					                   &navDatum.dSVClockBias,
									   &navDatum.dSVClockDrift,
									   &navDatum.dSVClockDriftRate);
				//navDatum.tmTimeOfClock.year = yearB2toB4(navDatum.tmTimeOfClock.year);
				// BROADCAST ORBIT - 1
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dIODE,
									   &navDatum.dCrs,
									   &navDatum.dDetla_n,
									   &navDatum.dM0);
				// BROADCAST ORBIT - 2
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dCuc,
									   &navDatum.dEccentricity,
									   &navDatum.dCus,
									   &navDatum.dSqrt_A);
				// BROADCAST ORBIT - 3
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dTOE,
									   &navDatum.dCic,
									   &navDatum.dOMEGA,
									   &navDatum.dCis);
				// BROADCAST ORBIT - 4
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.d_i0,
									   &navDatum.dCrc,
									   &navDatum.d_omega,
									   &navDatum.dOMEGADOT);
				// BROADCAST ORBIT - 5
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.didot,
									   &navDatum.dCodesOnL2Channel,
									   &navDatum.dWeek,
									   &navDatum.dL2PDataFlag);
				if(navDatum.dWeek == DBL_MAX)
					navDatum.dWeek = 0;
				if(navDatum.dL2PDataFlag == DBL_MAX)
					navDatum.dL2PDataFlag = 0;
				// BROADCAST ORBIT - 6
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine     = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dSVAccuracy,
									   &navDatum.dSVHealth,
									   &navDatum.dTGD,
									   &navDatum.dIODC);
				// BROADCAST ORBIT - 7
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dTransmissionTimeOfMessage,
									   &navDatum.dFitInterval,
									   &navDatum.dSpare1,
									   &navDatum.dSpare2);
				navDatumList.push_back(navDatum);
				fgets(line, 100, pNavfile);
			}
			else
			{
				fgets(line, 100, pNavfile);
			}
		}
		fclose(pNavfile);

		// ͳ�ƻ�� m_datalist_sat 20071005
		NavDatumList navDatumList_sat[MAX_PRN];
		for(int i = 0; i < MAX_PRN; i++)
			navDatumList_sat[i].clear();
		for(size_t s_i = 0; s_i < navDatumList.size(); s_i++)
		{
			if(navDatumList[s_i].bySatPRN < MAX_PRN)
				navDatumList_sat[navDatumList[s_i].bySatPRN].push_back(navDatumList[s_i]);
		}
		for(int i = 0; i < MAX_PRN; i++)
		{
			if(navDatumList_sat[i].size() > 0)
			{
				m_data.insert(NavSatMap::value_type(i, navDatumList_sat[i]));
			}
		}
		return true;
	}

	void Rinex3_0_NavFile::writedouble(FILE* pNavfile, double value)
	{
		if(pNavfile)
		{
			if(value != DBL_MAX)
			{
				char szValue[21];
				string strValue;
				sprintf(szValue, "%20.12E", value);
				stringEraseFloatZero(szValue, strValue);
				stringRaplaceA2B(strValue, 'E', 'D'); // 2012/04/24
				fprintf(pNavfile, "%19s", strValue.c_str());
			}
			else
				fprintf(pNavfile, "%-19s", " ");
		}
	}

	bool Rinex3_0_NavFile::write(string strNavfileName)
	{
		if(isEmpty())
			return false;
		// д�ļ�ͷ
		NavSatMap::const_iterator it_0 = m_data.begin();
		int n1  = getIntBit(it_0->second[0].tmTimeOfClock.year, 0);
		int n10 = getIntBit(it_0->second[0].tmTimeOfClock.year, 1);
		char strFileExp[5];
		if(m_typeSatSystem == 1)
			sprintf(strFileExp, ".%1d%1dR", n10, n1);
		else
            sprintf(strFileExp, ".%1d%1dN", n10, n1);

		strNavfileName = strNavfileName + strFileExp;
		FILE* pNavfile = fopen(strNavfileName.c_str(), "w+");
		fprintf(pNavfile,"%20s%20s%20s%20s\n",
			             m_header.szRinexVersion,
						 m_header.szFileType,
						 m_header.szSatSystem,
						 Rinex3_0_MaskString::szVerType);
		fprintf(pNavfile,"%20s%20s%20s%20s\n",
			             m_header.szProName,
						 m_header.szAgencyName,
			             m_header.szFileDate,
						 Rinex3_0_MaskString::szPgmRunDate);
		char szValue[14];
		string strValue;
		if(m_header.pdIonAlpha[0] != DBL_MAX)
		{
			if(m_typeSatSystem == 0)
				fprintf(pNavfile,"GPSA ");
			else if(m_typeSatSystem == 1)
				fprintf(pNavfile,"BD2A ");
			else
				fprintf(pNavfile,"%5s"," ");

			sprintf(szValue, "%12.4E", m_header.pdIonAlpha[0]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonAlpha[1]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonAlpha[2]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonAlpha[3]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			fprintf(pNavfile,"%-7s%20s\n", " ", Rinex3_0_MaskString::szIonoCorr);
		}


		/*fprintf(pNavfile,"%-2s%12.3E%12.3E%12.3E%12.3E%-10s%20s\n",
			             " ",
						 m_header.pdIonBeta[0],
						 m_header.pdIonBeta[1],
			             m_header.pdIonBeta[2],
						 m_header.pdIonBeta[3],
						 " ",
						 Rinex3_0_MaskString::szIonBeta);*/
		if(m_header.pdIonBeta[0] != DBL_MAX)
		{
			if(m_typeSatSystem == 0)
				fprintf(pNavfile,"GPSB ");
			else if(m_typeSatSystem == 1)
				fprintf(pNavfile,"BD2B ");
			else
				fprintf(pNavfile,"%5s"," ");		

			sprintf(szValue, "%12.4E", m_header.pdIonBeta[0]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonBeta[1]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonBeta[2]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			sprintf(szValue, "%12.4E", m_header.pdIonBeta[3]);
			stringEraseFloatZero(szValue, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%12s", strValue.c_str());

			fprintf(pNavfile,"%-7s%20s\n", " ", Rinex3_0_MaskString::szIonoCorr);
		}


		/*fprintf(pNavfile,"%-3s%19.11E%19.11E%9d%9d%-1s%20s\n",
			             " ",
						 m_header.deltaUTC.dA0,
						 m_header.deltaUTC.dA1,
			             m_header.deltaUTC.lnT,
						 m_header.deltaUTC.lnW,
						 " ",
						 Rinex3_0_MaskString::szDeltaUTC);*/
		if(m_header.deltaUTC.dA0 != DBL_MAX)
		{
			char szValue2[21];			
			fprintf(pNavfile,"%4s ", m_header.szTimeToUTC);
			sprintf(szValue2, "%17.10E", m_header.deltaUTC.dA0);
			stringEraseFloatZero(szValue2, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%17s", strValue.c_str());

			sprintf(szValue2, "%16.9E", m_header.deltaUTC.dA1);
			stringEraseFloatZero(szValue2, strValue);
			stringRaplaceA2B(strValue, 'E', 'D'); 
			fprintf(pNavfile, "%16s", strValue.c_str());

			fprintf(pNavfile,"%7d%5d%-10s%20s\n",
							 m_header.deltaUTC.lnT,
							 m_header.deltaUTC.lnW,
							 " ",
							 Rinex3_0_MaskString::szTimeSysCorr);
			
		}
		if(m_header.lnLeapSeconds != INT_MAX)
			fprintf(pNavfile,"%6d%-54s%20s\n",
			                 m_header.lnLeapSeconds,
							 " ",
							 Rinex3_0_MaskString::szLeapSec);
		fprintf(pNavfile,"%-60s%20s\n",
			             " ",
						 Rinex3_0_MaskString::szEndOfHead);
		
		// д�۲�����
		for(NavSatMap::iterator it = m_data.begin(); it != m_data.end(); ++it)
		{
			for(size_t s_i = 0; s_i < it->second.size(); s_i++)
			{
				Rinex2_1_NavDatum navDatum = it->second[s_i];
				// PRN / EPOCH / SV CLK
				fprintf(pNavfile,"%1c%02d %4d %02d %02d %02d %02d %02d",
					             m_header.szSatSystem[0],
					             navDatum.bySatPRN,
								 navDatum.tmTimeOfClock.year,
								 navDatum.tmTimeOfClock.month,
								 navDatum.tmTimeOfClock.day,
								 navDatum.tmTimeOfClock.hour,
								 navDatum.tmTimeOfClock.minute,
								 (int)navDatum.tmTimeOfClock.second);
				writedouble(pNavfile, navDatum.dSVClockBias);
				writedouble(pNavfile, navDatum.dSVClockDrift);
				writedouble(pNavfile, navDatum.dSVClockDriftRate);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 1
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.dIODE);
				writedouble(pNavfile, navDatum.dCrs);
				writedouble(pNavfile, navDatum.dDetla_n);
				writedouble(pNavfile, navDatum.dM0);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 2
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.dCuc);
				writedouble(pNavfile, navDatum.dEccentricity);
				writedouble(pNavfile, navDatum.dCus);
				writedouble(pNavfile, navDatum.dSqrt_A);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 3
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.dTOE);
				writedouble(pNavfile, navDatum.dCic);
				writedouble(pNavfile, navDatum.dOMEGA);
				writedouble(pNavfile, navDatum.dCis);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 4
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.d_i0);
				writedouble(pNavfile, navDatum.dCrc);
				writedouble(pNavfile, navDatum.d_omega);
				writedouble(pNavfile, navDatum.dOMEGADOT);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 5
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.didot);
				writedouble(pNavfile, navDatum.dCodesOnL2Channel);
				writedouble(pNavfile, navDatum.dWeek);
				writedouble(pNavfile, navDatum.dL2PDataFlag);
				fprintf(pNavfile, "\n");
				// BROADCAST ORBIT - 6
				fprintf(pNavfile, "%-4s", " ");
				writedouble(pNavfile, navDatum.dSVAccuracy);
				writedouble(pNavfile, navDatum.dSVHealth);
				writedouble(pNavfile, navDatum.dTGD);
				writedouble(pNavfile, navDatum.dIODC);
				fprintf(pNavfile,"\n");
				// BROADCAST ORBIT - 7
				fprintf(pNavfile,"%-4s"," ");
				writedouble(pNavfile, navDatum.dTransmissionTimeOfMessage);
				writedouble(pNavfile, navDatum.dFitInterval);
				writedouble(pNavfile, navDatum.dSpare1);
				writedouble(pNavfile, navDatum.dSpare2);
				fprintf(pNavfile,"\n");
			}
		}
		fclose(pNavfile);
		return true;
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����T, ��õ� nPRN �� GPS ���ǵĹ㲥������Ԫ(���ڽ�����)
	// �������ͣ�T                   : GPSʱ��
	//           nPRN                : GPS���ǵ�PRN��
	//           navDatum            : ��nPRN��GPS���ǵĵ����㲥����
	//           threshold_span_max  : �㲥�����������ֵ, �����������ֵ, ���Ӧ����������������Ч
	// ���룺t, nPRN, threshold_span_max
	// �����navDatum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2013/6/14
	// �汾ʱ�䣺2013/6/14
	// �޸ļ�¼��
	// ��ע�� ʱ�䲻ʹ��������ʽ
	bool Rinex3_0_NavFile::getEphemeris(DayTime T, int nPRN, Rinex2_1_NavDatum& navDatum, double threshold_span_max)
	{		
		NavSatMap::const_iterator it;
		size_t nCount;
		if((it = m_data.find(nPRN)) != m_data.end())
		{
			nCount = it->second.size();
			if(nCount <= 0)
				return false;
		}
		else
			return false;

		int index = -1;
		// ʱ�����ٽ�����, �ο�ʱ��Ϊ����ο���Ԫ TOE 
		double dSpanSeconds = 100000000;
		for(size_t s_i = 0; s_i < nCount; s_i++)
		{
			Rinex2_1_NavDatum navDatum_i   = it->second[s_i];			
			double dSpanSeconds_i = T - navDatum_i.tmTimeOfClock;
			if( fabs(dSpanSeconds_i) < dSpanSeconds ) // 2008/08/07, Ϊ��ֹ�㲥�����޷������ʼʱ����ǰ������
			{// ��֤����������ʱ���ڹ۲�ʱ��֮ǰ
				index = int(s_i);
				dSpanSeconds = fabs(dSpanSeconds_i);
			}
		}
		// �����������ֵ�����Ӧ����������������Ч
		if(index != -1 && dSpanSeconds <= threshold_span_max)
		{
			navDatum = it->second[index];
			return true;
		}
		else
		{
			return false;
		}
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����T, ��õ�nPRN��GPS���ǵĹ㲥����(λ�á��Ӳ�)
	// �������ͣ�T           : GPSʱ��
	//           nPRN        : GPS���ǵ�PRN��
	//           pos         : GPS��λ�ú��Ӳ�
	// ���룺t, nPRN
	// �����NavElement
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼��
	// ��ע��ʱ�䲻ʹ��������ʽ
	bool Rinex3_0_NavFile::getEphemeris(DayTime T, int nPRN, POSCLK &posclk)
	{
		Rinex2_1_NavDatum navDatum;
		if(getEphemeris(T, nPRN, navDatum))
		{// ���� GPS ���Ǵ�����Ч����, ��������λ�ú��Ӳ�
			posclk = getPosition(T, navDatum);
			return true;
		}
		else
			return false;
	}

	// �ӳ������ƣ� getClock   
	// ���ܣ�����GPS���ǹ㲥����, ���ĳ��GPS���ǵ��Ӳ����ֵ
	// �������ͣ�T           : �ο���Ԫʱ��
	//           navDatum    : ĳ��GPS���ǵĵ����㲥����
	//           dClock      : �Ӳ����ֵ(��)
	// ���룺T, NavElement
	// �����dClock
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼��
	// ��ע��
	double Rinex3_0_NavFile::getClock(DayTime T, Rinex2_1_NavDatum navDatum)
	{
		// �˴�������һ�������ЧӦ������, ��Ϊ��λ�����ʱ���Ѿ����п���
		//WeekTime T_WT;
		// �жϱ���/GPSϵͳ���
		//if(m_typeSatSystem == 1) // 2012/04/24
		//	T_WT = TimeCoordConvert::BDT2WeekTime(T);
		//else
		//	T_WT = TimeCoordConvert::GPST2WeekTime(T);
		//double tk = T_WT - navDatum.getWeekTime_toe();
		double tk = T - navDatum.tmTimeOfClock; // 20160516, �ϱ��޸�
		double dClock = navDatum.dSVClockBias + navDatum.dSVClockDrift * tk + navDatum.dSVClockDriftRate * pow(tk,2);
		return dClock;
	}

	// �ӳ������ƣ� getPosition   
	// ���ܣ����� GPS ���ǹ㲥���������ĳ�� GPS ���ǵĹ��λ�ú��Ӳ�
	// �������ͣ�T           : �ο���Ԫʱ��
	//           navDatum    : ĳ��GPS���ǵĵ����㲥����
	//           pos         : GPS���ǵĹ��λ��(��)���Ӳ�(��)
	// ���룺t, navDatum
	// �����pos
	// ���ԣ�C++
    // �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��ʱ�䲻ʹ��������ʽ
	POSCLK Rinex3_0_NavFile::getPosition(DayTime T, Rinex2_1_NavDatum navDatum)
	{
		// ��һ��: �����������еĽ��ٶ�
		//const double GM_EARTH_WGS84 = 3.986005E+14; // 2009/11/03 �޸�
		double n0 = sqrt(m_GM_EARTH) / (pow(navDatum.dSqrt_A, 3));
		double n  = n0 + navDatum.dDetla_n;
		//�ڶ���: ����黯ʱ��
		// ���GPS����
		WeekTime T_WT;
		// �жϱ���/GPSϵͳ���
		//if(m_typeSatSystem == 1) // 2012/04/24
		//	T_WT = TimeCoordConvert::BDT2WeekTime(T);
		//else
			T_WT = TimeCoordConvert::GPST2WeekTime(T);

		double tk = T_WT - navDatum.getWeekTime_toe(); // 20160516, �ϱ��޸�
		//double tk = T - navDatum.tmTimeOfClock;
		/*if( tk > 302400)
			tk -= 604800;
		else if( tk <- 302400 )
			tk += 604800;*/
		//������: ����۲�˲ʱ������ƽ�����
		double Mk = navDatum.dM0 + n * tk;
		//���Ĳ�: ����ƫ����ǣ����ȣ�
		double Ek   = Mk;
		double Ek_1 = Mk - 1;
		while( fabs(Ek - Ek_1) > 1.0E-10 )
		{
			Ek_1 = Ek;
			Ek   = Mk + navDatum.dEccentricity * sin(Ek);
		}
		//���岽: ���������ǣ����ȣ�
		double fk = atan2( sqrt(1 - pow(navDatum.dEccentricity, 2)) * sin(Ek),
						   cos(Ek) - navDatum.dEccentricity );
		//������: ����������Ǿ�
		double faik = fk + navDatum.d_omega;
		//���߲�: �����㶯������
		double cos2faik = cos(2 * faik);
		double sin2faik = sin(2 * faik);
		double detla_u = navDatum.dCuc * cos2faik + navDatum.dCus * sin2faik;
		double detla_r = navDatum.dCrc * cos2faik + navDatum.dCrs * sin2faik;
		double detla_i = navDatum.dCic * cos2faik + navDatum.dCis * sin2faik;
		//�ڰ˲�: ���㾭���㶯�������������uk�����Ǿ�ʸrk���͹�����ik
		double uk = faik + detla_u;
		double rk = pow(navDatum.dSqrt_A, 2) * (1 - navDatum.dEccentricity * cos(Ek)) + detla_r;
		double ik = navDatum.d_i0 + detla_i + navDatum.didot * tk;
		//�ھŲ�: ���������ڹ��ƽ���ϵ�λ��
		double xk = rk * cos(uk);
		double yk = rk * sin(uk);
		//��ʮ��: ����۲�ʱ�̵������㾭��
		//const double EARTH_W_WGS84 = 7.2921151467E-5;
		//double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W) * tk - m_EARTH_W * navDatum.dTOE;
		//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��
		POSCLK pos;
		if(m_typeSatSystem == 1 && navDatum.bySatPRN <= 5)
		{
			Matrix  POS,new_POS,RX,RZ;
			double  theta_E = m_EARTH_W * tk;
			double  theta_R = -5*PI/180;
			POS.Init(3,1);
			new_POS.Init(3,1);
			RX.Init(3,3);
			RZ.Init(3,3);
			double OMEGAk = navDatum.dOMEGA + navDatum.dOMEGADOT * tk - m_EARTH_W * navDatum.dTOE;
			//��ʮһ��: ���������ڹ���ϵ�е�λ��
			POS.SetElement(0,0,xk * cos(OMEGAk) - yk * cos(ik) * sin(OMEGAk));
			POS.SetElement(1,0,xk * sin(OMEGAk) + yk * cos(ik) * cos(OMEGAk));
			POS.SetElement(2,0,yk * sin(ik));
			RX.SetElement(0,0,1);
			RX.SetElement(1,1,cos(theta_R));
			RX.SetElement(1,2,sin(theta_R));
			RX.SetElement(2,1,-sin(theta_R));
			RX.SetElement(2,2,cos(theta_R));
			RZ.SetElement(2,2,1);
			RZ.SetElement(0,0,cos(theta_E));
			RZ.SetElement(0,1,sin(theta_E));
			RZ.SetElement(1,0,-sin(theta_E));
			RZ.SetElement(1,1,cos(theta_E));
			new_POS = RZ * RX * POS;
			pos.x = new_POS.GetElement(0,0);
			pos.y = new_POS.GetElement(1,0);
			pos.z = new_POS.GetElement(2,0);
		}
		else
		{
			double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W) * tk - m_EARTH_W * navDatum.dTOE;
			//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��		
			pos.x = xk * cos(OMEGAk) - yk * cos(ik) * sin(OMEGAk);
			pos.y = xk * sin(OMEGAk) + yk * cos(ik) * cos(OMEGAk);
			pos.z = yk * sin(ik);
		}		
		pos.clk = getClock(T, navDatum);
		return pos;
	}

	// �ӳ������ƣ� exportSP3File   
	// ���ܣ����� sp3 ��ʽ�����ļ�
	// �������ͣ�strSP3fileName  : �ļ�����
	//           T_Begin         : ������ʼʱ��
	//           T_End           : ��������ʱ��
	//           spanSeconds     : ��������ʱ����ʱ����, Ĭ��5����
	// ���룺T_Begin, T_End, spanSeconds
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2014/03/23 
	// �޸ļ�¼��1. 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
	// ��ע��
	bool Rinex3_0_NavFile::exportSP3File(string strSP3fileName, DayTime T_Begin, DayTime T_End, double spanSeconds)
	{
		SP3File sp3file;
		BYTE pbySatList[MAX_PRN_GPS]; // �����б�
		for(int i = 0; i < MAX_PRN_GPS; i++)
			pbySatList[i] = 0;

		DayTime T = T_Begin; // ������BDTʱ��
		int k = 0;
		while( T_End - T > 0 )
		{
			SP3Epoch sp3Epoch;
			sp3Epoch.t = T;
			sp3Epoch.sp3.clear();
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				POSCLK pos;
				if(getEphemeris(T, i, pos))
				{
					pbySatList[i] = 1;
					SP3Datum sp3Datum;
					sp3Datum.pos.x  = pos.x * 0.001;     // �������
					sp3Datum.pos.y  = pos.y * 0.001;     // �������
					sp3Datum.pos.z  = pos.z * 0.001;     // �������
					sp3Datum.clk    = pos.clk * 1.0E+06; // ����΢��
					char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
					if(m_typeSatSystem == 1)
						sprintf(szSatName, "C%02d", i);
					else
                        sprintf(szSatName, "G%02d", i);
					szSatName[3] = '\0';
					sp3Epoch.sp3.insert(SP3SatMap::value_type(szSatName, sp3Datum));
				}
			}
			sp3file.m_data.push_back(sp3Epoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		// Line 1
		sprintf(sp3file.m_header.szSP3Version, "#a");
		sprintf(sp3file.m_header.szPosVelFlag, "P");
		sp3file.m_header.tmStart = T_Begin;
		sp3file.m_header.nNumberofEpochs = 96;
		sprintf(sp3file.m_header.szDataType, "ORBIT");
		sprintf(sp3file.m_header.szCoordinateSys, "IGS00");
		sprintf(sp3file.m_header.szOrbitType, "HLM");
		sprintf(sp3file.m_header.szAgency, "IGS");
		// Line 2
		sprintf(sp3file.m_header.szLine2Symbols, "##");
		// �жϱ���/GPSϵͳ���
		if(m_typeSatSystem == 1) // 2012/05/23
			sp3file.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(TimeCoordConvert::BDT2GPST(T_Begin));
		else
			sp3file.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(T_Begin);

		sp3file.m_header.dEpochInterval = spanSeconds;
		double dMJD = TimeCoordConvert::DayTime2MJD(T_Begin);
		sp3file.m_header.nModJulDaySt = long(floor(dMJD));    
		sp3file.m_header.dFractionalDay = dMJD - sp3file.m_header.nModJulDaySt;
		// Line 3
		sprintf(sp3file.m_header.szLine3Symbols, "+ ");
		// �ۺ�ͳ�ƿ��������б�
		sp3file.m_header.pstrSatNameList.clear(); // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
		sp3file.m_header.pbySatAccuracyList.clear();
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
				if(m_typeSatSystem == 1)
					sprintf(szSatName, "C%02d", i);
				else
                    sprintf(szSatName, "G%02d", i);
				szSatName[3] = '\0';
				sp3file.m_header.pstrSatNameList.push_back(szSatName);
				sp3file.m_header.pbySatAccuracyList.push_back(3);
			}
		}
		sp3file.m_header.bNumberofSats = BYTE(sp3file.m_header.pstrSatNameList.size());
		// Line 8--12
		sprintf(sp3file.m_header.szLine8Symbols, "++");
		// Line 13-14
		sprintf(sp3file.m_header.szLine13Symbols, "%%c");
		if(m_typeSatSystem == 1)
		{
			sprintf(sp3file.m_header.szFileType, "C");
			sprintf(sp3file.m_header.szTimeSystem, "BDS");
		}
		else
		{
			sprintf(sp3file.m_header.szFileType, "G");
			sprintf(sp3file.m_header.szTimeSystem, "GPS");
		}
		// Line 15-16
		sprintf(sp3file.m_header.szLine15Symbols, "%%f");
		sp3file.m_header.dBaseforPosVel  = 0;
		sp3file.m_header.dBaseforClkRate = 0;
		// Line 17-18
		sprintf(sp3file.m_header.szLine17Symbols, "%%i");
		// Line 19-22
		sprintf(sp3file.m_header.szLine19Symbols, "/*");
		//sp3file.write(strSP3fileName);
		if(sp3file.m_header.bNumberofSats <= 0)
			return false;
		else
		{
			sp3file.write(strSP3fileName);
			return true;
		}
	}

	// �ӳ������ƣ� exportCLKFile   
	// ���ã�����clk��ʽ�Ӳ��ļ�
	// ���ͣ�strCLKfileName  : �ļ�����
	//       T_Begin         : ������ʼʱ��
	//       T_End           : ��������ʱ��
	//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
	// ���룺T_Begin, T_End, dSpanSeconds
	// �����
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool Rinex3_0_NavFile::exportCLKFile(string strCLKfileName, DayTime T_Begin, DayTime T_End, double spanSeconds)
	{
		// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���			
		CLKFile                 clkfile;    // �����Ӳ��ļ�	
		BYTE pbySatList[MAX_PRN_GPS];       // �����б�
		for(int i = 0; i < MAX_PRN_GPS; i++)
			pbySatList[i] = 0;		
		int k = 0;
		DayTime T = T_Begin;
		while( T_End - T >= 0 )
		{
			CLKEpoch clkEpoch;				
			clkEpoch.t = T;
			clkEpoch.ARList.clear();
			clkEpoch.ASList.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				POSCLK posclk;
				if(getEphemeris(T, i, posclk))
				{
					pbySatList[i] = 1;
					CLKDatum   ASDatum;
					ASDatum.count = 2;
					char  satname[4];
					if(m_typeSatSystem == 0)     //GPS
						sprintf(satname,"G%2d",i);
					else
						sprintf(satname,"C%2d",i);//����
					ASDatum.name = satname;
					//satname[3] = '\0';
					ASDatum.clkBias = posclk.clk;
					ASDatum.clkBiasSigma = 0;
					clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));
				}
			}
			clkfile.m_data.push_back(clkEpoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		sprintf(clkfile.m_header.szRinexVersion, "2.0");
		clkfile.m_header.cFileType = 'C';
		sprintf(clkfile.m_header.szProgramName,"NUDTTK");
		sprintf(clkfile.m_header.szAgencyName,"NUDT");
		clkfile.m_header.LeapSecond = 0;
		clkfile.m_header.ClockDataTypeCount = 1;
		clkfile.m_header.pstrClockDataTypeList.clear();
		clkfile.m_header.pstrClockDataTypeList.push_back("AS");
		sprintf(clkfile.m_header.szACShortName,"NUDT");
		clkfile.m_header.nStaCount = 0;
		sprintf(clkfile.m_header.szStaCoordFrame,"IGS00 : IGS REALIZATION OF THE ITRF2000");
		// �ۺ�ͳ�ƿ��������б�
		clkfile.m_header.pszSatList.clear();
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				char szPRN[4];
				if(m_typeSatSystem == 0) //GPS
					sprintf(szPRN, "G%2d", i);
				else
					sprintf(szPRN, "C%2d", i);;//����				
				clkfile.m_header.pszSatList.push_back(szPRN);
			}
		}
		clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());
		//clkfile.write(strCLKfileName);	
		if(clkfile.m_header.bySatCount <= 0)
			return false;
		else
		{
			clkfile.write(strCLKfileName);		
			return true;
		}
	}
	// �ӳ������ƣ� exportSP3File_GPST   
	// ���ܣ����� GPSʱ��ϵͳ�ı���sp3 ��ʽ�����ļ�
	// �������ͣ�strSP3fileName  : �ļ�����
	//           T_Begin         : ������ʼʱ��
	//           T_End           : ��������ʱ��
	//           spanSeconds     : ��������ʱ����ʱ����, Ĭ��5����
	// ���룺T_Begin, T_End, spanSeconds
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool Rinex3_0_NavFile::exportSP3File_GPST(string strSP3fileName, GPST T_Begin, GPST T_End, double spanSeconds)
	{
		SP3File sp3file;
		BYTE pbySatList[MAX_PRN_GPS]; // �����б�
		for(int i = 0; i < MAX_PRN_GPS; i++)
			pbySatList[i] = 0;

		GPST T = T_Begin; // ������BDTʱ��
		int k = 0;
		while( T_End - T > 0 )
		{
			SP3Epoch sp3Epoch;
			sp3Epoch.t = T;
			sp3Epoch.sp3.clear();
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				POSCLK pos;
				if(getEphemeris(TimeCoordConvert::GPST2BDT(T), i, pos))
				{
					pbySatList[i] = 1;
					SP3Datum sp3Datum;
					sp3Datum.pos.x  = pos.x * 0.001;     // �������
					sp3Datum.pos.y  = pos.y * 0.001;     // �������
					sp3Datum.pos.z  = pos.z * 0.001;     // �������
					sp3Datum.clk    = pos.clk * 1.0E+06; // ����΢��
					char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
					if(m_typeSatSystem == 1)
						sprintf(szSatName, "C%02d ", i);
					else
						sprintf(szSatName, "G%02d ", i);
					szSatName[3] = '\0';
					sp3file.m_header.pstrSatNameList.push_back(szSatName);
					sp3Epoch.sp3.insert(SP3SatMap::value_type(szSatName, sp3Datum));
				}
			}
			sp3file.m_data.push_back(sp3Epoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		// Line 1
		sprintf(sp3file.m_header.szSP3Version, "#a");
		sprintf(sp3file.m_header.szPosVelFlag, "P");
		sp3file.m_header.tmStart = sp3file.m_data.front().t;		
		sp3file.m_header.nNumberofEpochs =  96;
		sprintf(sp3file.m_header.szDataType, "ORBIT");
		sprintf(sp3file.m_header.szCoordinateSys, "IGS00");
		sprintf(sp3file.m_header.szOrbitType, "HLM");
		sprintf(sp3file.m_header.szAgency, "IGS");
		// Line 2
		sprintf(sp3file.m_header.szLine2Symbols, "##");
		// �жϱ���/GPSϵͳ���
		//if(m_typeSatSystem == 1) // 2012/05/23
		//	sp3file.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(TimeCoordConvert::BDT2GPST(T_Begin));
		//else
	    sp3file.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(T_Begin);

		sp3file.m_header.dEpochInterval = spanSeconds;
		double dMJD = TimeCoordConvert::DayTime2MJD(T_Begin);
		sp3file.m_header.nModJulDaySt = long(floor(dMJD));    
		sp3file.m_header.dFractionalDay = dMJD - sp3file.m_header.nModJulDaySt;
		// Line 3
		sprintf(sp3file.m_header.szLine3Symbols, "+ ");
		// �ۺ�ͳ�ƿ��������б�
		sp3file.m_header.pstrSatNameList.clear(); // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
		sp3file.m_header.pbySatAccuracyList.clear();
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
				if(m_typeSatSystem == 1)
					sprintf(szSatName, "C%02d ", i);
				else
                    sprintf(szSatName, "G%02d ", i);
				szSatName[3] = '\0';
				sp3file.m_header.pstrSatNameList.push_back(szSatName);
				sp3file.m_header.pbySatAccuracyList.push_back(3);
			}
		}
		sp3file.m_header.bNumberofSats = BYTE(sp3file.m_header.pstrSatNameList.size());
		// Line 8--12
		sprintf(sp3file.m_header.szLine8Symbols, "++");
		// Line 13-14
		sprintf(sp3file.m_header.szLine13Symbols, "%%c");		
		sprintf(sp3file.m_header.szFileType, "C");
		sprintf(sp3file.m_header.szTimeSystem, "GPS");	
		// Line 15-16
		sprintf(sp3file.m_header.szLine15Symbols, "%%f");
		sp3file.m_header.dBaseforPosVel  = 0;
		sp3file.m_header.dBaseforClkRate = 0;
		// Line 17-18
		sprintf(sp3file.m_header.szLine17Symbols, "%%i");
		// Line 19-22
		sprintf(sp3file.m_header.szLine19Symbols, "/*");
		if(sp3file.m_header.bNumberofSats <= 0)
			return false;
		else
		{
			sp3file.write(strSP3fileName);
			return true;
		}
	}
	// �ӳ������ƣ� exportCLKFile_GPST   
	// ���ã�����clk��ʽ�Ӳ��ļ�
	// ���ͣ�strCLKfileName  : �ļ�����
	//       T_Begin         : ������ʼʱ��
	//       T_End           : ��������ʱ��
	//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
	// ���룺T_Begin, T_End, dSpanSeconds
	// �����
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool Rinex3_0_NavFile::exportCLKFile_GPST(string strCLKfileName, GPST T_Begin, GPST T_End, double spanSeconds)
	{
		// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���			
		CLKFile                 clkfile;    // �����Ӳ��ļ�	
		BYTE pbySatList[MAX_PRN_GPS];       // �����б�
		for(int i = 0; i < MAX_PRN_GPS; i++)
			pbySatList[i] = 0;		
		int k = 0;
		DayTime T = T_Begin;
		while( T_End - T >= 0 )
		{
			CLKEpoch clkEpoch;				
			clkEpoch.t = T;
			clkEpoch.ARList.clear();
			clkEpoch.ASList.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				POSCLK posclk;
				if(getEphemeris(TimeCoordConvert::GPST2BDT(T), i, posclk))
				{
					pbySatList[i] = 1;
					CLKDatum   ASDatum;
					ASDatum.count = 2;
					char  satname[4];					
					sprintf(satname,"C%2d",i);					
					ASDatum.name = satname;
					//satname[3] = '\0';
					ASDatum.clkBias = posclk.clk;
					ASDatum.clkBiasSigma = 0;
					clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));
				}
			}
			clkfile.m_data.push_back(clkEpoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		sprintf(clkfile.m_header.szRinexVersion, "2.0");
		clkfile.m_header.cFileType = 'C';
		sprintf(clkfile.m_header.szProgramName,"NUDTTK");
		sprintf(clkfile.m_header.szAgencyName,"NUDT");
		clkfile.m_header.LeapSecond = 0;
		clkfile.m_header.ClockDataTypeCount = 1;
		clkfile.m_header.pstrClockDataTypeList.clear();
		clkfile.m_header.pstrClockDataTypeList.push_back("AS");
		sprintf(clkfile.m_header.szACShortName,"NUDT");
		clkfile.m_header.nStaCount = 0;
		sprintf(clkfile.m_header.szStaCoordFrame,"IGS00 : IGS REALIZATION OF THE ITRF2000");
		// �ۺ�ͳ�ƿ��������б�
		clkfile.m_header.pszSatList.clear();
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				char szPRN[4];				
				sprintf(szPRN, "C%2d", i);;//����				
				clkfile.m_header.pszSatList.push_back(szPRN);
			}
		}
		clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());
		if(clkfile.m_header.bySatCount <= 0)
			return false;
		else
		{
			clkfile.write(strCLKfileName);		
			return true;
		}
	}
}
