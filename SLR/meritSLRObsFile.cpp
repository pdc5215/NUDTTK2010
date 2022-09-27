#include "meritSLRObsFile.hpp"
#include "cstgSLRObsFile.hpp"
#include "constDef.hpp"

#include <math.h>

namespace NUDTTK
{
	namespace SLR
	{
		meritSLRObsFile::meritSLRObsFile(void)
		{
		}

		meritSLRObsFile::~meritSLRObsFile(void)
		{
		}

		bool meritSLRObsFile::isValidEpochLine(string strLine, meritDataRecord& meritLine)
		{
			bool nFlag = true;
			meritDataRecord Line;  // meritDataRecord �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
			// �ַ����ĳ���Ҫ >= 130
			if(strLine.length() < 130)
				return false;
			// ����Ϊ������Ӧ�õ����ر�Դ�
			char szYear[3];
			char szDay[4];
			char szTimeofDay[13];
			char szStationID[5];
			char szCDPSysNum[3];
			char szCDPOSNum[3];
			char szAzimuth[8];
			char szElevation[7];
			char szLaserRange[13];
			char szLaserRangeSD[8];
			char szWavelength[5];
			char szSurfacePressure[6];
			char szSurfaceTemperature[5];
			char szSurfaceRelHumidity[4];
			char szTropCorrect[6];
			char szMassCenterCorrect[7];
			char szReceiveAmplitude[6];
			char szCalibrationSD[5];
			char szNormalWindowIndicator[2];
			char szCompressedRangeNum[5];
			char szEpochEvent[2];
			char szEpochTimeScale[2];
			char szAngleOriginIndicator[2];
			char szTRCIndicator[2];
			char szMCCIndicator[2];
			char szRACIndicator[2];
			char szSCMIndicator[2];
			char szCDSIndicator[2];
			char szSCFIndicator[2];
			char szFRNIndicator[2];
			char szCalibrationSysDelay[9];   
			char szCalibrationDelayShift[7];
			sscanf(strLine.c_str(),"%7c%2c%3c%12c%4c%2c%2c%7c%6c%12c%7c%4c%5c%4c%3c%5c%6c%5c%8c%6c%4c%1c%4c%1c%1c%1c%1c%1c%1c%1c%1c%1c%1c%c",
									 Line.szSatCOSPARID,
									 szYear,
									 szDay,
									 szTimeofDay,
									 szStationID,
									 szCDPSysNum,
									 szCDPOSNum,
									 szAzimuth,
									 szElevation,
									 szLaserRange,
									 szLaserRangeSD,
									 szWavelength,
									 szSurfacePressure,
									 szSurfaceTemperature,
									 szSurfaceRelHumidity,
									 szTropCorrect,
									 szMassCenterCorrect,
									 szReceiveAmplitude,
									 szCalibrationSysDelay,
									 szCalibrationDelayShift,
									 szCalibrationSD,
									 szNormalWindowIndicator,
									 szCompressedRangeNum,
									 szEpochEvent,
									 szEpochTimeScale,
									 szAngleOriginIndicator,
									 szTRCIndicator,
									 szMCCIndicator,
									 szRACIndicator,
									 szSCMIndicator,
									 szCDSIndicator,
									 szSCFIndicator,
									 szFRNIndicator,
									&Line.RFIndicator);
			Line.szSatCOSPARID[7]   = '\0';
			szYear[2]               = '\0';
			szDay[3]                = '\0';
			szTimeofDay[12]         = '\0';
			szStationID[4]          = '\0';
			szCDPSysNum[2]          = '\0';
			szCDPOSNum[2]           = '\0';
			szAzimuth[7]            = '\0';
			szElevation[6]          = '\0';
			szLaserRange[12]        = '\0';
			szLaserRangeSD[7]       = '\0';
			szWavelength[4]         = '\0';
			szSurfacePressure[5]    = '\0';
			szSurfaceTemperature[4] = '\0';
			szSurfaceRelHumidity[3] = '\0';
			szTropCorrect[5]        = '\0';
			szMassCenterCorrect[6]  = '\0';
			szReceiveAmplitude[5]   = '\0';
			szCalibrationSD[4]      = '\0';
			szNormalWindowIndicator[1] = '\0';
			szCompressedRangeNum[4]    = '\0';
			szEpochEvent[1]            = '\0';
			szEpochTimeScale[1]        = '\0';
			szAngleOriginIndicator[1]  = '\0';
			szTRCIndicator[1]          = '\0';
			szMCCIndicator[1]          = '\0';
			szRACIndicator[1]          = '\0';
			szSCMIndicator[1]          = '\0';
			szCDSIndicator[1]          = '\0';
			szSCFIndicator[1]          = '\0';
			szFRNIndicator[1]          = '\0';
			szCalibrationSysDelay[8]   = '\0';
			szCalibrationDelayShift[6] = '\0';
			cstgSLRObsFile::deleteStringZero(szCalibrationSysDelay);
			cstgSLRObsFile::deleteStringZero(szCalibrationDelayShift);
			sscanf(szCalibrationSysDelay,   "%8lf",  &Line.CalibrationSysDelay);
			sscanf(szCalibrationDelayShift, "%6lf",  &Line.CalibrationDelayShift);
			sscanf(szYear, "%2d",   &Line.Year);
			sscanf(szDay,  "%3d",   &Line.Day);
			sscanf(szTimeofDay,     "%12lf",   &Line.TimeofDay);
			sscanf(szStationID,     "%4d",     &Line.StationID);
			sscanf(szCDPSysNum,     "%2d",     &Line.CDPSysNum);
			sscanf(szCDPOSNum,      "%2d",     &Line.CDPOSNum);
			sscanf(szAzimuth,       "%7lf",    &Line.Azimuth);
			sscanf(szElevation,     "%6lf",    &Line.Elevation);
			sscanf(szLaserRange,    "%12lf",   &Line.LaserRange);
			sscanf(szLaserRangeSD,  "%7lf",    &Line.LaserRangeSD);
			sscanf(szWavelength,    "%4lf",    &Line.Wavelength);
			sscanf(szSurfacePressure,    "%5lf",   &Line.SurfacePressure);
			sscanf(szSurfaceTemperature, "%4lf",   &Line.SurfaceTemperature);
			sscanf(szSurfaceRelHumidity, "%3lf",   &Line.SurfaceRelHumidity);
			sscanf(szTropCorrect,        "%5lf",   &Line.TropCorrect);
			sscanf(szMassCenterCorrect,  "%6lf",   &Line.MassCenterCorrect);
			sscanf(szReceiveAmplitude,   "%5lf",   &Line.ReceiveAmplitude);
			sscanf(szCalibrationSD,          "%4lf",   &Line.CalibrationSD);
			sscanf(szNormalWindowIndicator,  "%1d",    &Line.NormalWindowIndicator);
			sscanf(szCompressedRangeNum,     "%4d",    &Line.CompressedRangeNum);
			sscanf(szEpochEvent,           "%1d",    &Line.EpochEvent);
			sscanf(szEpochTimeScale,       "%1d",    &Line.EpochTimeScale);
			sscanf(szAngleOriginIndicator, "%1d",    &Line.AngleOriginIndicator);
			sscanf(szTRCIndicator,         "%1d",    &Line.TRCIndicator);
			sscanf(szMCCIndicator,         "%1d",    &Line.MCCIndicator);
			sscanf(szRACIndicator,         "%1d",    &Line.RACIndicator);
			sscanf(szSCMIndicator,         "%1d",    &Line.SCMIndicator);
			sscanf(szCDSIndicator,         "%1d",    &Line.SCHIndicator);
			sscanf(szSCFIndicator,         "%1d",    &Line.SCFIndicator);
			sscanf(szFRNIndicator,         "%1d",    &Line.FRNIndicator);
			
			//Line.screenPrintf();
			nFlag = true;
			if( Line.Day > 366 || Line.Day < 0 )
				nFlag = false;
			if( Line.Year < 0 )
				nFlag = false;
			if( Line.TimeofDay < 0 )
				nFlag = false;
			// �ж�����������?
			if(nFlag)
				meritLine = Line;
			return nFlag;
		}

