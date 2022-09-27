#include "Rinex3_0_ObsFile.hpp"
#include "constDef.hpp"
#include "MathAlgorithm.hpp"
#include "TimeCoordConvert.hpp"

using namespace NUDTTK::Math;


namespace NUDTTK
{			
    const char Rinex3_0_MaskString::szVerType[]          = "RINEX VERSION / TYPE";
	const char Rinex3_0_MaskString::szPgmRunDate[]       = "PGM / RUN BY / DATE ";
	const char Rinex3_0_MaskString::szComment[]          = "COMMENT             ";	
	const char Rinex3_0_MaskString::szMarkerName[]       = "MARKER NAME         ";	
	const char Rinex3_0_MaskString::szMarkerNum[]        = "MARKER NUMBER       ";
	const char Rinex3_0_MaskString::szMarkerType[]       = "MARKER TYPE         ";
	const char Rinex3_0_MaskString::szObservAgency[]     = "OBSERVER / AGENCY   ";
	const char Rinex3_0_MaskString::szRecTypeVers[]      = "REC # / TYPE / VERS ";
	const char Rinex3_0_MaskString::szAntType[]          = "ANT # / TYPE        ";
	const char Rinex3_0_MaskString::szApproxPosXYZ[]     = "APPROX POSITION XYZ ";	
	const char Rinex3_0_MaskString::szAntDeltaHEN[]      = "ANTENNA: DELTA H/E/N";
	const char Rinex3_0_MaskString::szAntDeltaXYZ[]      = "ANTENNA: DELTA X/Y/Z";
	const char Rinex3_0_MaskString::szAntPhaseCenter[]   = "ANTENNA: PHASECENTER";
	const char Rinex3_0_MaskString::szAntBSightXYZ[]     = "ANTENNA: B.SIGHT XYZ";
	const char Rinex3_0_MaskString::szZeroDirAZI[]       = "ANTENNA: ZERODIR AZI";
	const char Rinex3_0_MaskString::szZeroDirXYZ[]       = "ANTENNA: ZERODIR XYZ";
	const char Rinex3_0_MaskString::szCerterOfMassXYZ[]  = "CENTER OF MASS: XYZ ";
	const char Rinex3_0_MaskString::szSysTypeOfObs[]     = "SYS / # / OBS TYPES ";
	const char Rinex3_0_MaskString::szSignalStrUnit[]    = "SIGNAL STRENGTH UNIT";
	const char Rinex3_0_MaskString::szInterval[]         = "INTERVAL            ";
	const char Rinex3_0_MaskString::szTmOfFirstObs[]     = "TIME OF FIRST OBS   ";	
	const char Rinex3_0_MaskString::szTmOfLastObs[]      = "TIME OF LAST OBS    ";
	const char Rinex3_0_MaskString::szRecClockOffApp[]   = "RCV CLOCK OFFS APPL ";
	const char Rinex3_0_MaskString::szSysDCBSApp[]       = "SYS / DCBS APPLIED  ";
	const char Rinex3_0_MaskString::szSysPCVSApp[]       = "SYS / PCVS APPLIED  ";
	const char Rinex3_0_MaskString::szIonoCorr[]         = "IONOSPHERIC CORR    ";
	const char Rinex3_0_MaskString::szTimeSysCorr[]      = "TIME SYSTEM CORR    ";
	const char Rinex3_0_MaskString::szSysScaleFac[]      = "SYS / SCALE FACTOR  ";
	const char Rinex3_0_MaskString::szLeapSec[]          = "LEAP SECONDS        ";
	const char Rinex3_0_MaskString::szNumsOfSv[]         = "# OF SATELLITES     ";
	const char Rinex3_0_MaskString::szPRNNumOfObs[]      = "PRN / # OF OBS      ";	
	const char Rinex3_0_MaskString::szEndOfHead[]        = "END OF HEADER       ";

 
	Rinex3_0_ObsFile::Rinex3_0_ObsFile(void)
	{
	}
    Rinex3_0_ObsFile::~Rinex3_0_ObsFile(void)
	{
	}
	void Rinex3_0_ObsFile::clear()
    {
		m_header = Rinex3_0_ObsHeader::Rinex3_0_ObsHeader();
		m_data.clear();		
	}

