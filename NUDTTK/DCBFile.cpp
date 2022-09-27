#include "DCBFile.hpp"
#include <math.h>

namespace NUDTTK
{
	DCBFile::DCBFile(void)
	{
	}
	DCBFile::~DCBFile(void)
	{
	}
	void DCBFile::clear()
	{
		m_headP1C1 = DCBFileHeader::DCBFileHeader();
		m_dataP1C1.clear();
		m_headP1P2 = DCBFileHeader::DCBFileHeader();
		m_dataP1P2.clear();
	}

	bool DCBFile::isValidEpochLine(string strLine, DCBDatum& datum)
	{
		bool nFlag = true;
		char szName[4];
		char szStationName[17];
		double value = 0;
		double rms = 0;
		//***   ****************    *****.***   *****.***
		sscanf(strLine.c_str(), "%3c%*3c%16c%*4c%9lf%*3c%9lf",
			                    szName,
	                            szStationName,
							    &value,
							    &rms);
		szName[3] = '\0';
		szStationName[16] = '\0';

	    if(szName[0] == 'G'
		|| szName[0] == 'R'
		|| szName[0] == 'C'
		|| value != 0)
		{
			datum.name  = szName;
            datum.value = value;
            datum.rms = rms;
			nFlag = true;
		}
		else
			nFlag = false;
		return nFlag;
	}


	// �ӳ������ƣ� open   
	// ���ܣ���ȡ DCB �ļ�����
	// �������ͣ�P1C1FileName: DCB�ļ�����
	//           P1P2FileName: DCB�ļ�����
	// ���룺P1C1FileName, P1P2FileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��, �ȵ·�
	// ����ʱ�䣺2013/11/16
	// �汾ʱ�䣺2013/11/15
	// �޸ļ�¼��
	// ��ע�� 
	bool DCBFile::open(string  P1C1FileName, string  P1P2FileName)
	{
		// P1C1
		clear();
		FILE * pDCBFile = fopen(P1C1FileName.c_str(), "r+t");
		if(pDCBFile == NULL) 
			return false;
		char line[200];
		// Line 1
		fgets(line, 200, pDCBFile); 
		sprintf(m_headP1C1.szAgencyObsTypeTime, "%80s", line);
		m_headP1C1.szAgencyObsTypeTime[80] = '\0';
		// Line 2
		fgets(line, 200, pDCBFile); 
		sprintf(m_headP1C1.szline, "%80s", line);
		m_headP1C1.szline[80] = '\0';
		// Line 4
		fgets(line, 200, pDCBFile); 
		fgets(line, 200, pDCBFile); 
		sprintf(m_headP1C1.szComments, "%62s", line);
		m_headP1C1.szComments[62] = '\0';
		// Line 6
		fgets(line, 200, pDCBFile); 
		fgets(line, 200, pDCBFile); 
		sprintf(m_headP1C1.szPrnValueRms, "%46s", line);
		m_headP1C1.szPrnValueRms[46] = '\0';
		// Line 7
		fgets(line, 200, pDCBFile); 
		sprintf(m_headP1C1.szFormatMarker, "%47s", line);
		m_headP1C1.szFormatMarker[47] = '\0';
        //m_dataP1C1.clear();
		while(!feof(pDCBFile))
		{
			if(fgets(line, 200, pDCBFile))
			{
				DCBDatum datum;
				if(isValidEpochLine(line, datum))
				{
					m_dataP1C1.insert(SatDCBCorrectMap::value_type(datum.name, datum));
				}
			}
		}
		fclose(pDCBFile);
        // P1P2
		FILE * pDCBFile2 = fopen(P1P2FileName.c_str(), "r+t");
		if(pDCBFile2 == NULL) 
			return false;
		// Line 1
		fgets(line, 200, pDCBFile2); 
		sprintf(m_headP1P2.szAgencyObsTypeTime, "%80s", line);
		m_headP1P2.szAgencyObsTypeTime[80] = '\0';
		// Line 2
		fgets(line, 200, pDCBFile2); 
		sprintf(m_headP1P2.szline, "%80s", line);
		m_headP1P2.szline[80] = '\0';
		// Line 4
		fgets(line, 200, pDCBFile2); 
		fgets(line, 200, pDCBFile2); 
		sprintf(m_headP1P2.szComments, "%62s", line);
		m_headP1P2.szComments[62] = '\0';
		// Line 6
		fgets(line, 200, pDCBFile2); 
		fgets(line, 200, pDCBFile2); 
		sprintf(m_headP1P2.szPrnValueRms, "%46s", line);
		m_headP1P2.szPrnValueRms[46] = '\0';
		// Line 7
		fgets(line, 200, pDCBFile2); 
		sprintf(m_headP1P2.szFormatMarker, "%47s", line);
		m_headP1P2.szFormatMarker[47] = '\0';
        //m_dataP1P2.clear();
		while(!feof(pDCBFile2))
		{
			if(fgets(line, 200, pDCBFile2))
			{
				DCBDatum datum;
				if(isValidEpochLine(line, datum))
				{
					m_dataP1P2.insert(SatDCBCorrectMap::value_type(datum.name, datum));
				}
			}
		}
		fclose(pDCBFile2);
		return true;
	}