		bool meritSLRObsFile::open(string strMeritFileName)
		{
			FILE * pMeritFile = fopen(strMeritFileName.c_str(),"r+t");
			if(pMeritFile == NULL) 
				return false;
			char szline[200];     
			m_data.clear();
			while(!feof(pMeritFile))
			{
				if(fgets(szline, 200, pMeritFile))  
				{
					meritDataRecord Line;
					if(isValidEpochLine(szline, Line))
					{
						m_data.push_back(Line);
					}
				}
			}
			fclose(pMeritFile);

			if(m_data.size() > 0)
				return true;
			else
				return false;
		}

		bool meritSLRObsFile::append(string strMeritFileName)
		{
			meritSLRObsFile meritFile;
			if(!meritFile.open(strMeritFileName))
				return false;
			for(size_t s_i = 0; s_i < meritFile.m_data.size(); s_i++)
				m_data.push_back(meritFile.m_data[s_i]);
			return true;
		}

		UTC meritDataRecord::getTime()
		{
			int nB4Year = yearB2toB4(Year);
			UTC t(nB4Year, 1, 1, 0, 0, 0);
			t = t + (Day - 1) * 86400 ; // �������(nDay�Ƿ��1��ʼ����)
			t = t + TimeofDay * 1.0E-7; // �ټ�����Ĳ���
			return t;
		}