	bool Rinex3_0_ObsFile::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}
	// �ӳ������ƣ� isValidEpochLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	//         ����0 -> �ļ�ĩβ
	//         ����1 -> ��Чʱ��
	//         ����2 -> ��Ч
	// �������ͣ�strLine           : ���ı� 
	//           pObsfile      ����: �ļ�ָ��
	// ���룺strLine, pObsfile
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/4/7
	// �汾ʱ�䣺2013/4/7
	// �޸ļ�¼��
	// ������ 
	int  Rinex3_0_ObsFile::isValidEpochLine(string strLine, FILE * pObsfile)
	{
		DayTime tmEpoch;
		// ���漸��������int��, ������ΪstrLine�ĸ�ʽ��������sscanf������������
		int byEpochFlag = -1; // 0: OK; 1: power failure between previous and current epoch  > 1: Event flag ( 2 - 6 ) 
		int bySatCount  = -1; // ��ʱ�����ǣ����վ������
		char szRecordIdentifier[1 +1 ];
		if(pObsfile != NULL)  // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pObsfile))
				return 0;
		}		

		sscanf(strLine.c_str(),"%1c%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
			&szRecordIdentifier,
			&tmEpoch.year,
			&tmEpoch.month,
			&tmEpoch.day,
			&tmEpoch.hour,
			&tmEpoch.minute,
			&tmEpoch.second,
			&byEpochFlag,
			&bySatCount);
		if(szRecordIdentifier[0] != '>')
			return 2;
		if(tmEpoch.month > 12 || tmEpoch.month<= 0)
			return 2;
		if(tmEpoch.day >31 || tmEpoch.day <= 0)
			return 2;
		/* BDS �߲��������к��С�2015 03 25 00 18 73.0000000�������ݸ�ʽ��supice, 2015-11-27
		if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
			return 2;
		if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
			return 2;
		if(tmEpoch.second > 60 || tmEpoch.second < 0)
			return 2;
		*/
		if(byEpochFlag > 6 || byEpochFlag < 0)
			return 2;
		if(bySatCount > 999 || bySatCount < 0)
			return 2;
		return 1;
	}

	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
    // �������ͣ�strObsfileName : �۲������ļ�·��
	//           bOn_BDT2GPST   : �Ƿ�BDTת��ΪGPST, Ĭ��GPSʱ��
    // ���룺strObsfileName
    // �����
    // ���ԣ�C++
    // �����ߣ�������
	// ����ʱ�䣺2013/4/7
	// �汾ʱ�䣺2013/4/7
    // �޸ļ�¼��
    // ������ 
    bool Rinex3_0_ObsFile::open(string strObsfileName, bool bOn_BDT2GPST)
	{
		//clock_t start, finish;
		//double  duration;
		//start = clock();

		if(!isWildcardMatch(strObsfileName.c_str(), "*.*O", true) && !isWildcardMatch(strObsfileName.c_str(), "*.rnx", true))
		{
			printf(" %s �ļ�����ƥ��!\n", strObsfileName.c_str());
			return false;
		}

		FILE * pObsfile = fopen(strObsfileName.c_str(),"r+t");
		if(pObsfile == NULL) 
		{
			printf(" %s �ļ�Ϊ��!\n", strObsfileName.c_str());
			return false;
		}
		m_header = Rinex3_0_ObsHeader::Rinex3_0_ObsHeader();		
		// ��ʼѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
		int bFlag = 1;
		while(bFlag)
		{
			char line[400];
			fgets(line,400,pObsfile);
			string strLineMask = line;
			string strLine     = line;			
			strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
			// �޳� \n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n > 10000)//2013.7.16 ������
			{
				printf(" %s �ļ�����!\n", strObsfileName.c_str());
				fclose(pObsfile);	
				return false;
			}
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n, 1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length ��������'\0'
				strLineMask.append(20-strLineMask.length(),' ');

			if(strLineMask == Rinex3_0_MaskString::szVerType)
			{
				sscanf(line,"%9lf%*11c%1c%*19c%1c",&m_header.RinexVersion,&m_header.szFileType,&m_header.szSatlliteSystem);					
				m_header.szFileType[1]='\0';
				m_header.szSatlliteSystem[1]='\0';
			}				
			else if(strLineMask == Rinex3_0_MaskString::szPgmRunDate)
			{
				strLine.copy(m_header.szProgramName, 20, 0);
				strLine.copy(m_header.szProgramAgencyName, 20, 20);
				strLine.copy(m_header.szFileDate, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szComment)
			{
				strLine.copy(m_header.szCommentLine, 60, 0);					
			}
			else if(strLineMask == Rinex3_0_MaskString::szMarkerName)
			{
				strLine.copy(m_header.szMarkName, 60, 0);					
			}
			else if(strLineMask == Rinex3_0_MaskString::szMarkerNum)
			{
				strLine.copy(m_header.szMarkNumber, 20, 0);					
			}
			else if(strLineMask == Rinex3_0_MaskString::szMarkerType)
			{
				strLine.copy(m_header.szMarkType, 20, 0);					
			}
			else if(strLineMask == Rinex3_0_MaskString::szObservAgency)
			{
				strLine.copy(m_header.szObserverName, 20, 0);
				strLine.copy(m_header.szObserverAgencyName, 40, 20);
			}			
			else if(strLineMask == Rinex3_0_MaskString::szRecTypeVers)
			{
				strLine.copy(m_header.szRecNumber, 20, 0);
				strLine.copy(m_header.szRecType, 20, 20);
				strLine.copy(m_header.szRecVersion, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szAntType)
			{
				strLine.copy(m_header.szAntNumber, 20, 0);
				strLine.copy(m_header.szAntType, 20, 20);
			}
			else if(strLineMask == Rinex3_0_MaskString::szApproxPosXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.ApproxPosXYZ.x, &m_header.ApproxPosXYZ.y, &m_header.ApproxPosXYZ.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szAntDeltaHEN)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntDeltaHEN.x, &m_header.AntDeltaHEN.y, &m_header.AntDeltaHEN.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szAntDeltaXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntDeltaXYZ.x, &m_header.AntDeltaXYZ.y, &m_header.AntDeltaXYZ.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szAntPhaseCenter)
			{  
				AntPhaCen	antPhaCen;		
				sscanf(line,"%1c%*1c%3c%9lf%14lf%14lf",&antPhaCen.szSatlliteSystem,
					                                   &antPhaCen.szObsCode,
													   &antPhaCen.PhaCen.x,
													   &antPhaCen.PhaCen.y,
													   &antPhaCen.PhaCen.z);
				antPhaCen.szSatlliteSystem[1] = '\0';
				m_header.AntPhaCentList.push_back(antPhaCen);
								
			}
			else if(strLineMask == Rinex3_0_MaskString::szAntBSightXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntDirection.x, &m_header.AntDirection.y, &m_header.AntDirection.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szZeroDirAZI)
			{  
				sscanf(line,"%14lf",&m_header.AntZeroDirAZI);
			}
			else if(strLineMask == Rinex3_0_MaskString::szZeroDirXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntZeroDirXYZ.x, &m_header.AntZeroDirXYZ.y, &m_header.AntZeroDirXYZ.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szCerterOfMassXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntCenOfMas.x, &m_header.AntCenOfMas.y, &m_header.AntCenOfMas.z);
			}
			else if(strLineMask == Rinex3_0_MaskString::szSysTypeOfObs)
			{  

				SysObsTyp         sysObsTyp;									
				sscanf(line,"%1c%*2c%3d", &sysObsTyp.szSatlliteSystem, &sysObsTyp.ObsTypCount);
				sysObsTyp.szSatlliteSystem[1] = '\0';
				int               nline = sysObsTyp.ObsTypCount / 13;	// ������
				int               nResidue = sysObsTyp.ObsTypCount % 13;
				if(sysObsTyp.ObsTypCount <= 13)
				{
					for(BYTE i = 0; i < sysObsTyp.ObsTypCount; i++)
					{
						char strObsType[3 + 1];
						strLine.copy(strObsType, 3, 6 + i * 4 + 1);
						strObsType[3] = '\0';
						sysObsTyp.ObsTypelist.push_back(strObsType);
					}					
				}
				else
				{
					for(BYTE i = 0; i < 13; i++)
					{
						char strObsType[3 + 1];
						strLine.copy(strObsType, 3, 6 + i * 4 + 1);
						strObsType[3] = '\0';
						sysObsTyp.ObsTypelist.push_back(strObsType);
					}
					for(int n = 1; n < nline;n++)
					{// ��ȡ�м������
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < 13; i++)
						{
							char strObsType[3 + 1];
							strLine.copy(strObsType, 3, 6 + i * 4 + 1);
							strObsType[3] = '\0';
							sysObsTyp.ObsTypelist.push_back(strObsType);
						}						
					}
					if(nResidue > 0)
					{
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < nResidue; i++)
						{
							char strObsType[3 + 1];
							strLine.copy(strObsType, 3, 6 + i * 4 + 1);
							strObsType[3] = '\0';
							sysObsTyp.ObsTypelist.push_back(strObsType);
						}		
					}
				}
				m_header.SysObsTypList.push_back(sysObsTyp);
			}
			else if(strLineMask == Rinex3_0_MaskString::szSignalStrUnit)
			{
				strLine.copy(m_header.szSignalSteUnit, 20, 0);
			}
			else if(strLineMask == Rinex3_0_MaskString::szInterval)
			{  
				sscanf(line,"%10lf", &m_header.Interval);
			}
			else if(strLineMask == Rinex3_0_MaskString::szTmOfFirstObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%13lf",&m_header.tmStart.year, 
					                               &m_header.tmStart.month,
												   &m_header.tmStart.day,   
												   &m_header.tmStart.hour,
												   &m_header.tmStart.minute,
												   &m_header.tmStart.second);
				strLine.copy(m_header.szTimeSystem, 3, 48);

				string strTimeSystem = m_header.szTimeSystem;
				if(strTimeSystem.find("BDT") == -1)
				{
					bOn_BDT2GPST = false;
				}
				if(bOn_BDT2GPST)
				{// 20160519, ��ʱ��ϵͳ����ΪGPSʱ��
					sprintf(m_header.szTimeSystem, "GPS");
					m_header.tmStart = TimeCoordConvert::BDT2GPST(m_header.tmStart);
				}
			}
			else if(strLineMask == Rinex3_0_MaskString::szTmOfLastObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%13lf",&m_header.tmEnd.year, 
					                               &m_header.tmEnd.month,
												   &m_header.tmEnd.day,	 
												   &m_header.tmEnd.hour,
												   &m_header.tmEnd.minute,
												   &m_header.tmEnd.second);
				if(bOn_BDT2GPST)// 20160519, ��ʱ��ϵͳ����ΪGPSʱ��
					m_header.tmEnd = TimeCoordConvert::BDT2GPST(m_header.tmEnd);
			}
			else if(strLineMask == Rinex3_0_MaskString::szRecClockOffApp)
			{  
				sscanf(line,"%6d",&m_header.RecClokOffApp);
			}
			else if(strLineMask == Rinex3_0_MaskString::szSysDCBSApp)
			{  
				DCBPCVApp         DCBApp;						
				strLine.copy(DCBApp.szSatlliteSystem, 1, 0);
				strLine.copy(DCBApp.szCorPro, 17, 2);
				strLine.copy(DCBApp.szCorSou, 40, 20);	
				m_header.SysDCBAppList.push_back(DCBApp);
			}
			else if(strLineMask == Rinex3_0_MaskString::szSysPCVSApp)
			{  
				DCBPCVApp         PCVApp;				
				strLine.copy(PCVApp.szSatlliteSystem, 1, 0);
				strLine.copy(PCVApp.szCorPro, 17, 2);
				strLine.copy(PCVApp.szCorSou, 40, 20);	
				m_header.SysPCVAppList.push_back(PCVApp);
			}
			else if(strLineMask == Rinex3_0_MaskString::szSysScaleFac)
			{  
				SysObsTyp         sysObsTyp;							
				sscanf(line,"%1c%*1c%4d%*2c%2d", &sysObsTyp.szSatlliteSystem,&sysObsTyp.ScaFactor,&sysObsTyp.ObsTypCount);
				sysObsTyp.szSatlliteSystem[1] = '\0';
				int               nline = sysObsTyp.ObsTypCount / 12;	// ������
				int               nResidue = sysObsTyp.ObsTypCount % 12;
				if(sysObsTyp.ObsTypCount <= 12)
				{
					for(BYTE i = 0; i < sysObsTyp.ObsTypCount; i++)
					{
						char strObsType[3 + 1];
						strLine.copy(strObsType, 3, 10 + i * 4 + 1);
						strObsType[3] = '\0';
						sysObsTyp.ObsTypelist.push_back(strObsType);
					}				
				}
				else
				{
					for(BYTE i = 0; i < 12; i++)
					{
						char strObsType[3 + 1];
						strLine.copy(strObsType, 3, 10 + i * 4 + 1);
						strObsType[3] = '\0';
						sysObsTyp.ObsTypelist.push_back(strObsType);
					}
					for(int n = 1;n < nline; n++)
					{// ��ȡ�м������
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < 12; i++)
						{
							char strObsType[3 + 1];
							strLine.copy(strObsType, 3, 10 + i * 4 + 1);
							strObsType[3] = '\0';
							sysObsTyp.ObsTypelist.push_back(strObsType);
						}
					}
					if(nResidue > 0)
					{
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < nResidue; i++)
						{
							char strObsType[3 + 1];
							strLine.copy(strObsType, 3, 10 + i * 4 + 1);
							strObsType[3] = '\0';
							sysObsTyp.ObsTypelist.push_back(strObsType);
						}	
					}
				}
				m_header.SysSclFacList.push_back(sysObsTyp);
			}
			else if(strLineMask == Rinex3_0_MaskString::szLeapSec)
			{  
				sscanf(line,"%6d", &m_header.LeapSecond);
			}
			else if(strLineMask == Rinex3_0_MaskString::szNumsOfSv)
			{  
				sscanf(line,"%6d", &m_header.SatCount);
			}			
			//else if(strLineMask == Rinex3_0_MaskString::szPRNNumOfObs)
			//{// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
			//	/*
			//	static BYTE	nPRN=0;
			//	nPRN++;
			//	BYTE bPRN;
			//	sscanf(line,"%*4c%2d",&bPRN);
			//	m_header.pbySatList.push_back(bPRN);
			//	*/
			//}
			else if(strLineMask == Rinex3_0_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else 
			{// ���������ݲ�������
			}
		}

		// �۲�����
		bFlag = TRUE;
		m_data.clear();
		int k = 0;
		char line[400];
		fgets(line, 400, pObsfile);
		//BYTE pbySatList[MAX_GPSPRN + 1]; // �����б�
		//for(int i = 0; i < MAX_GPSPRN + 1; i++)
		//	pbySatList[i] = 0;
		while(bFlag)
		{
			string strLine = line;
			int nFlag = isValidEpochLine(strLine, pObsfile);
			if(nFlag == 0)      // �ļ�ĩβ
			{
				bFlag = false;
			}
			else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
			{
				k++;				
				Rinex3_0_ObsEpoch   obsEpoch;	
				obsEpoch.obs.resize(m_header.SysObsTypList.size()); //�۲�����������ʼ��
				for(size_t s_i = 0; s_i < m_header.SysObsTypList.size(); s_i ++)
				{
					obsEpoch.obs[s_i].szSatlliteSystem[0] = m_header.SysObsTypList[s_i].szSatlliteSystem[0];
					obsEpoch.obs[s_i].szSatlliteSystem[1] = '\0';
				}
				//for(size_t)
				// ����Epoch
				sscanf(strLine.c_str(),"%1c%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d%*6c%15lf",
					&obsEpoch.szRecordIdentifier,
					&obsEpoch.t.year,
					&obsEpoch.t.month,
					&obsEpoch.t.day,
					&obsEpoch.t.hour,
					&obsEpoch.t.minute,
					&obsEpoch.t.second,
					&obsEpoch.byEpochFlag,
					&obsEpoch.EpochSatCount,
					&obsEpoch.RecClockOffset);
				obsEpoch.szRecordIdentifier[1]='\0';

				// supice ����ʱ���ǵĸ�ʽ��2015-11-27
				if(obsEpoch.t.second >= 60)
				{
					int minute = int(floor(obsEpoch.t.second / 60.0));
					obsEpoch.t.second = obsEpoch.t.second - minute * 60;
					obsEpoch.t.minute += minute;
					if(obsEpoch.t.minute >= 60)
					{
						int hour = int(floor(obsEpoch.t.minute / 60.0));
						obsEpoch.t.minute = obsEpoch.t.minute - hour * 60;
						obsEpoch.t.hour += hour;
						if(obsEpoch.t.hour >= 24)
						{
							int day = int(floor(obsEpoch.t.hour / 24.0));
							obsEpoch.t.hour = obsEpoch.t.hour - day * 24;
							obsEpoch.t.day += day;
						}
					}	
				}

				if(bOn_BDT2GPST)// 20160519, ��ʱ��ϵͳ����ΪGPSʱ��
					obsEpoch.t = TimeCoordConvert::BDT2GPST(obsEpoch.t);

				// ����Epoch/SAT�������ɼ��۲�����			
				for(int i = 0;i < obsEpoch.EpochSatCount; i++)
				{				
					char                  szSatlliteSystem[1 + 1];  // ����ϵͳ
					int                   satPRN = 0;               // ���Ǳ��
					int                   obsTypCount;              // �۲��������͸���
					size_t                s_sat;
					Rinex2_1_ObsTypeList  obsTypeList;
					fgets(line, 400, pObsfile);
					strLine = line;
					sscanf(line, "%1c%2d", &szSatlliteSystem, &satPRN);
					szSatlliteSystem[1] = '\0';					
					for(size_t s_i = 0; s_i < m_header.SysObsTypList.size(); s_i ++)
					{
						if(m_header.SysObsTypList[s_i].szSatlliteSystem[0] == szSatlliteSystem[0])
						{
							s_sat = s_i;
							obsTypCount = m_header.SysObsTypList[s_i].ObsTypCount;
							break;
						}
					}
					for(int j = 0; j < obsTypCount;j ++)
					{
						Rinex2_1_ObsDatum     obsDatum;						
						char strObsSatElements[17];
						char strObsValue[15];
						if(size_t(16 * j + 14) <= strLine.length())
						{
							strLine.copy(strObsSatElements, 16, 16 * j + 3);
							string  strElements = strObsSatElements;
							size_t  nPos_n = strElements.find('\n');
							if(nPos_n < strElements.length())      // �����س�������ַ�
							{
								strElements.erase(nPos_n, 16 - nPos_n);
								if(strElements.length() < 16)		   // ����16λ						
									strElements.append(16 - strElements.length(),' ');
								strElements.copy(strObsSatElements, 16, 0);								
							}
							strObsSatElements[16] = '\0';
							strncpy(strObsValue, strObsSatElements, 14);							
							strObsValue[14] = '\0';
							//��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
							if(strcmp(strObsValue,"              ") == 0)							
								obsDatum.data = DBL_MAX;												
							else
								//sscanf(strObsSatElements, "%14lf", &obsDatum.data);
								sscanf(strObsValue, "%lf", &obsDatum.data);
								sscanf(strObsSatElements, "%*14c%1c%1c", &obsDatum.lli, &obsDatum.ssi);
						}
						else
							obsDatum.data = DBL_MAX;
						obsTypeList.push_back(obsDatum);
					}
					obsEpoch.obs[s_sat].sobs.insert(Rinex2_1_SatMap::value_type(satPRN, obsTypeList));					
				}
				// �޳��۲�����Ϊ�������ϵͳ
				size_t s_j  = 0;
				//int    k = 0;
				while(s_j < obsEpoch.obs.size())
				{
					//k++;					
					if(obsEpoch.obs[s_j].sobs.size() == 0)
						obsEpoch.obs.erase(obsEpoch.obs.begin() + s_j);
					else
					{
						s_j++;
						continue;
					}
				}	
				m_data.push_back(obsEpoch);
				fgets(line,400,pObsfile);
		    }
	        else  
	        {// ��Ч�����У�����հ��У��ӹ���������
				fgets(line,400,pObsfile);
	        }
		}
		// �ۺ�ͳ�ƿ��������б�
		//// m_header.pbySatList.clear();
		//for(int i = 0; i <= MAX_GPSPRN; i++)
		//{
		//	if(pbySatList[i] == 1)
		//	{
		//		m_header.pbySatList.push_back(BYTE(i));
		//	}
		//}
		//m_header.bySatCount = BYTE(m_header.pbySatList.size());
		//���³�ʼ�۲�ʱ�̺����۲�ʱ��
		size_t nListNum = m_data.size();
		if(nListNum > 0)
		{
			m_header.tmStart = m_data[0].t;
			m_header.tmEnd   = m_data[nListNum - 1].t;
		}
		fclose(pObsfile);		
		return true;
    }
	// �ӳ������ƣ� write   
	// ���ܣ����۲�����д���ļ� 
	// �������ͣ�strObsfileName_noExp     : �۲������ļ�·��(��׺�������Զ�����, ���벻����)
	//           strObsfileName_all       : �����Ĺ۲������ļ�·��
	// ���룺strObsfileName_noExp
	// �����strObsfileName_all
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/4/7
	// �汾ʱ�䣺2013/5/6
	// �޸ļ�¼��
	// ������ 
	bool Rinex3_0_ObsFile::write(string strObsfileName_noExp,string& strObsfileName_all)
	{
		if(isEmpty())
			return false;
		// д�ļ�ͷ������ Rinex3.0 �ı�׼�ļ�ͷ��д
		int n1  = getIntBit(m_header.tmStart.year, 0);
		int n10 = getIntBit(m_header.tmStart.year, 1);
		char strFileExp[5];
		sprintf(strFileExp,".%1d%1do",n10,n1);
		strObsfileName_all = strObsfileName_noExp + strFileExp;
		FILE* pObsfile = fopen(strObsfileName_all.c_str(), "w+");
		fprintf(pObsfile,"%9.2lf%-11s%-20s%-20s%20s\n", 
			m_header.RinexVersion,
			" ",
			m_header.szFileType,
			m_header.szSatlliteSystem,
			Rinex3_0_MaskString::szVerType);
		fprintf(pObsfile,"%-20s%-20s%-20s%20s\n",
			m_header.szProgramName, 
			m_header.szProgramAgencyName, 
			m_header.szFileDate,
			Rinex3_0_MaskString::szPgmRunDate);
		//fprintf(pObsfile,"%-60s%20s\n",//�����Comment line
		//	m_header.szCommentLine,
		//	Rinex3_0_MaskString::szComment);
	
		fprintf(pObsfile,"%-60s%20s\n",
			m_header.szMarkName,
			Rinex3_0_MaskString::szMarkerName);	
		fprintf(pObsfile,"%-20s%-40s%20s\n",
			m_header.szMarkNumber,
			" ",
			Rinex3_0_MaskString::szMarkerNum);	
		fprintf(pObsfile,"%-20s%-40s%20s\n",
			m_header.szMarkType,
			" ",
			Rinex3_0_MaskString::szMarkerType);
		fprintf(pObsfile,"%-20s%-40s%20s\n",
			m_header.szObserverName, 
			m_header.szObserverAgencyName, 
			Rinex3_0_MaskString::szObservAgency);		
		fprintf(pObsfile,"%-20s%-20s%-20s%20s\n",
			m_header.szRecNumber,
			m_header.szRecType,
			m_header.szRecVersion,
			Rinex3_0_MaskString::szRecTypeVers);
		fprintf(pObsfile,"%-20s%-20s%-20s%20s\n",
			m_header.szAntNumber,
			m_header.szAntType,
			" ",
			Rinex3_0_MaskString::szAntType);
		if(m_header.ApproxPosXYZ.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.ApproxPosXYZ.x,
				m_header.ApproxPosXYZ.y,
				m_header.ApproxPosXYZ.z,					 
				" ",
				Rinex3_0_MaskString::szApproxPosXYZ);
		}
		if(m_header.AntDeltaHEN.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.AntDeltaHEN.x,
				m_header.AntDeltaHEN.y,
				m_header.AntDeltaHEN.z,					 
				" ",
				Rinex3_0_MaskString::szAntDeltaHEN);
		}
		if(m_header.AntDeltaXYZ.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.AntDeltaXYZ.x,
				m_header.AntDeltaXYZ.y,
				m_header.AntDeltaXYZ.z,					 
				" ",
				Rinex3_0_MaskString::szAntDeltaXYZ);
		}
		for(size_t s_i = 0; s_i < m_header.AntPhaCentList.size(); s_i ++)
		{
			fprintf(pObsfile,"%1s%1s%3s%9.4f%14.4f%14.4f%-18s%20s\n",
			m_header.AntPhaCentList[s_i].szSatlliteSystem,
			" ",
			m_header.AntPhaCentList[s_i].szObsCode,
			m_header.AntPhaCentList[s_i].PhaCen.x,
			m_header.AntPhaCentList[s_i].PhaCen.y,
			m_header.AntPhaCentList[s_i].PhaCen.z,
			" ",
			Rinex3_0_MaskString::szAntPhaseCenter);
		}
		if(m_header.AntDirection.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.AntDirection.x,
				m_header.AntDirection.y,
				m_header.AntDirection.z,					 
				" ",
				Rinex3_0_MaskString::szAntBSightXYZ);
		}
		if(m_header.AntZeroDirAZI != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%-46s%20s\n",
				m_header.AntZeroDirAZI,				 
				" ",
				Rinex3_0_MaskString::szZeroDirAZI);
		}
		if(m_header.AntZeroDirXYZ.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.AntZeroDirXYZ.x,
				m_header.AntZeroDirXYZ.y,
				m_header.AntZeroDirXYZ.z,					 
				" ",
				Rinex3_0_MaskString::szZeroDirXYZ);
		}
		if(m_header.AntCenOfMas.x != DBL_MAX)
		{
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
				m_header.AntCenOfMas.x,
				m_header.AntCenOfMas.y,
				m_header.AntCenOfMas.z,
				" ",
				Rinex3_0_MaskString::szCerterOfMassXYZ);
		}
		for(size_t s_i = 0; s_i < m_header.SysObsTypList.size(); s_i ++)
		{
			int obsTypes = (int)m_header.SysObsTypList[s_i].ObsTypelist.size();
			int nLine    = obsTypes / 13;  // ������
		    int nResidue = obsTypes % 13;  // ����
			int nBlank   = 0;              // �հ�λ��
			string         strBlank; 
			fprintf(pObsfile,"%1s%-2s%3d",                         
				    m_header.SysObsTypList[s_i].szSatlliteSystem,                          
				    " ",                                                
				    obsTypes);
			if(obsTypes <= 13)
			{
				nBlank = 60 - (6 + 4 * obsTypes);
				strBlank.append(nBlank,' ');				
				for(int i = 0;i < obsTypes;i ++)
					fprintf(pObsfile,"%-1s%3s"," ",m_header.SysObsTypList[s_i].ObsTypelist[i].c_str());
				fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex3_0_MaskString::szSysTypeOfObs);
			}
			else
			{
				nBlank = 60 - (6 + 4 * nResidue);
				strBlank.append(nBlank,' ');
				for(int i = 0; i < 13; i ++)
					fprintf(pObsfile,"%-1s%3s"," ",m_header.SysObsTypList[s_i].ObsTypelist[i].c_str());
				fprintf(pObsfile,"%-2s%20s\n"," ",Rinex3_0_MaskString::szSysTypeOfObs);
				for(int n = 1; n < nLine; n ++) 
				{
					fprintf(pObsfile,"%6s"," ");
					for(int j = 0; j < 13; j ++)
						fprintf(pObsfile,"%-1s%3s"," ",m_header.SysObsTypList[s_i].ObsTypelist[13 * n + j].c_str());
					fprintf(pObsfile,"%20s\n",Rinex3_0_MaskString::szSysTypeOfObs);
				}
				if(nResidue > 0)
				{
					fprintf(pObsfile,"%-6s"," ");
					for(int i = 0; i < nResidue; i ++)
						fprintf(pObsfile,"%-1s%3s"," ",m_header.SysObsTypList[s_i].ObsTypelist[13 * nLine + i].c_str());
				}
				fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex3_0_MaskString::szSysTypeOfObs);
			}			
		}
		fprintf(pObsfile,"%-20s%40s%20s\n",
			m_header.szSignalSteUnit,
			" ",			
			Rinex3_0_MaskString::szSignalStrUnit);		
		if(m_header.Interval != DBL_MAX)
		{
			fprintf(pObsfile,"%10.3f%50s%20s\n",
				m_header.Interval,
				" ",			
				Rinex3_0_MaskString::szInterval);
		}
		fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
			m_header.tmStart.year,  
			m_header.tmStart.month,
			m_header.tmStart.day,   
			m_header.tmStart.hour,
			m_header.tmStart.minute,
			m_header.tmStart.second,		
			" ",
			m_header.szTimeSystem,
			" ",
			Rinex3_0_MaskString::szTmOfFirstObs);
		fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
			m_header.tmEnd.year,  
			m_header.tmEnd.month,
			m_header.tmEnd.day,   
			m_header.tmEnd.hour,
			m_header.tmEnd.minute,
			m_header.tmEnd.second,		
			" ",
			m_header.szTimeSystem,
			" ",
			Rinex3_0_MaskString::szTmOfLastObs);
		if(m_header.RecClokOffApp != INT_MAX)
		{
			fprintf(pObsfile,"%6d%54s%20s\n",
				m_header.RecClokOffApp,
				" ",			
				Rinex3_0_MaskString::szRecClockOffApp);
		}
		for(size_t s_i = 0; s_i < m_header.SysDCBAppList.size(); s_i ++)
		{
			fprintf(pObsfile,"%1s%1s%17s%1s%40s%20s\n",
				m_header.SysDCBAppList[s_i].szSatlliteSystem,
				" ",
				m_header.SysDCBAppList[s_i].szCorPro,
				m_header.SysDCBAppList[s_i].szCorSou,			
				Rinex3_0_MaskString::szSysDCBSApp);
		}
		for(size_t s_i = 0; s_i < m_header.SysPCVAppList.size(); s_i ++)
		{
			fprintf(pObsfile,"%1s%1s%17s%1s%40s%20s\n",
				m_header.SysPCVAppList[s_i].szSatlliteSystem,
				" ",
				m_header.SysPCVAppList[s_i].szCorPro,
				m_header.SysPCVAppList[s_i].szCorSou,			
				Rinex3_0_MaskString::szSysPCVSApp);
		}
		for(size_t s_i = 0; s_i < m_header.SysSclFacList.size(); s_i ++)
		{
			int obsTypes = (int)m_header.SysSclFacList[s_i].ObsTypelist.size();
			int nLine    = obsTypes / 13;  // ������
		    int nResidue = obsTypes % 13;  // ����
			int nBlank   = 0;              // �հ�λ��
			string         strBlank; 
			fprintf(pObsfile,"%c%-2s%3d",                         
				    m_header.SysSclFacList[s_i].szSatlliteSystem,                          
				    " ",                                                
				    obsTypes);
			if(obsTypes <= 13)
			{
				nBlank = 60 - (6 + 4 * obsTypes);
				strBlank.append(nBlank,' ');				
				for(int i = 0;i < obsTypes;i ++)
					fprintf(pObsfile,"%c%3s"," ",m_header.SysSclFacList[s_i].ObsTypelist[i].c_str());
				fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex3_0_MaskString::szSysScaleFac);
			}
			else
			{
				nBlank = 60 - (6 + 4 * nResidue);
				strBlank.append(nBlank,' ');
				for(int i = 0; i < 13; i ++)
					fprintf(pObsfile,"%-1s%3s"," ",m_header.SysSclFacList[s_i].ObsTypelist[i].c_str());
				fprintf(pObsfile,"%-2s%20s\n"," ",Rinex3_0_MaskString::szSysScaleFac);
				for(int n = 1; n < nLine; n ++) 
				{
					fprintf(pObsfile,"%6s"," ");
					for(int j = 0; j < 13; j ++)
						fprintf(pObsfile,"%-1s%3s"," ",m_header.SysSclFacList[s_i].ObsTypelist[13 * n + j].c_str());
					fprintf(pObsfile,"%20s\n",Rinex3_0_MaskString::szSysScaleFac);
				}
				if(nResidue > 0)
				{
					fprintf(pObsfile,"%-10s"," ");
					for(int i = 0; i < nResidue; i ++)
						fprintf(pObsfile,"%-1s%3s"," ",m_header.SysSclFacList[s_i].ObsTypelist[13 * nLine + i].c_str());
				}
				fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex3_0_MaskString::szSysScaleFac);
			}			
		}
		if(m_header.LeapSecond != INT_MAX)
		{
			fprintf(pObsfile,"%6d%54s%20s\n",
				m_header.LeapSecond,
				" ",			
				Rinex3_0_MaskString::szLeapSec);
		}
		if(m_header.SatCount != INT_MAX)
		{
			fprintf(pObsfile,"%6d%54s%20s\n",
					m_header.SatCount,
					" ",			
					Rinex3_0_MaskString::szNumsOfSv);
		}
		fprintf(pObsfile,"%-60s%20s\n"," ",Rinex3_0_MaskString::szEndOfHead);
		// �ļ�ͷ��д���

		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			Rinex3_0_ObsEpoch obsEpoch = m_data[s_i];
			// ʱ����������, 2007/11/06 
			// ���ļ�����ʱ,Ϊ�˱������ 60 ��, ���Ӳ���λ�����
			//DayTime t = obsEpoch.t;
			//t.Second = 0;
			//double sencond = round(obsEpoch.t.Second * 1.0E+7) * 1.0E-7;
			//t = t + sencond;
			//Epoch/SAT--1--32
			fprintf(pObsfile,"%-2s%4d%1s%02d%1s%02d%1s%02d%1s%02d%13.9lf%-2s%1d%3d%-6s%15.12lf%4s\n",
				obsEpoch.szRecordIdentifier,
				obsEpoch.t.year,
				" ",
				obsEpoch.t.month,
				" ",
				obsEpoch.t.day,
				" ",
				obsEpoch.t.hour,
				" ",
				obsEpoch.t.minute,
				obsEpoch.t.second,
				" ",
				obsEpoch.byEpochFlag,
				obsEpoch.EpochSatCount,
				" ",
				obsEpoch.RecClockOffset,				
				" ");		
		    // ����Epoch/SAT�������ɼ��۲�����--------------------
			for(size_t s_j = 0; s_j < obsEpoch.obs.size(); s_j ++)
			{				
				for(Rinex2_1_SatMap::iterator it = obsEpoch.obs[s_j].sobs.begin(); it != obsEpoch.obs[s_j].sobs.end(); ++it)
				{
					fprintf(pObsfile,"%1s%02d",obsEpoch.obs[s_j].szSatlliteSystem,it->first);
					int ObsTypes = 0;
					for(size_t s_k = 0; s_k < m_header.SysObsTypList.size(); s_k ++)
					{
						if(m_header.SysObsTypList[s_k].szSatlliteSystem[0] == obsEpoch.obs[s_j].szSatlliteSystem[0])
						{							
							ObsTypes = m_header.SysObsTypList[s_k].ObsTypCount;
							break;
						}
					}
					for(int i = 0; i < ObsTypes; i++)
					{
						Rinex2_1_ObsDatum obsDatum = it->second[i];						
						if(obsDatum.data != DBL_MAX)
							fprintf(pObsfile,"%14.3f%c%c",							                 
								obsDatum.data,
								obsDatum.lli,
								obsDatum.ssi);
						else
							fprintf(pObsfile,"%-16s"," ");
					}
					fprintf(pObsfile,"\n");
				}
			}			
        }
		fclose(pObsfile);
		return true;
	}
	bool Rinex3_0_ObsFile::write(string strObsfileName_noExp)
	{
		string strObsfileName_all;
		return write(strObsfileName_noExp, strObsfileName_all);
	}    
	// �ӳ������ƣ� rinex3_0T2_1File   
	// ���ܣ�Rinex3.0�Ĺ۲������ļ�ת��ΪRinex2.1�Ĺ۲������ļ� 
	// �������ͣ�obsFile                  : Rinex2.1�ļ� 
	//           szSatlliteSystem         : ����ϵͳ
	//           mark_obstype             : �۲��������������ʶ�����Ϊ0�����������ݣ����Ϊ1���ڵ�������
	// ���룺szSatlliteSystem
	// �����obsFile
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/5/6
	// �汾ʱ�䣺2013/5/6
	// �޸ļ�¼��
	// ������   ����վ�͹���վ�Ĺ۲��������͵ķ��Ų�ͬ��ת��ʱ��Ҫע��
	//          2013��9���Ժ󣬹��ڽ��ջ��Թ۲��������͵ķ��Ž������޸ģ�������ϵı��һ��
	//          C2����B1;C7����B2;C6����B3
	//          ����kzn2��sin1C1����B1;C7����B2;C6����B3
	// �޸��ˣ� �ϱ�����̍
	// ˵��  �� GPS�۲��������ͱ仯����ԭ�������ݴ�������У�mark_obstype = 0����C1C��ӦP1�����ֲ��䡣
	//          �ڵ������ݴ���ʱ��mark_obstype = 1����C1C��ӦC1��C1W��ӦP1��ͬʱ�ڸ�ʽת������ļ����������Ƶ���
	//          ���ݣ��Ա�����˫Ƶ����Ԥ����׶ν���Ƶ��ѡ��
	bool  Rinex3_0_ObsFile::rinex3_0T2_1File(Rinex2_1_ObsFile &obsFile, char szSatlliteSystem[], int mark_obstype)
	{
		strncpy(obsFile.m_header.szRinexVersion, "     2.1            " , 20);
		obsFile.m_header.szFileType[0] = m_header.szFileType[0];
		obsFile.m_header.szSatlliteSystem[0] = szSatlliteSystem[0];
		strncpy(obsFile.m_header.szProgramAgencyName, m_header.szProgramAgencyName, 20);
		strncpy(obsFile.m_header.szProgramName, m_header.szProgramName, 20);
		strncpy(obsFile.m_header.szFileDate, m_header.szFileDate, 20);
		strncpy(obsFile.m_header.szMarkName, m_header.szMarkName, 60);
		strncpy(obsFile.m_header.szMarkNumber, m_header.szMarkNumber, 20);
		strncpy(obsFile.m_header.szObserverName, m_header.szObserverName, 20);
		strncpy(obsFile.m_header.szObserverAgencyName, m_header.szObserverAgencyName, 40);
		strncpy(obsFile.m_header.szRecNumber, m_header.szRecNumber, 20);
		strncpy(obsFile.m_header.szRecType, m_header.szRecType, 20);
		strncpy(obsFile.m_header.szRecVersion, m_header.szRecVersion, 20);
		strncpy(obsFile.m_header.szAntNumber, m_header.szAntNumber, 20);
		strncpy(obsFile.m_header.szAntType, m_header.szAntType, 20);
		obsFile.m_header.ApproxPos = m_header.ApproxPosXYZ;
		int stationID = 0;
		string  strStationName = m_header.szMarkName;
		stationID = string2BDStationId(strStationName);
		if(m_header.AntDeltaHEN.x != DBL_MAX)
			obsFile.m_header.AntOffset = m_header.AntDeltaHEN;
		BYTE            obsTypes = 0;          // �۲���������
		vector<size_t>  obsTypeNumList;        // ��Ҫ�Ĺ۲�����������3.0�۲����������е����
		for(size_t s_i = 0; s_i < m_header.SysObsTypList.size(); s_i ++)
		{
			if(m_header.SysObsTypList[s_i].szSatlliteSystem[0] == szSatlliteSystem[0])
			{
				if(szSatlliteSystem[0] == 'C')// BDS�۲��������͹�������ջ��в��죬��Ҫ�������׷���
				{									
					for(size_t s_j = 0; s_j < m_header.SysObsTypList[s_i].ObsTypelist.size(); s_j ++)
					{
/*						if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C2I") != -1
						|| m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C1I") != -1)	*/		
						if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C2I") != -1)	
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P1);
							//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_P2;
							obsTypeNumList.push_back(s_j);						
						}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L2I") != -1
						//	  ||m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L1I") != -1)
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L2I") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L1);
							//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_L2;
							obsTypeNumList.push_back(s_j);							
						}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S2I") != -1
						//	  ||m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S1I") != -1)
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S2I") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S1);
							obsTypeNumList.push_back(s_j);						
						}//
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C7I") != -1)
						//{
						//	obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P2);
						//	//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_P5;
						//	obsTypeNumList.push_back(s_j);							
						//}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L7I") != -1)
						//{
						//	obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L2);
						//	//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_L5;
						//	obsTypeNumList.push_back(s_j);						
						//}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S7I") != -1)
						//{
						//	obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S2);
						//	obsTypeNumList.push_back(s_j);
						//	//obsTypes ++;
						//}//
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C6I") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P2);
							//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_P1;
							obsTypeNumList.push_back(s_j);							
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L6I") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L2);
							//obsFile.m_header.pbyObsTypeList[obsTypes] = TYPE_OBS_L1;
							obsTypeNumList.push_back(s_j);
						
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S6I") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S2);
							obsTypeNumList.push_back(s_j);						
						}//
					}
					obsFile.m_header.byObsTypes = BYTE(obsFile.m_header.pbyObsTypeList.size());	
					//}
					//else
					//{
					//	printf("�۲�������Դ�������\n");
					//	return false;
					//}//
				}
				else if (szSatlliteSystem[0] == 'G')// GPS�۲�����������ʱ��ȷ��
				{					
					for(size_t s_j = 0; s_j < m_header.SysObsTypList[s_i].ObsTypelist.size(); s_j ++)
					{
						//���²��ֶ����������ݴ����ݲ�����C1W��C5X, L5X, S5X������  ��̍  2016/12/27
						if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C1C") != -1)
						{
							if(mark_obstype == 0)
							{
								obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P1);
								obsTypeNumList.push_back(s_j);	
							}
							else if(mark_obstype == 1)
							{
								obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_C1);
								obsTypeNumList.push_back(s_j);	
							}
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C1W") != -1)
						{
							if(mark_obstype == 1)
							{
								obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P1);
								obsTypeNumList.push_back(s_j);	
							}
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L1C") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L1);
							obsTypeNumList.push_back(s_j);							
						}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("D1C") != -1)  
						//{
						//	obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_D1);
						//	obsTypeNumList.push_back(s_j);							
						//}//
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S1C") != -1)   // �����
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S1);
							obsTypeNumList.push_back(s_j);							
						}//
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C2W") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P2);
							obsTypeNumList.push_back(s_j);							
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L2W") != -1)
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L2);
							obsTypeNumList.push_back(s_j);							
						}			
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("D2W") != -1)  // D
						//{
						//	obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_D2);
						//	obsTypeNumList.push_back(s_j);							
						//}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S2W") != -1)  // �����
						{
							obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S2);
							obsTypeNumList.push_back(s_j);							
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("C5X") != -1)
						{
							if(mark_obstype == 1)
							{
								obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P5);
								obsTypeNumList.push_back(s_j);
							}						
						}
						else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("L5X") != -1)
						{
							if(mark_obstype == 1)
							{
								obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L5);
								obsTypeNumList.push_back(s_j);
							}
						}
						//else if(m_header.SysObsTypList[s_i].ObsTypelist[s_j].find("S5X") != -1)
						//{
						//	if(mark_obstype == 1)
						//	{
						//		obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_S5);
						//		obsTypeNumList.push_back(s_j);
						//	}
						//}
					}
					obsFile.m_header.byObsTypes = BYTE(obsFile.m_header.pbyObsTypeList.size());	
					//}
					//else
					//{
					//	printf("�۲�������Դ�������\n");
					//	return false;
					//}//
				}					
			}
		}
		strncpy(obsFile.m_header.szTimeType, m_header.szTimeSystem, 3);		
		obsFile.m_header.Interval   = m_header.Interval;
		obsFile.m_header.LeapSecond = m_header.LeapSecond;
		obsFile.m_header.tmStart    = m_header.tmStart;
		obsFile.m_header.tmEnd      = m_header.tmEnd;	
		// ͷ�ļ�ת�����

		// ת���۲�����
		BYTE pbySatList[MAX_PRN + 1]; // �����б�
		for(int i = 0; i < MAX_PRN + 1; i++)
			pbySatList[i] = 0;
		for(size_t s_i = 0; s_i < m_data.size();s_i ++)
		{
			bool    bflag  = false;
			size_t  s_sat  = 0;
			for(size_t s_j = 0; s_j < m_data[s_i].obs.size(); s_j ++)
			{
				if(m_data[s_i].obs[s_j].szSatlliteSystem[0] == szSatlliteSystem[0])
				{
					bflag = true;
					s_sat = s_j;
					break;
				}
			}
			if(bflag)
			{
				Rinex2_1_ObsEpoch     obsEpoch;
				obsEpoch.t = m_data[s_i].t;
				obsEpoch.byEpochFlag = m_data[s_i].byEpochFlag;
				obsEpoch.bySatCount = (BYTE)m_data[s_i].obs[s_sat].sobs.size();				
				for(Rinex2_1_SatMap::iterator it = m_data[s_i].obs[s_sat].sobs.begin(); it != m_data[s_i].obs[s_sat].sobs.end(); ++it)
				{
					Rinex2_1_ObsTypeList    obsTypeList;
					for(size_t s_k = 0; s_k < obsTypeNumList.size(); s_k ++)
						obsTypeList.push_back(it->second[obsTypeNumList[s_k]]);
					obsEpoch.obs.insert(Rinex2_1_SatMap::value_type(it->first,obsTypeList));
					pbySatList[it->first] = 1;
				}
				obsFile.m_data.push_back(obsEpoch);
			}
		}
		// �ۺ�ͳ�ƿ��������б�
		obsFile.m_header.pbySatList.clear();
		for(int i = 0; i <= MAX_PRN; i++)
		{
			if(pbySatList[i] == 1)
			{
				obsFile.m_header.pbySatList.push_back(BYTE(i));
			}
		}
		obsFile.m_header.bySatCount = BYTE(obsFile.m_header.pbySatList.size());
		return true;
	}
}