	// �ӳ������ƣ� write   
	// ���ܣ���ȡ DCB �ļ�����
	// �������ͣ�P1C1FileName: DCB�ļ�����
	//           P1P2FileName: DCB�ļ�����
	// ���룺P1C1FileName, P1P2FileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/11/16
	// �汾ʱ�䣺2013/11/15
	// �޸ļ�¼��
	// ��ע�� 
	bool DCBFile::write(string  P1C1FileName, string  P1P2FileName)
	{
		return true;
	}

	// �ӳ������ƣ� getDCBCorrectValue   
	// ���ܣ���ȡ�����������ݽṹ
	// �������ͣ�satName��  ��������
	//			 dcb_P1 ��  DCB����ֵ(��λ����)
	//			 dcb_P2 ��  DCB����ֵ(��λ����)
	//           RecType��  ���ջ�����, 'P'��'C'��'N'
	/*
				 'P':  receiver is cross-correlating and requires correction of P2' and C1 Rogue SNR, Trimble 4000, etc.
				 'C':  receiver is non-cross-correlating but reports C1 instead of P1 Trimble 4700, 5700, Leica RS500, CRS1000, SR9600, etc. unless AS is off
				 'N':  receiver is non-cross-correlating and reports true P1, P2 
	*/
	// ���ջ����Ͷ�Ӧ����
	// P���ͣ�C1/X2=C1 + (P2 - P1)
	// C���ͣ�C1/P2
	// N���ͣ�P1/P2
	// ���룺satName, RecType
	// �����dcb_P1, dcb_P2
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/11/16
	// �汾ʱ�䣺2013/11/15
	// �޸ļ�¼��
	// ��ע�� 
	bool DCBFile::getDCBCorrectValue(string satName, double &dcb_P1, double &dcb_P2, char RecType)
	{
		dcb_P1 = 0;
		dcb_P2 = 0;
		SatDCBCorrectMap::iterator it_P1C1 = m_dataP1C1.find(satName);
		SatDCBCorrectMap::iterator it_P1P2 = m_dataP1P2.find(satName);
		if(RecType == 'C')
		{
			if(it_P1C1 != m_dataP1C1.end() && it_P1P2 != m_dataP1P2.end())
			{
				dcb_P1 = it_P1C1->second.value * 1.0E-9 * SPEED_LIGHT + pow(GPS_FREQUENCE_L2, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
				dcb_P2 = pow(GPS_FREQUENCE_L1, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
			}
		}
		if(RecType == 'N')
		{
			if(it_P1P2 != m_dataP1P2.end())
			{
				dcb_P1 = pow(GPS_FREQUENCE_L2, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
				dcb_P2 = pow(GPS_FREQUENCE_L1, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
			}
		}
		if(RecType == 'P')
		{
			if(it_P1C1 != m_dataP1C1.end() && it_P1P2 != m_dataP1P2.end())
			{
				dcb_P1 = it_P1C1->second.value * 1.0E-9 * SPEED_LIGHT + pow(GPS_FREQUENCE_L2, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
				dcb_P2 = it_P1C1->second.value * 1.0E-9 * SPEED_LIGHT + pow(GPS_FREQUENCE_L1, 2) / (pow(GPS_FREQUENCE_L1, 2) - pow(GPS_FREQUENCE_L2, 2)) * it_P1P2->second.value * 1.0E-9 * SPEED_LIGHT;
			}
		}
		return true;
	}
}