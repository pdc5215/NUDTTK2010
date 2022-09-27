#include "cstgSLRObsFile.hpp"
#include <math.h>

namespace NUDTTK
{
	namespace SLR
	{
		cstgSLRObsFile::cstgSLRObsFile(void)
		{
			m_bChecksum = true;
		}

		cstgSLRObsFile::~cstgSLRObsFile(void)
		{
		}

		UTC cstgSinglePassArc::getTime(cstgDataRecord Record)
		{
			int nB4Year = yearB2toB4(normalHeaderRecord.nYear);
			UTC t(nB4Year, 1, 1, 0, 0, 0);
			t = t + (normalHeaderRecord.nDay - 1) * 86400 ;    // �������(nDay�Ƿ�� 1 ��ʼ���� ?)
			t = t + Record.TimeofDay * 1.0E-7;                 // �ټ�����Ĳ���
			return t;
		}

		void cstgSLRObsFile::deleteStringZero(char* strSrc)
		{
			string str = strSrc;
			for(size_t i = 0; i < str.length(); i++)
			{
				if(strSrc[i] == '0')
					strSrc[i] = ' ';
				else
					return;
			}
		}

		// Ѱ���»��ε����:  0 - EOF; 1 - �»��ο�ʼ; 2 - ԭʼ���ݼ�¼��ʼ; 3 - ����
		int  cstgSLRObsFile::isValidNewPass(string strLine, FILE * pCSTGFile)
		{
			if(pCSTGFile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
			{
				if(feof(pCSTGFile))
					return 0;
			}
			if(strLine.length() < 5)
				return 3;
			char szHead[6];
			strLine.copy(szHead, 5, 0);
			szHead[5] = '\0';
			if(strcmp(szHead,"99999") == 0)
				return 1;
			if(strcmp(szHead,"88888") == 0)
				return 2;
			return 3;
		}

		bool cstgSLRObsFile::readLine_HeaderRecord(string strLine, cstgHeaderRecord& HeaderRecord)
		{
			if(strLine.length() < 55)
				return false;
			cstgHeaderRecord Record;
			char szCalibrationSysDelay[9];   // �����Ǹ���, 2007/12/27
			char szCalibrationDelayShift[7];
			sscanf(strLine.c_str(),"%7c%2d%3d%4d%2d%2d%4lf%8c%6c%4lf%1d%1d%1d%1d%1d%4lf%1d%2d%1d",
									Record.szSatCOSPARID,
								   &Record.nYear,
								   &Record.nDay,
								   &Record.nCDPPadID,
								   &Record.nCDPSysNum,
								   &Record.nCDPOccSeqNum,
								   &Record.Wavelength,
								   szCalibrationSysDelay,
								   szCalibrationDelayShift,
								   &Record.CalibrationDelayRMS,
								   &Record.NormalWindowIndicator,
								   &Record.EpochTimeScale,
								   &Record.SCMIndicator,
								   &Record.SCHIndicator,
								   &Record.SCIndicator,
								   &Record.PassRMS,
								   &Record.DQSIndicator,
								   &Record.nCheckSum,
								   &Record.nFormatIndicator
								);
			Record.szSatCOSPARID[7]    = '\0';
			szCalibrationSysDelay[8]   = '\0';
			szCalibrationDelayShift[6] = '\0';
			deleteStringZero(szCalibrationSysDelay);
			deleteStringZero(szCalibrationDelayShift);
			sscanf(szCalibrationSysDelay,   "%8lf",  &Record.CalibrationSysDelay);
			sscanf(szCalibrationDelayShift, "%6lf",  &Record.CalibrationDelayShift);
			if(Record.Wavelength >= 3000) // �任�� ns
				Record.Wavelength = Record.Wavelength * 0.1;
			// ����У��ͱȶ�
			double dsum = 0;
			for(size_t i = 0; i < 52; i++)
			{
				char szA[2];
				szA[0] = strLine.at(i);
				szA[1] = '\0';
				double dA = atof(szA);
				dsum += dA;
			}
			dsum = fmod(dsum, 100);
			if( int(dsum) != Record.nCheckSum && m_bChecksum)
			{
				//printf("readLine_HeaderRecord У���δͨ������! %s", strLine.c_str());
				return false;
			}
			HeaderRecord = Record;
			return true;
		}

		bool cstgSLRObsFile::readLine_DataRecord(string strLine, cstgDataRecord& DataRecord)
		{
			if(strLine.length() < 55)
				return false;
			cstgDataRecord Record;
			char szTimeofDay[13];
			char szLaserRange[13];
			char szRMS[8];
			char szSurfacePressure[6];
			char szSurfaceTemperature[5];
			char szSurfaceRelHumidity[4];
			char szCompressedRangeNum[5];
			char szDataReleaseIndicate[2];
			char szIntegerSeconds[2];
			char szNormalPointWindow[2];
			char szSNR[3];
			char sznCheckSum[3];
			sscanf(strLine.c_str(),"%12c%12c%7c%5c%4c%3c%4c%1c%1c%1c%2c%2c",
								   szTimeofDay,
								   szLaserRange,
								   szRMS,
								   szSurfacePressure,
								   szSurfaceTemperature,
								   szSurfaceRelHumidity,
								   szCompressedRangeNum,
								   szDataReleaseIndicate,
								   szIntegerSeconds,
								   szNormalPointWindow,
								   szSNR,
								   sznCheckSum
								);
			szTimeofDay[12]          = '\0';
			szLaserRange[12]         = '\0';
			szRMS[7]                 = '\0';
			szSurfacePressure[5]     = '\0';
			szSurfaceTemperature[4]  = '\0';
			szSurfaceRelHumidity[3]  = '\0';
			szCompressedRangeNum[4]  = '\0';
			szDataReleaseIndicate[1] = '\0';
			szIntegerSeconds[1]      = '\0';
			szNormalPointWindow[1]   = '\0';
			szSNR[2]                 = '\0';
			sznCheckSum[2]           = '\0';
			sscanf(szTimeofDay,            "%12lf",  &Record.TimeofDay);
			sscanf(szLaserRange,           "%12lf",  &Record.LaserRange);
			sscanf(szRMS,                  "%7lf",   &Record.RMS);
			sscanf(szSurfacePressure,      "%5lf",   &Record.SurfacePressure);
			sscanf(szSurfaceTemperature,   "%4lf",   &Record.SurfaceTemperature);
			sscanf(szSurfaceRelHumidity,   "%3lf",   &Record.SurfaceRelHumidity);
			sscanf(szCompressedRangeNum,   "%4d",    &Record.CompressedRangeNum);
			sscanf(szDataReleaseIndicate,  "%1d",    &Record.DataReleaseIndicate);
			sscanf(szIntegerSeconds,       "%1d",    &Record.IntegerSeconds);
			sscanf(szNormalPointWindow,    "%1d",    &Record.NormalPointWindow);
			sscanf(szSNR,                  "%2lf",   &Record.SNR);
			sscanf(sznCheckSum,            "%2d",    &Record.nCheckSum);
			// ����У��ͱȶ�
			double dsum = 0;
			for(size_t i = 0; i < 52; i++)
			{
				char szA[2];
				szA[0] = strLine.at(i);
				szA[1] = '\0';
				double dA = atof(szA);
				dsum += dA;
			}
			dsum = fmod(dsum, 100);
			if( int(dsum) != Record.nCheckSum  && m_bChecksum)
			{
				//printf("readLine_DataRecord У���δͨ������! %s", strLine.c_str());
				return false;
			}
			DataRecord = Record;
			return true;
		}

		bool cstgSLRObsFile::open(string strCSTGFileName, bool bAdd)
		{
			FILE * pCSTGFile = fopen(strCSTGFileName.c_str(),"r+t");
			if(pCSTGFile == NULL) 
				return false;
			char szline[200]; 
			if(!bAdd)
				m_data.clear();
			int bFlag = 1;
			// �»��μ�¼
			int bNormal_Raw = 0; // 0 ������¼Normal��; 1 ������¼Raw��
			int bPassAdd = 0;    // ��Ǹû��������Ƿ���Ա����
			cstgSinglePassArc newPass;
			fgets(szline, 200, pCSTGFile);
			int count_add = 0;
			while(bFlag)
			{
				int nLineFlag = isValidNewPass(szline, pCSTGFile);
				if(nLineFlag == 0)
				{
					bFlag = false;
					// 2008/04/04���, �������һ�λ�©��һ������
					// ��һ�����μ�¼���
					if(bPassAdd == 1 && newPass.normalDataRecordList.size() > 0)
					{
						// ��������۲�ʱ�䣬���������ҹ��ϵ����
						for(size_t s_k = 1; s_k < newPass.normalDataRecordList.size(); s_k++)
						{
							if(newPass.normalDataRecordList[s_k].TimeofDay
							 - newPass.normalDataRecordList[s_k-1].TimeofDay < 0)
							{// �� s_k ������������, һ�����ν����ܳ���һ�θ����
								for(size_t s_j = s_k; s_j < newPass.normalDataRecordList.size(); s_j++)
									newPass.normalDataRecordList[s_j].TimeofDay += 86400;
								break;
							}
							//cout<<newPass.getTime(newPass.normalDataRecordList[s_k]).ToString()<<endl;
						}
						m_data.push_back(newPass);
						count_add++;
						//cout<<newPass.normalDataRecordList.size()<<endl;
					}
				}
				else if(nLineFlag == 1)
				{// �»���
					bFlag = true;
					// ��һ�����μ�¼���
					if(bPassAdd == 1 && newPass.normalDataRecordList.size() > 0)
					{
						// ��������۲�ʱ�䣬���������ҹ��ϵ����
						for(size_t s_k = 1; s_k < newPass.normalDataRecordList.size(); s_k++)
						{
							if(newPass.normalDataRecordList[s_k].TimeofDay
							 - newPass.normalDataRecordList[s_k-1].TimeofDay < 0)
							{// �� s_k ������������, һ�����ν����ܳ���һ�θ����
								for(size_t s_j = s_k; s_j < newPass.normalDataRecordList.size(); s_j++)
									newPass.normalDataRecordList[s_j].TimeofDay += 86400;
								break;
							}
							//cout<<newPass.getTime(newPass.normalDataRecordList[s_k]).ToString()<<endl;
						}
						m_data.push_back(newPass);
						count_add++;
						//cout<<newPass.normalDataRecordList.size()<<endl;
					}
					bPassAdd = 1;
					// ���� HeaderRecord �жϻ��������Ƿ���Ч
					bNormal_Raw = 0;
					// ���� NormalHeaderRecord
					fgets(szline, 200, pCSTGFile);
					if(!readLine_HeaderRecord(szline, newPass.normalHeaderRecord))
					{// ��ͨ�����б�ǣ��������ε����ݲ��ܽ������
						bPassAdd = 0;
					}
					// ��� normalDataRecordList �� rawDataRecordList
					newPass.normalDataRecordList.clear();
					newPass.rawDataRecordList.clear();
				}
				else if(nLineFlag == 2)
				{// ԭʼ���ݼ�¼��ʼ
					bFlag = true;
					bNormal_Raw = 1;
					// ���� RawHeaderRecord
					fgets(szline, 200, pCSTGFile);
					if(!readLine_HeaderRecord(szline, newPass.rawHeaderRecord))
					{// ��ͨ�����б��, �������ε����ݲ��ܽ������
						bPassAdd = 0;
					}
				}
				else
				{
					bFlag = true;
					// ���� bNormal_Raw ָʾ, ��¼����
					if(bNormal_Raw == 0)
					{// ������¼Normal��
						cstgDataRecord DataRecord;
						if(readLine_DataRecord(szline, DataRecord))
							newPass.normalDataRecordList.push_back(DataRecord);
					}
					else
					{
					}
				}
				// ������һ��
				fgets(szline, 200, pCSTGFile);
			}
			fclose(pCSTGFile);
			if(m_data.size() > 0 && count_add > 0)
				return true;
			else
				return false;
		}
	}
}