		void meritDataRecord::screenPrintf()
		{
			printf("******************************************************\n");
			printf("����COSPAR��: %s\n",szSatCOSPARID);
			printf("������: %2d\n",Year);
			printf("�����: %3d\n",Day);
			printf("��: %f\n",TimeofDay * 0.1E-6);
			printf("��վ���: %4d\n",StationID);
			printf("�淽λ��: %f\n", Azimuth * 1.0E-4);
			printf("��߶Ƚ�: %f\n", Elevation* 1.0E-4);
			printf("��������: %f ǧ��\n", LaserRange * 1.0E-12 * SPEED_LIGHT * 1.0E-3);
			printf("�����׼��: %f ��\n", LaserRangeSD * 1.0E-12 * SPEED_LIGHT);
			printf("���Ⲩ��: %f ����\n", Wavelength  * 10);
			printf("�ر���ѹ��: %f ����\n", SurfacePressure * 0.1);
			printf("�ر����¶�: %f K\n", SurfaceTemperature * 0.1);
			printf("�ر������ʪ��: %%%f\n", SurfaceRelHumidity);
			printf("�����������������: %f ��\n", TropCorrect * 1.0E-12 * SPEED_LIGHT);
			printf("����һ�ܵ����ĸ���: %f ��\n", MassCenterCorrect * 1.0E-12 * SPEED_LIGHT);
			printf("ϵͳ�ӳ�: %f ��\n", CalibrationSysDelay * 1.0E-12 * SPEED_LIGHT);
			printf("ϵͳ�ӳ�У��Ư��: %f ��\n", CalibrationDelayShift * 1.0E-12 * SPEED_LIGHT);
			printf("ϵͳ�ӳ�У����׼��: %f ��\n", CalibrationSD * 1.0E-12 * SPEED_LIGHT);

			switch(NormalWindowIndicator)
			{
			case 0:
				printf("��׼��ָʾ���ڣ��Ǳ�׼��\n");
				break;
			case 2:
				printf("��׼��ָʾ���ڣ�10��\n");
				break;
			case 3:
				printf("��׼��ָʾ���ڣ�15��\n");
				break;
			case 4:
				printf("��׼��ָʾ���ڣ�20��\n");
				break;
			case 5:
				printf("��׼��ָʾ���ڣ�30��\n");
				break;
			case 6:
				printf("��׼��ָʾ���ڣ�1����\n");
				break;
			case 7:
				printf("��׼��ָʾ���ڣ�2����\n");
				break;
			case 8:
				printf("��׼��ָʾ���ڣ�3����\n");
				break;
			case 9:
				printf("��׼��ָʾ���ڣ�5����\n");
				break;
			default:
				printf("��׼��ָʾ���ڣ�*** %d\n",NormalWindowIndicator);
				break;
			}

			printf("ѹ���ɱ�׼���ԭʼ���ݵ���Ŀ: %d ��\n", CompressedRangeNum);
			switch(EpochEvent)
			{
			case 0:
				printf("��Ԫ�ο��㣺���漤����ջز�ʱ��\n");
				break;
			case 1:
				printf("��Ԫ�ο��㣺���Ƿ���ʱ��\n");
				break;
			case 2:
				printf("��Ԫ�ο��㣺���淢��ʱ��\n");
				break;
			case 3:
				printf("��Ԫ�ο��㣺��������ʱ��\n");
				break;
			default:
				printf("��Ԫ�ο��㣺***%d\n",EpochEvent);
				break;
			}
			switch(EpochTimeScale)
			{
			case 0:
				printf("��Ԫʱ���׼��UT0\n");
				break;
			case 1:
				printf("��Ԫʱ���׼��UT1\n");
				break;
			case 2:
				printf("��Ԫʱ���׼��UT2\n");
				break;
			case 3:
				printf("��Ԫʱ���׼��UTC(USNO) ������������̨\n");
				break;
			case 4:
				printf("��Ԫʱ���׼��A.1(USNO)\n");
				break;
			case 5:
				printf("��Ԫʱ���׼��TAI\n");
				break;
			case 6:
				printf("��Ԫʱ���׼��A-S\n");
				break;
			case 7:
				printf("��Ԫʱ���׼��UTC(BIH) ����ʱ���\n");
				break;
			default:
				printf("��Ԫʱ���׼��***%d\n",EpochTimeScale);
				break;
			}

			switch(AngleOriginIndicator)
			{
			case 0:
				printf("�Ƕ����ָʾ��δ֪\n");
				break;
			case 1:
				printf("�Ƕ����ָʾ���ɲ�����õ�\n");
				break;
			case 2:
				printf("�Ƕ����ָʾ�����裨Ԥ�������Ա���룩\n");
				break;
			case 3:
				printf("�Ƕ����ָʾ�������õ�����У�������Ķ�����\n");
				break;
			default:
				printf("�Ƕ����ָʾ��***%d\n",AngleOriginIndicator);
				break;
			}

			if( TRCIndicator == 0 )
				printf("�������������ָʾ���������� Marini-Murray ��ʽ���� \n");
			else
				printf("�������������ָʾ������δ����Ӧ���� \n");

			if( MCCIndicator == 0 )
				printf("���ĸ���ָʾ���������� \n");
			else
				printf("���ĸ���ָʾ��δ������ \n");

			if( RACIndicator == 0 )
				printf("���շ��ȸ���ָʾ���������� \n");
			else
				printf("���շ��ȸ���ָʾ��δ������ \n");

			switch(SCMIndicator)
			{
			case 0:
				printf("ϵͳУ������ָʾ���ⲿУ��\n");
				break;
			case 1:
				printf("ϵͳУ������ָʾ���ڲ�У��\n");
				break;
			case 2:
				printf("ϵͳУ������ָʾ���ֶ�У��\n");
				break;
			case 3:
				printf("ϵͳУ������ָʾ��δ����У��\n");
				break;
			default:
				printf("ϵͳУ������ָʾ��***%d\n", SCMIndicator);
				break;
			}

			switch(SCHIndicator)
			{
			case 0:
				printf("�ӳ�У��Ư��ָʾ����ǰ������Ư��\n");
				break;
			case 1:
				printf("�ӳ�У��Ư��ָʾ����ֵ����ֵ��Ư��\n");
				break;
			default:
				printf("�ӳ�У��Ư��ָʾ��***%d\n",SCHIndicator);
				break;
			}

			printf("ϵͳ�ṹ���ָʾ��%d\n", SCFIndicator);
			printf("��ʽ����ָʾ��%d\n", FRNIndicator);
			printf("�ͷ��ź�ָʾ��%c\n", RFIndicator);
			printf("******************************************************\n");
		}
	}
}
