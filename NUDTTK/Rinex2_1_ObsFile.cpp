#include "Rinex2_1_ObsFile.hpp"
#include "constDef.hpp"
#include "MathAlgorithm.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	const char Rinex2_1_MaskString::szVerType[]          = "RINEX VERSION / TYPE";
	const char Rinex2_1_MaskString::szPgmRunDate[]       = "PGM / RUN BY / DATE ";
	const char Rinex2_1_MaskString::szComment[]          = "COMMENT             ";
	const char Rinex2_1_MaskString::szMarkerName[]       = "MARKER NAME         ";
	const char Rinex2_1_MaskString::szMarkerNum[]        = "MARKER NUMBER       ";
	const char Rinex2_1_MaskString::szObservAgency[]     = "OBSERVER / AGENCY   ";
	const char Rinex2_1_MaskString::szRecTypeVers[]      = "REC # / TYPE / VERS ";
	const char Rinex2_1_MaskString::szAntType[]          = "ANT # / TYPE        ";
	const char Rinex2_1_MaskString::szApproxPosXYZ[]     = "APPROX POSITION XYZ ";
	const char Rinex2_1_MaskString::szAntDeltaHEN[]      = "ANTENNA: DELTA H/E/N";
	const char Rinex2_1_MaskString::szWaveLenFact[]      = "WAVELENGTH FACT L1/2";
	const char Rinex2_1_MaskString::szTypeOfObserv[]     = "# / TYPES OF OBSERV ";
	const char Rinex2_1_MaskString::szInterval[]         = "INTERVAL            ";
	const char Rinex2_1_MaskString::szTmOfFirstObs[]     = "TIME OF FIRST OBS   ";
	const char Rinex2_1_MaskString::szTmOfLastObs[]      = "TIME OF LAST OBS    ";
	const char Rinex2_1_MaskString::szLeapSec[]          = "LEAP SECONDS        ";
	const char Rinex2_1_MaskString::szLeapSecGNSS[]      = "LEAP SECONDS GNSS   ";
	const char Rinex2_1_MaskString::szNumsOfSv[]         = "# OF SATELLITES     ";
	const char Rinex2_1_MaskString::szPRNNumOfObs[]      = "PRN / # OF OBS      ";
	const char Rinex2_1_MaskString::szEndOfHead[]        = "END OF HEADER       ";
	const char Rinex2_1_MaskString::szIonAlpha[]         = "ION ALPHA           ";
	const char Rinex2_1_MaskString::szIonBeta[]          = "ION BETA            ";
	const char Rinex2_1_MaskString::szDeltaUTC[]         = "DELTA-UTC: A0,A1,T,W";
	const char Rinex2_1_MaskString::szMetSensorModType[] = "MET SENSOR MOD/TYPE/";
	const char Rinex2_1_MaskString::szMetSensorPos[]     = "MET SENSOR POS XYZH ";
	const char Rinex2_1_MaskString::szDataTypes[]        = "# / TYPES OF DATA   ";
	const char Rinex2_1_MaskString::szAnalysisCenter[]   = "ANALYSIS CENTER     ";
	const char Rinex2_1_MaskString::szTRFofSolnSta[]     = "# OF SOLN STA / TRF ";
	const char Rinex2_1_MaskString::szSolnStaNameNum[]   = "SOLN STA NAME / NUM ";
	const char Rinex2_1_MaskString::szSolnSatsNum[]      = "# OF SOLN SATS      ";
	const char Rinex2_1_MaskString::szPRNList[]          = "PRN LIST            ";

	Rinex2_1_ObsFile::Rinex2_1_ObsFile(void)
	{
	}

	Rinex2_1_ObsFile::~Rinex2_1_ObsFile(void)
	{
	}

	void Rinex2_1_ObsFile::clear()
	{
		m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
		m_data.clear();
	}

	bool Rinex2_1_ObsFile::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}

	// �ӳ������ƣ� cutdata   
	// ���ܣ���������,��������Ϊ[t_Begin, t_End)
	// �������ͣ�t_Begin     : ��ʼʱ��
	//           t_End       : ��ֹʱ��
	// ���룺t_Begin, t_End
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/08/13
	// �汾ʱ�䣺2012/04/11
	// �޸ļ�¼��2012/04/11, ������������޸�, ʹԭ�������Ӧ���ڱ��� Rinex2.1 ���ݶ�ȡ
	// ��ע�� 
	bool Rinex2_1_ObsFile::cutdata(DayTime t_Begin, DayTime t_End)
	{
		// ȷ�� t_Begin <= t_End
		if( t_Begin - t_End >= 0 || t_End - m_header.tmStart <= 0 || t_Begin - m_header.tmEnd > 0 )
			return false;
		vector<Rinex2_1_ObsEpoch> obsDataList;
		obsDataList.clear();
		BYTE pbySatList[MAX_PRN];
		for( int i = 0; i < MAX_PRN; i++ )
			pbySatList[i] = 0;
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			Rinex2_1_ObsEpoch ObsEpoch = m_data[s_i];
			if( ObsEpoch.t - t_Begin >= 0 && ObsEpoch.t - t_End < 0)
			{
				for(Rinex2_1_SatMap::iterator it = ObsEpoch.obs.begin(); it != ObsEpoch.obs.end(); ++it)
				{
					pbySatList[ it->first ] = 1;
				}
				obsDataList.push_back(ObsEpoch);
			}
		}
		size_t nListNum = obsDataList.size();
		if( nListNum <= 0 )
			return false;

		m_data.clear();
		m_data = obsDataList;
		// �����ļ�ͷ��Ϣ, ���³�ʼ�۲�ʱ�̺����۲�ʱ��
		m_header.tmStart = m_data[0].t;
		m_header.tmEnd   = m_data[nListNum - 1].t;
		// �ۺ�ͳ�ƿ��������б�
		m_header.pbySatList.clear();
		for(int i = 0; i < MAX_PRN; i++)
		{
			if(pbySatList[i] == 1)
			{
				m_header.pbySatList.push_back(BYTE(i));
			}
		}
		m_header.bySatCount = BYTE(m_header.pbySatList.size());
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
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/27
	// �汾ʱ�䣺2007/3/27
	// �޸ļ�¼��
	// ��ע�� 
	int  Rinex2_1_ObsFile::isValidEpochLine(string strLine, FILE * pObsfile)
	{
		DayTime tmEpoch;
		// ���漸��������int��, ������ΪstrLine�ĸ�ʽ��������sscanf������������
		int byEpochFlag = -1; // 0: OK; 1: power failure between previous and current epoch  > 1: Event flag ( 2 - 5 ) 
		int bySatCount  = -1; // ��ʱ�̿���GPS���Ǹ���
		if(pObsfile != NULL)  // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pObsfile))
				return 0;
		}

		sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
			                   &tmEpoch.year,
							   &tmEpoch.month,
							   &tmEpoch.day,
							   &tmEpoch.hour,
							   &tmEpoch.minute,
							   &tmEpoch.second,
							   &byEpochFlag,
							   &bySatCount);
		
		if(tmEpoch.month > 12 || tmEpoch.month < 0)
			return 2;
		if(tmEpoch.day >31 || tmEpoch.day < 0)
			return 2;
		if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
			return 2;
		if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
			return 2;
		if(tmEpoch.second > 60 || tmEpoch.second < 0)
			return 2;
		if(byEpochFlag > 5 || byEpochFlag < 0)
			return 2;

		if(bySatCount < 0)
			return 2;

		return 1;
	}

	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strObsfileName : �۲������ļ�·��
	// ���룺strObsfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/27
	// �汾ʱ�䣺2007/3/27
	// �޸ļ�¼��
	// ��ע�� 
	bool Rinex2_1_ObsFile::open(string strObsfileName)
	{
		//clock_t start, finish;
		//double  duration;
		//start = clock();

		if(!isWildcardMatch(strObsfileName.c_str(), "*.*O", true) && !isWildcardMatch(strObsfileName.c_str(), "*.rnx", true)
			 && !isWildcardMatch(strObsfileName.c_str(), "*.*O", true))
		{
			printf(" %s �ļ�����ƥ��!\n", strObsfileName.c_str());
			return false;
		}

		FILE * pObsfile = fopen(strObsfileName.c_str(),"r+t");
		if(pObsfile == NULL) 
			return false;
		m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
		// ��ʼѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
		int bFlag = 1;
		while(bFlag)
		{
			char line[200];
			fgets(line,100,pObsfile);
			string strLineMask = line;
			string strLine     = line;
			strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
			// �޳� \n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n, 1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length ��������'\0'
				strLineMask.append(20-strLineMask.length(),' ');
			
			if(strLineMask == Rinex2_1_MaskString::szVerType)
			{
				strLine.copy(m_header.szRinexVersion, 20, 0);
				strLine.copy(m_header.szFileType, 20, 20);
				strLine.copy(m_header.szSatlliteSystem, 20, 40);
			}
			else if(strLineMask == Rinex2_1_MaskString::szPgmRunDate)
			{
				strLine.copy(m_header.szProgramName, 20, 0);
				strLine.copy(m_header.szProgramAgencyName, 20, 20);
				strLine.copy(m_header.szFileDate, 20, 40);
			}
			else if(strLineMask == Rinex2_1_MaskString::szObservAgency)
			{
				strLine.copy(m_header.szObserverName, 20, 0);
				strLine.copy(m_header.szObserverAgencyName, 40, 20);
			}
			else if(strLineMask == Rinex2_1_MaskString::szMarkerName)
			{
				strLine.copy(m_header.szMarkName, 60, 0);
			}
			else if(strLineMask == Rinex2_1_MaskString::szMarkerNum)
			{
				strLine.copy(m_header.szMarkNumber, 20, 0);
			}
			else if(strLineMask == Rinex2_1_MaskString::szRecTypeVers)
			{
				strLine.copy(m_header.szRecNumber, 20, 0);
				strLine.copy(m_header.szRecType, 20, 20);
				strLine.copy(m_header.szRecVersion, 20, 40);
			}
			else if(strLineMask == Rinex2_1_MaskString::szAntType)
			{
				strLine.copy(m_header.szAntNumber, 20, 0);
				strLine.copy(m_header.szAntType, 20, 20);
			}
			else if(strLineMask == Rinex2_1_MaskString::szApproxPosXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.ApproxPos.x, &m_header.ApproxPos.y, &m_header.ApproxPos.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szAntDeltaHEN)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntOffset.x, &m_header.AntOffset.y, &m_header.AntOffset.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szWaveLenFact)
			{  
				sscanf(line,"%6d%6d%6d",&m_header.bL1WaveLengthFact, &m_header.bL2WaveLengthFact, &m_header.bL5WaveLengthFact);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfFirstObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",&m_header.tmStart.year,  &m_header.tmStart.month,
												   &m_header.tmStart.day,   &m_header.tmStart.hour,
												   &m_header.tmStart.minute,&m_header.tmStart.second);
				strLine.copy(m_header.szTimeType, 3, 48);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfLastObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",&m_header.tmEnd.year,  &m_header.tmEnd.month,
												   &m_header.tmEnd.day,	  &m_header.tmEnd.hour,
												   &m_header.tmEnd.minute,&m_header.tmEnd.second);
			}
			else if(strLineMask == Rinex2_1_MaskString::szInterval)
			{  
				sscanf(line,"%10lf", &m_header.Interval);
			}
			else if(strLineMask == Rinex2_1_MaskString::szLeapSec)
			{  
				sscanf(line,"%6d", &m_header.LeapSecond);
			}
			else if(strLineMask == Rinex2_1_MaskString::szNumsOfSv)
			{  
				sscanf(line,"%6d", &m_header.bySatCount);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTypeOfObserv)
			{  
				sscanf(line,"%6d", &m_header.byObsTypes);				
				int               nline    = m_header.byObsTypes / 9;	// ������
				int               nResidue = m_header.byObsTypes % 9;
				if(m_header.byObsTypes <= 9)
				{
					for(BYTE i = 0; i < m_header.byObsTypes; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));						
					}					
				}
				else
				{
					for(BYTE i = 0; i < 9; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));					
					}
					for(int n = 1; n < nline;n++)
					{// ��ȡ�м������
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < 9; i++)
						{
							char strObsType[7];
							strLine.copy(strObsType, 6, 6 + i * 6);
							strObsType[6] = '\0';
							m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
						}						
					}
					if(nResidue > 0)
					{
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < nResidue; i++)
						{
							char strObsType[7];
							strLine.copy(strObsType, 6, 6 + i * 6);
							strObsType[6] = '\0';
							m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
						}		
					}
				}	
				//sscanf(line,"%6d", &m_header.byObsTypes);
				//for(BYTE i = 0; i < m_header.byObsTypes; i++)
				//{
				//	char strObsType[7];
				//	strLine.copy(strObsType, 6, 6 + i * 6);
				//	strObsType[6] = '\0';
				//	m_header.pbyObsTypeList[i] = string2ObsId(strObsType);
				//}//
			}
			else if(strLineMask == Rinex2_1_MaskString::szPRNNumOfObs)
			{// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
				/*
				static BYTE	nPRN=0;
				nPRN++;
				BYTE bPRN;
				sscanf(line,"%*4c%2d",&bPRN);
				m_header.pbySatList.push_back(bPRN);
				*/
			}
			else if(strLineMask == Rinex2_1_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else 
			{// Comment�Ȳ�������
			}
		}

		// �۲�����
		bFlag = TRUE;
		m_data.clear();
		int k = 0;
		char line[100];
		fgets(line, 100, pObsfile);
		BYTE pbySatList[MAX_PRN_GPS + 1]; // �����б�
		for(int i = 0; i < MAX_PRN_GPS + 1; i++)
			pbySatList[i] = 0;
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
				Rinex2_1_ObsEpoch obsEpoch;
				vector<BYTE>  gpsSatPRNList; 
				// ����Epoch/SAT 1--32
				sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
					                   &obsEpoch.t.year,
									   &obsEpoch.t.month,
								       &obsEpoch.t.day,
									   &obsEpoch.t.hour,
									   &obsEpoch.t.minute,
									   &obsEpoch.t.second,
								       &obsEpoch.byEpochFlag,
									   &obsEpoch.bySatCount);
				obsEpoch.t.year = yearB2toB4(obsEpoch.t.year);
				// 32----68  32X, 12(A1,I2)  68----80  receiver clock offset F12.9
				int nLine = obsEpoch.bySatCount / 12;
				int nResidue = obsEpoch.bySatCount % 12;			
				if(obsEpoch.bySatCount <= 12) // �۲����Ǹ���С�ڵ���12
				{
					for(int i = 0; i < obsEpoch.bySatCount; i++)
					{
						char strObsGPSSat[4];
						int bySatPRN;
						strLine.copy(strObsGPSSat, 3, 32 + i * 3);
						strObsGPSSat[3] = '\0';
						sscanf(strObsGPSSat, "%*1c%2d", &bySatPRN);
						gpsSatPRNList.push_back(BYTE(bySatPRN));
						pbySatList[bySatPRN] = 1; // �ɼ�����PRN��Ŵ����1
					}
				}
				else // �۲����Ǹ�������12
				{
					// ��ȡǰ12������
					for(int i = 0; i < 12; i++)
					{
						char strObsGPSSat[4];
						int bySatPRN;
						strLine.copy(strObsGPSSat, 3 , 32 + i * 3);
						strObsGPSSat[3] = '\0';
						sscanf(strObsGPSSat, "%*1c%2d", &bySatPRN);
						gpsSatPRNList.push_back(BYTE(bySatPRN));
						pbySatList[bySatPRN] = 1; // �ɼ�����PRN��Ŵ����1
					}
					// ��ȡ�м� nLine - 1 ������
					for(int j = 1; j < nLine; j++)
					{
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < 12; i++)
						{
							char strObsGPSSat[4];
							int bySatPRN;
							strLine.copy(strObsGPSSat, 3, 32 + i * 3);
							strObsGPSSat[3] = '\0';
							sscanf(strObsGPSSat, "%*1c%2d", &bySatPRN);
							gpsSatPRNList.push_back(BYTE(bySatPRN));
							pbySatList[bySatPRN] = 1; // �ɼ�����PRN��Ŵ����1
						}
					}
					//��ȡ���nResidue������
					if(nResidue > 0)
					{
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < nResidue; i++)
						{
							char strObsGPSSat[4];
							int bySatPRN;
							strLine.copy(strObsGPSSat, 3, 32 + i * 3);
							strObsGPSSat[3] = '\0';
							sscanf(strObsGPSSat, "%*1c%2d", &bySatPRN);
							gpsSatPRNList.push_back(BYTE(bySatPRN));
							pbySatList[bySatPRN] = 1; // �ɼ�����PRN��Ŵ����1
						}
					}
				}
				obsEpoch.obs.clear();
				// ����Epoch/SAT�������ɼ��۲�����
				for(int m = 0; m < obsEpoch.bySatCount; m++)
				{
					// gpsSatPRNList[m]; // ���Ǻ�
					int nLine =    m_header.byObsTypes / 5;
					int nResidue = m_header.byObsTypes % 5;
					Rinex2_1_ObsTypeList obsTypeList;
					if(m_header.byObsTypes <= 5) // �۲����͵ĸ���С�ڵ���5
					{
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < m_header.byObsTypes; i++)
						{
							Rinex2_1_ObsDatum obsDatum;
							char strObsSatElements[17];
							char strObsValue[15];
							if(size_t(16 * i + 14) <= strLine.length())
							{
								size_t n_copy = strLine.copy(strObsSatElements, 16, 16 * i);
								strObsSatElements[16] = '\0';
								strncpy(strObsValue, strObsSatElements, 14);
								strObsValue[14] = '\0';
								// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
								if(strcmp(strObsValue,"              ") == 0)
									obsDatum.data = DBL_MAX;
								else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
									sscanf(strObsValue, "%14lf", &obsDatum.data);
								//obsDatum.lli = strObsSatElements[14]; // ����champ������ÿ�����ݵ�ĩβֱ����س�, ��ȡ����������Դ�����д�ļ�ʱ������һ�пհ���
								//obsDatum.ssi = strObsSatElements[15];
							}
							else
							{
								obsDatum.data = DBL_MAX;
							}
							obsTypeList.push_back(obsDatum);
						}
					}
					else // �۲����ʹ���5
					{// ��ȡǰ5������
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < 5; i++)
						{
							Rinex2_1_ObsDatum obsDatum;
							char strObsSatElements[17];
							char strObsValue[15];
							if(size_t(16 * i + 14) <= strLine.length())
							{
								size_t n_copy = strLine.copy(strObsSatElements, 16, 16 * i);
								strObsSatElements[16] = '\0';
								strncpy(strObsValue, strObsSatElements, 14);
								strObsValue[14] = '\0';
								// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
								if(strcmp(strObsValue,"              ") == 0)
									obsDatum.data = DBL_MAX;
								else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
									sscanf(strObsValue, "%14lf", &obsDatum.data);							
								//obsDatum.lli = strObsSatElements[14];
								//obsDatum.ssi = strObsSatElements[15];
							}
							else
							{
								obsDatum.data = DBL_MAX;
							}
							obsTypeList.push_back(obsDatum);
						}
						for(int j = 1; j < nLine; j++)
						{// ��ȡ�м� nLine - 1 ������
							fgets(line,100,pObsfile);
							strLine = line;
							for(int i = 0; i < 5; i++)
							{
								Rinex2_1_ObsDatum obsDatum;
								char strObsSatElements[17];
								char strObsValue[15];
								if(size_t(16 * i + 14) <= strLine.length())
								{
									size_t n_copy = strLine.copy(strObsSatElements, 16, 16 * i);
									strObsSatElements[16] = '\0';
									strncpy(strObsValue, strObsSatElements, 14);
									strObsValue[14] = '\0';
									// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
									if(strcmp(strObsValue,"              ") == 0)
										obsDatum.data = DBL_MAX;
									else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
										sscanf(strObsValue, "%14lf", &obsDatum.data);
									//obsDatum.lli = strObsSatElements[14];
									//obsDatum.ssi = strObsSatElements[15];
								}
								else
								{
									obsDatum.data = DBL_MAX;
								}
							    obsTypeList.push_back(obsDatum);
							}
						}
						if(nResidue > 0)
						{// ��ȡ��� nResidue ������
							fgets(line, 100, pObsfile);
							//glStringRaplaceAToB(line,'\n',' ');
							strLine = line;
							for(int i = 0; i < nResidue; i++)
							{
								Rinex2_1_ObsDatum obsDatum;
								char strObsSatElements[17];
								char strObsValue[15];
								if(size_t(16 * i + 14) <= strLine.length())
								{
									size_t n_copy = strLine.copy(strObsSatElements, 16, 16 * i);
									strObsSatElements[16] = '\0';
									strncpy(strObsValue, strObsSatElements, 14);
									strObsValue[14] = '\0';
									// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
									if(strcmp(strObsValue,"              ") == 0)
										obsDatum.data = DBL_MAX;
									else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
										sscanf(strObsValue, "%14lf", &obsDatum.data);
									//obsDatum.lli = strObsSatElements[14];
									//obsDatum.ssi = strObsSatElements[15];
								}
								else
								{
									obsDatum.data = DBL_MAX;
								}
							    obsTypeList.push_back(obsDatum);
							}
						}
					}
					obsEpoch.obs.insert(Rinex2_1_SatMap::value_type(gpsSatPRNList[m], obsTypeList));
				}
				m_data.push_back(obsEpoch);
				fgets(line,100,pObsfile);
			}
			else  
			{// ��Ч�����У�����հ��У��ӹ���������
				fgets(line,100,pObsfile);
			}
		}
		// �ۺ�ͳ�ƿ��������б�
		m_header.pbySatList.clear();
		for(int i = 0; i <= MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				m_header.pbySatList.push_back(BYTE(i));
			}
		}
		m_header.bySatCount = BYTE(m_header.pbySatList.size());
		//���³�ʼ�۲�ʱ�̺����۲�ʱ��
		size_t nListNum = m_data.size();
		if(nListNum > 0)
		{
			m_header.tmStart = m_data[0].t;
			m_header.tmEnd   = m_data[nListNum-1].t;
		}
		fclose(pObsfile);

		//finish = clock();
		//duration = (double)(finish - start) / CLOCKS_PER_SEC;
		//cout<<"�ļ���ȡ��ϣ���ʱ"<<duration<<"�롣"<<endl;
		return true;
	}

	// �ӳ������ƣ� openMixedFile   
	// ���ܣ�������ϵͳ�����ʽ�Ĺ۲�����, ��ȡ����ϵͳ������
	// �������ͣ�strObsfileName : �۲������ļ�·��
	//           cSystem:		: ����ϵͳ��ʶ(Ĭ�� 'G'- GPS ϵͳ)
	// ���룺strObsfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2007/3/27
	// �汾ʱ�䣺2013/6/25
	// �޸ļ�¼��1������ RINEX 2.11 �ļ��Ľ���, �� ��(2013/6/25)
	//           2��Ŀǰ��֧�ֵ�ϵͳ�۲����ݵĳ�ȡ, �� ��(2013/6/25)
	// ��ע��
	bool Rinex2_1_ObsFile::openMixedFile(string  strObsfileName, char cSystem)
	{
		//clock_t start, finish;
		//double  duration;
		//start = clock();
		if(!isWildcardMatch(strObsfileName.c_str(), "*.*O", true))
		{
			printf(" %s �ļ�����ƥ��!\n", strObsfileName.c_str());
			return false;
		}
		FILE * pObsfile = fopen(strObsfileName.c_str(),"r+t");
		if(pObsfile == NULL) 
		{
			return false;
		}
		m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
		// ��ʼѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
		int bFlag = true;
		while(bFlag)
		{
			char line[200];
			fgets(line, 100, pObsfile);
			string strLineMask = line;
			string strLine     = line;
			strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
			// �޳� \n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n, 1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length ��������'\0'
				strLineMask.append(20-strLineMask.length(),' ');
			if(strLineMask == Rinex2_1_MaskString::szVerType)
			{
				strLine.copy(m_header.szRinexVersion, 20, 0);
				strLine.copy(m_header.szFileType, 20, 20);
				strLine.copy(m_header.szSatlliteSystem, 20, 40);
				// ��� Satelllite System Ϊ blank �Ĵ���(2013/6/26)
				if(cSystem == 'C') // 2014/12/11, ����ԭ��ϵͳ���
					sprintf(m_header.szSatlliteSystem, "C                   ");
				else
					sprintf(m_header.szSatlliteSystem, "GPS                 ");
				/*if(m_header.szSatlliteSystem[0] == ' ')
				{
					m_header.szSatlliteSystem[0] = 'G';
				}*/
			}
			else if(strLineMask == Rinex2_1_MaskString::szPgmRunDate)
			{
				strLine.copy(m_header.szProgramName, 20, 0);
				strLine.copy(m_header.szProgramAgencyName, 20, 20);
				strLine.copy(m_header.szFileDate, 20, 40);
			}
			else if(strLineMask == Rinex2_1_MaskString::szObservAgency)
			{
				strLine.copy(m_header.szObserverName, 20, 0);
				strLine.copy(m_header.szObserverAgencyName, 40, 20);
			}
			else if(strLineMask == Rinex2_1_MaskString::szMarkerName)
			{
				strLine.copy(m_header.szMarkName, 60, 0);
			}
			else if(strLineMask == Rinex2_1_MaskString::szMarkerNum)
			{
				strLine.copy(m_header.szMarkNumber, 20, 0);
			}
			else if(strLineMask == Rinex2_1_MaskString::szRecTypeVers)
			{
				strLine.copy(m_header.szRecNumber, 20, 0);
				strLine.copy(m_header.szRecType, 20, 20);
				strLine.copy(m_header.szRecVersion, 20, 40);
			}
			else if(strLineMask == Rinex2_1_MaskString::szAntType)
			{
				strLine.copy(m_header.szAntNumber, 20, 0);
				strLine.copy(m_header.szAntType, 20, 20);
			}
			else if(strLineMask == Rinex2_1_MaskString::szApproxPosXYZ)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.ApproxPos.x, &m_header.ApproxPos.y, &m_header.ApproxPos.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szAntDeltaHEN)
			{  
				sscanf(line,"%14lf%14lf%14lf",&m_header.AntOffset.x, &m_header.AntOffset.y, &m_header.AntOffset.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szWaveLenFact)
			{  
				sscanf(line,"%6d%6d%6d",&m_header.bL1WaveLengthFact, &m_header.bL2WaveLengthFact, &m_header.bL5WaveLengthFact);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfFirstObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",&m_header.tmStart.year,  &m_header.tmStart.month,
					&m_header.tmStart.day,   &m_header.tmStart.hour,
					&m_header.tmStart.minute,&m_header.tmStart.second);
				strLine.copy(m_header.szTimeType, 3, 48);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfLastObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",&m_header.tmEnd.year,  &m_header.tmEnd.month,
					&m_header.tmEnd.day,	  &m_header.tmEnd.hour,
					&m_header.tmEnd.minute,&m_header.tmEnd.second);
			}
			else if(strLineMask == Rinex2_1_MaskString::szInterval)
			{  
				sscanf(line,"%10lf", &m_header.Interval);
			}
			else if(strLineMask == Rinex2_1_MaskString::szLeapSec)
			{  
				sscanf(line,"%6d", &m_header.LeapSecond);
			}
			else if(strLineMask == Rinex2_1_MaskString::szNumsOfSv)
			{  
				sscanf(line,"%6d", &m_header.bySatCount);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTypeOfObserv)
			{  
				sscanf(line,"%6d", &m_header.byObsTypes);				
				int               nline    = m_header.byObsTypes / 9;	// ������
				int               nResidue = m_header.byObsTypes % 9;
				if(m_header.byObsTypes <= 9)
				{
					for(BYTE i = 0; i < m_header.byObsTypes; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));						
					}					
				}
				else
				{
					for(BYTE i = 0; i < 9; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));					
					}
					for(int n = 1; n < nline;n++)
					{// ��ȡ�м������
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < 9; i++)
						{
							char strObsType[7];
							strLine.copy(strObsType, 6, 6 + i * 6);
							strObsType[6] = '\0';
							m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
						}						
					}
					if(nResidue > 0)
					{
						fgets(line,100,pObsfile);
						strLine = line;
						for(BYTE i = 0; i < nResidue; i++)
						{
							char strObsType[7];
							strLine.copy(strObsType, 6, 6 + i * 6);
							strObsType[6] = '\0';
							m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
						}		
					}
				}			

				//sscanf(line,"%6d", &m_header.byObsTypes);
				//for(BYTE i = 0; i < m_header.byObsTypes; i++)
				//{
				//	char strObsType[7];
				//	strLine.copy(strObsType, 6, 6 + i * 6);
				//	strObsType[6] = '\0';
				//	m_header.pbyObsTypeList[i] = string2ObsId(strObsType);
				//}//
			}
			else if(strLineMask == Rinex2_1_MaskString::szPRNNumOfObs)
			{
				// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
			}
			else if(strLineMask == Rinex2_1_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else 
			{
				// Comment�Ȳ�������
			}
		}// end of while (�ļ�ͷ��ȡ����)
		double RinexVersion = 0.0;
		sscanf(m_header.szRinexVersion, "%9lf", &RinexVersion);
		if(RinexVersion != 2.0 && RinexVersion != 2.1 && RinexVersion != 2.11)
		{
			printf("��ʱ������ RINEX 2.11 ���ϵİ汾��");
			return false;
		}
		// �۲����ݽ���
		bFlag = TRUE;
		m_data.clear();
		char line[100];
		fgets(line, 100, pObsfile);
		BYTE pbySatList[MAX_PRN_GPS + 1]; // ���� GPS �����б�
		for(int i = 0; i < MAX_PRN_GPS + 1; i++)
		{
			pbySatList[i] = 0;
		}
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
				Rinex2_1_ObsEpoch obsEpoch;
				vector<BYTE>  SatPRNList;		// ���� PRN �б�
				SatPRNList.clear();
				// ����Epoch/SAT 1--32
				sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
					&obsEpoch.t.year,
					&obsEpoch.t.month,
					&obsEpoch.t.day,
					&obsEpoch.t.hour,
					&obsEpoch.t.minute,
					&obsEpoch.t.second,
					&obsEpoch.byEpochFlag,
					&obsEpoch.bySatCount);	// bySatCount ����� GPS ���Ǹ����޸�(2013/6/25)
				// �ݲ����� EpochFlag > 1 �� Event (2013/6/26)
				if(obsEpoch.byEpochFlag > 1)
				{
					fgets(line, 100, pObsfile);
					continue;
				}
				obsEpoch.t.year = yearB2toB4(obsEpoch.t.year);
				// 32----68  32X, 12(A1,I2)  68----80  receiver clock offset F12.9	
				char strObsSat[4] = "   ";						// �洢����ϵͳ + PRN (����'G01')
				strObsSat[3] = '\0';
				char SatType = ' ';								// �洢����ϵͳ����(����'G'- GPS ϵͳ)
				int bySatPRN = 0;								// ���� PRN ��
				BYTE bySatCount_GPS = 0;								// �洢��ǰ��Ԫ���� GPS ���Ǹ���
				vector<char> pcSatSysFlag;
				pcSatSysFlag.resize(obsEpoch.bySatCount);
				int nLine    = obsEpoch.bySatCount / 12;
				int nResidue = obsEpoch.bySatCount % 12;
				// ����������С�ڵ��� 12 
				if(obsEpoch.bySatCount <= 12)
				{
					for(int i = 0; i < obsEpoch.bySatCount; i++)
					{
						// ��ȡ���� PRN �б�
						strLine.copy(strObsSat, 3, 32 + i * 3);
						sscanf(strObsSat, "%1c%2d", &SatType, &bySatPRN);
						// ��� SatType = ' ' �Ĵ���(2013/6/26)
						if(SatType == ' ')
						{
							SatType = 'G';
						}
						pcSatSysFlag[i] = SatType;
						SatPRNList.push_back(bySatPRN);
						if(SatType == cSystem)
						{
							pbySatList[bySatPRN] = 1;   // �ɼ�����PRN��Ŵ����1
							bySatCount_GPS++;
						}
						else
						{
							continue;
						}
					}
				}
				// �������������� 12
				else
				{
					// ��ȡǰ 12 ������
					for(int i = 0; i < 12; i++)
					{
						// ��ȡ�����б�
						strLine.copy(strObsSat, 3, 32 + i * 3);
						sscanf(strObsSat, "%1c%2d", &SatType, &bySatPRN);
						// ��� SatType = ' ' �Ĵ���(2013/6/26)
						if(SatType == ' ')
						{
							SatType = 'G';
						}
						pcSatSysFlag[i] = SatType;
						SatPRNList.push_back(bySatPRN);
						if(SatType == cSystem)
						{
							pbySatList[bySatPRN] = 1;   // �ɼ�����PRN��Ŵ����1
							bySatCount_GPS++;
						}
						else
						{
							continue;
						}
					}
					// ��ȡ�м� nLine - 1 ������
					for(int j = 1; j < nLine; j++)
					{
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < 12; i++)
						{
							// ��ȡ�����б�
							strLine.copy(strObsSat, 3, 32 + i * 3);
							sscanf(strObsSat, "%1c%2d", &SatType, &bySatPRN);
							// ��� SatType = ' ' �Ĵ���(2013/6/26)
							if(SatType == ' ')
							{
								SatType = 'G';
							}
							pcSatSysFlag[12 * j + i] = SatType;
							SatPRNList.push_back(bySatPRN);
							if(SatType == cSystem)
							{
								pbySatList[bySatPRN] = 1;   // �ɼ�����PRN��Ŵ����1
								bySatCount_GPS++;
							}
							else
							{
								continue;
							}
						}
					}
					//��ȡ��� nResidue ������
					if(nResidue > 0)
					{
						fgets(line, 100, pObsfile);
						strLine = line;
						for(int i = 0; i < nResidue; i++)
						{
							// ��ȡ�����б�
							strLine.copy(strObsSat, 3, 32 + i * 3);
							sscanf(strObsSat, "%1c%2d", &SatType, &bySatPRN);
							// ��� SatType = ' ' �Ĵ���(2013/6/26)
							if(SatType == ' ')
							{
								SatType = 'G';
							}
							pcSatSysFlag[12 * nLine + i] = SatType;
							SatPRNList.push_back(bySatPRN);
							if(SatType == cSystem)
							{
								pbySatList[bySatPRN] = 1;   // �ɼ�����PRN��Ŵ����1
								bySatCount_GPS++;
							}
							else
							{
								continue;
							}
						}
					}
				}// end of else(�ɹ۲����������� 12)
				// ��ʼ��ȡ��ǰ��Ԫ�Ĺ۲�����
				obsEpoch.obs.clear();
				// ����Epoch/SAT�������ɼ��۲�����
				for(int m = 0; m < obsEpoch.bySatCount; m++)
				{
					nLine    = m_header.byObsTypes / 5;
					nResidue = m_header.byObsTypes % 5;
					Rinex2_1_ObsTypeList obsTypeList;
					obsTypeList.clear();
					// �۲����͵ĸ���С�ڵ��� 5
					if(m_header.byObsTypes <= 5) 
					{
						fgets(line, 100, pObsfile);
						if(pcSatSysFlag[m] != cSystem)
						{
							continue;	// ����ǰ���ǲ��� GPS ���ǣ��Թ�����
						}
						strLine = line;
						// �޳��س��� '/n'(2013/6/26)
						size_t nPos_n = strLine.find('\n');
						strLine.erase(nPos_n, 1);
						for(int i = 0; i < m_header.byObsTypes; i++)
						{
							Rinex2_1_ObsDatum obsDatum;
							char strObsSatElements[17];		// F14.3 + I1 + I1
							char strObsValue[15];			// F14.3
							strObsSatElements[14] = ' ';	// I1
							strObsSatElements[15] = ' ';	// I1
							if(size_t(16 * i + 14) <= strLine.length())	// ��ֹ�հ�����δ����ֱ�ӻ���
							{
								strLine.copy(strObsSatElements, 16, 16 * i);
								strObsSatElements[16] = '\0';
								strncpy(strObsValue, strObsSatElements, 14);
								strObsValue[14] = '\0';
								// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
								if(strcmp(strObsValue,"              ") == 0)
								{
									obsDatum.data = DBL_MAX;
								}
								else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
								{
									sscanf(strObsValue, "%14lf", &obsDatum.data);
								}
								obsDatum.lli = strObsSatElements[14]; // ����champ������ÿ�����ݵ�ĩβֱ����س�, ��ȡ����������Դ�����д�ļ�ʱ������һ�пհ���
								obsDatum.ssi = strObsSatElements[15];
							}
							else
							{
								obsDatum.data = DBL_MAX;
							}
							obsTypeList.push_back(obsDatum);
						}
					}// end of if(�۲����͵ĸ���С�ڵ��� 5)
					// �۲����͵ĸ������� 5
					else 
					{
						// ����ǰ���ǲ��� GPS ���ǣ��Թ������ݿ�
						if(pcSatSysFlag[m] != cSystem)
						{
							for(int l = 1; l <= nLine; l++)
							{
								fgets(line, 100, pObsfile);
							}
							if(nResidue > 0)
							{
								fgets(line, 100, pObsfile);
							}
							continue;	
						}
						// ��ȡǰ5������
						fgets(line, 100, pObsfile);
						strLine = line;
						// �޳��س��� '/n'(2013/6/26)
						size_t nPos_n = strLine.find('\n');
						strLine.erase(nPos_n, 1);
						for(int i = 0; i < 5; i++)
						{
							Rinex2_1_ObsDatum obsDatum;
							char strObsSatElements[17];		// F14.3 + I1 + I1
							char strObsValue[15];			// F14.3
							strObsSatElements[14] = ' ';	// I1
							strObsSatElements[15] = ' ';	// I1
							if(size_t(16 * i + 14) <= strLine.length())
							{
								strLine.copy(strObsSatElements, 16, 16 * i);
								strObsSatElements[16] = '\0';
								strncpy(strObsValue, strObsSatElements, 14);
								strObsValue[14] = '\0';
								// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
								if(strcmp(strObsValue,"              ") == 0)
								{
									obsDatum.data = DBL_MAX;
								}
								else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
								{
									sscanf(strObsValue, "%14lf", &obsDatum.data);
								}
								obsDatum.lli = strObsSatElements[14];
								obsDatum.ssi = strObsSatElements[15];
							}
							else
							{
								obsDatum.data = DBL_MAX;
							}
							obsTypeList.push_back(obsDatum);
						}
						// ��ȡ�м� nLine - 1 ������
						for(int j = 1; j < nLine; j++)
						{
							fgets(line, 100, pObsfile);
							strLine = line;
							// �޳��س��� '/n'(2013/6/26)
							size_t nPos_n = strLine.find('\n');
							strLine.erase(nPos_n, 1);
							for(int i = 0; i < 5; i++)
							{
								Rinex2_1_ObsDatum obsDatum;
								char strObsSatElements[17];		// F14.3 + I1 + I1
								char strObsValue[15];			// F14.3
								strObsSatElements[14] = ' ';	// I1
								strObsSatElements[15] = ' ';	// I1
								if(size_t(16 * i + 14) <= strLine.length())
								{
									strLine.copy(strObsSatElements, 16, 16 * i);
									strObsSatElements[16] = '\0';
									strncpy(strObsValue, strObsSatElements, 14);
									strObsValue[14] = '\0';
									// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
									if(strcmp(strObsValue,"              ") == 0)
									{
										obsDatum.data = DBL_MAX;
									}
									else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
									{
										sscanf(strObsValue, "%14lf", &obsDatum.data);
									}
									obsDatum.lli = strObsSatElements[14];
									obsDatum.ssi = strObsSatElements[15];
								}
								else
								{
									obsDatum.data = DBL_MAX;
								}
								obsTypeList.push_back(obsDatum);
							}
						}
						// ��ȡ��� nResidue ������
						if(nResidue > 0)
						{
							fgets(line, 100, pObsfile);
							strLine = line;
							// �޳��س��� '/n'(2013/6/26)
							size_t nPos_n = strLine.find('\n');
							strLine.erase(nPos_n, 1);
							for(int i = 0; i < nResidue; i++)
							{
								Rinex2_1_ObsDatum obsDatum;
								char strObsSatElements[17];		// F14.3 + I1 + I1
								char strObsValue[15];			// F14.3
								strObsSatElements[14] = ' ';	// I1
								strObsSatElements[15] = ' ';	// I1
								if(size_t(16 * i + 14) <= strLine.length())
								{
									strLine.copy(strObsSatElements, 16, 16 * i);
									strObsSatElements[16] = '\0';
									strncpy(strObsValue, strObsSatElements, 14);
									strObsValue[14] = '\0';
									// ��ӶԿո���Ч���ݵ�У�飬��������Чʱһ����Կո���д
									if(strcmp(strObsValue,"              ") == 0)
									{
										obsDatum.data = DBL_MAX;
									}
									else // 2007-4-29 ��ֹ��ȡ�������Ч����I1I1  F14.3I1I1
									{
										sscanf(strObsValue, "%14lf", &obsDatum.data);
									}
									obsDatum.lli = strObsSatElements[14];
									obsDatum.ssi = strObsSatElements[15];
								}
								else
								{
									obsDatum.data = DBL_MAX;
								}
								obsTypeList.push_back(obsDatum);
							}
						}
					}// end of else(�۲����͵ĸ������� 5)
					obsEpoch.obs.insert(Rinex2_1_SatMap::value_type(SatPRNList[m], obsTypeList));
				}// end of for(��������)
				obsEpoch.bySatCount = bySatCount_GPS;			// ���ĵ�ǰ��Ԫ����������(��ͳ�� GPS ���Ǹ���)
				if(bySatCount_GPS > 0) //û�й۲����ݲ������һ��Ԫ�������꣬2014/12/03/ 
					m_data.push_back(obsEpoch);
				fgets(line, 100, pObsfile);
				// �ͷŶ�̬�ڴ�ռ�
				//delete[] pcSatSysFlag;
			}
			else  
			{// ��Ч�����У�����հ��У��ӹ���������
				fgets(line, 100, pObsfile);
			}
		}// end of while(bFlag)
		// �ۺ�ͳ�ƿ��������б�
		m_header.pbySatList.clear();
		for(int i = 0; i <= MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				m_header.pbySatList.push_back(BYTE(i));
			}
		}
		m_header.bySatCount = BYTE(m_header.pbySatList.size());
		// ���³�ʼ�۲�ʱ�̺����۲�ʱ�̣���ȫ���������Ϣ���� ����2013/7/7
		size_t nListNum = m_data.size();
		fclose(pObsfile);
		if(nListNum > 1)   // ��ֹ�۲�����ֻ��һ����������֣������꣬20140929
		{
			m_header.tmStart = m_data[0].t;
			m_header.tmEnd   = m_data[nListNum-1].t;
			if(m_header.Interval == DBL_MAX)
			{
				m_header.Interval = m_data[1].t - m_data[0].t;
			}
			m_header.szSatlliteSystem[0] = cSystem;
			return true;
		}
		else 
			return false;
		//finish = clock();
		//duration = (double)(finish - start) / CLOCKS_PER_SEC;
		//cout<<"�ļ���ȡ��ϣ���ʱ"<<duration<<"�롣"<<endl;
		//return true;
	}

	// �ӳ������ƣ� write   
	// ���ܣ����۲�����д���ļ� 
	// �������ͣ�strObsfileName_noExp    : �۲������ļ�·��(��׺�������Զ�����, ���벻����)
	//           strObsfileName_all      : �����Ĺ۲������ļ�·��
	// ���룺strObsfileName
	// �����strObsfileName_all
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/27
	// �汾ʱ�䣺2007/3/27
	// �޸ļ�¼��
	// ��ע�� 
	bool Rinex2_1_ObsFile::write(string strObsfileName_noExp,string& strObsfileName_all)
	{
		if(isEmpty())
			return false;
		// д�ļ�ͷ������ champ �ı�׼�ļ�ͷ��д
		int n1  = getIntBit(m_header.tmStart.year, 0);
		int n10 = getIntBit(m_header.tmStart.year, 1);
		char strFileExp[5];
		sprintf(strFileExp,".%1d%1do",n10,n1);
		strObsfileName_all = strObsfileName_noExp + strFileExp;
		FILE* pObsfile=fopen(strObsfileName_all.c_str(), "w+");
		// �� RINEX �ļ�����Ϊ M (Mixed)��Ŀǰ����ȡ GPS ���ݣ�д�ļ�ʱ�� 'M'��дΪ'G'(2013/6/28)
		if(m_header.getSatSystemChar() == 'M')
		{
			m_header.szSatlliteSystem[0] = 'G';
		}
		fprintf(pObsfile,"%20s%-20s%-20s%20s\n", 
			              m_header.szRinexVersion,
						  m_header.szFileType,
						  m_header.szSatlliteSystem,
						  Rinex2_1_MaskString::szVerType);
		fprintf(pObsfile,"%-20s%-20s%-20s%20s\n",
			              m_header.szProgramName, 
						  m_header.szProgramAgencyName, 
						  m_header.szFileDate,
						  Rinex2_1_MaskString::szPgmRunDate);
		fprintf(pObsfile,"%-60s%20s\n",
			              m_header.szMarkName,
						  Rinex2_1_MaskString::szMarkerName);
		// ���OBSERVER / AGENCY��2008-07-27
		fprintf(pObsfile,"%-20s%-40s%20s\n",
			              m_header.szObserverName, 
						  m_header.szObserverAgencyName, 
						  Rinex2_1_MaskString::szObservAgency);
		// ����Ϣ�������2008-07-27
		// fprintf(pObsfile,"%20s%-40s%20s\n",m_header.szMarkNumber," ",Rinex2_1_MaskString::szMarkerNum);
		fprintf(pObsfile,"%20s%20s%20s%20s\n",
			              m_header.szRecNumber,
						  m_header.szRecType,
						  m_header.szRecVersion,
						  Rinex2_1_MaskString::szRecTypeVers);
		fprintf(pObsfile,"%20s%20s%-20s%20s\n",
			              m_header.szAntNumber,
						  m_header.szAntType,
						  " ",
						  Rinex2_1_MaskString::szAntType);

		if(m_header.ApproxPos.x != DBL_MAX)
		{// ���� 3.0 -> 2.0 ʱ���� DBL_MAX
			fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
							  m_header.ApproxPos.x,
							  m_header.ApproxPos.y,
							  m_header.ApproxPos.z,					 
							  " ",
							  Rinex2_1_MaskString::szApproxPosXYZ);
		}

		fprintf(pObsfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
			              m_header.AntOffset.x,
						  m_header.AntOffset.y,
						  m_header.AntOffset.z,
						  " ",
						  Rinex2_1_MaskString::szAntDeltaHEN);
		if(m_header.bL1WaveLengthFact != 100)
			fprintf(pObsfile,"%6d%6d%6d%-42s%20s\n",
			              m_header.bL1WaveLengthFact,
						  m_header.bL2WaveLengthFact,
						  m_header.bL5WaveLengthFact,
			              " ",Rinex2_1_MaskString::szWaveLenFact);
		fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
			              m_header.tmStart.year,  
						  m_header.tmStart.month,
						  m_header.tmStart.day,   
						  m_header.tmStart.hour,
						  m_header.tmStart.minute,
						  m_header.tmStart.second,		
						  " ",
						  m_header.szTimeType,
						  " ",
						  Rinex2_1_MaskString::szTmOfFirstObs);
		fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
			              m_header.tmEnd.year,  
						  m_header.tmEnd.month,
						  m_header.tmEnd.day,   
						  m_header.tmEnd.hour,
						  m_header.tmEnd.minute,
						  m_header.tmEnd.second,
						  " ",
						  m_header.szTimeType,
						  " ",
						  Rinex2_1_MaskString::szTmOfLastObs);
		if(m_header.Interval != DBL_MAX)
			fprintf(pObsfile,"%10.3f%-50s%20s\n",
			              m_header.Interval,
						  " ",
						  Rinex2_1_MaskString::szInterval);
		if(m_header.LeapSecond != INT_MAX)
			fprintf(pObsfile,"%6d%-54s%20s\n",
			              m_header.LeapSecond,
						  " ",
						  Rinex2_1_MaskString::szLeapSec);
		fprintf(pObsfile,"%6d%-54s%20s\n",
			              m_header.bySatCount,
						  " ",
						  Rinex2_1_MaskString::szNumsOfSv);
		fprintf(pObsfile,"%6d",m_header.byObsTypes);  
		//for(int i = 1; i <= m_header.byObsTypes; i++)  // ����۲��������͸����������ֵ��д�ļ���������Ҫ�޸ģ�2013.09.10��������
		                                                 // ���޸ģ�20140902
		//{// ���� champ ��ʽҪ��˴���4X2Aֱ�ӵ���Ϊ6A, ����μ� gpsObsId2String
			//fprintf(pObsfile,"%6s", obsId2String(m_header.pbyObsTypeList[i-1]).c_str());
		int obsTypes = (int)m_header.byObsTypes;
		int nLine    = obsTypes / 9;  // ������
	    int nResidue = obsTypes % 9;  // ����
		int nBlank   = 0;              // �հ�λ��
		string         strBlank; 			
		if(obsTypes <= 9)
		{
			nBlank = 60 - (6 + 6 * obsTypes);
			strBlank.append(nBlank,' ');				
			for(int i = 0;i < obsTypes;i ++)
				fprintf(pObsfile,"%6s",obsId2String(m_header.pbyObsTypeList[i]).c_str());
			fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex2_1_MaskString::szTypeOfObserv);
		}
		else
		{
			nBlank = 60 - (6 + 6 * nResidue);
			strBlank.append(nBlank,' ');
			for(int i = 0; i < 9; i ++)
				fprintf(pObsfile,"%6s",obsId2String(m_header.pbyObsTypeList[i]).c_str());
			fprintf(pObsfile,"%20s\n", Rinex2_1_MaskString::szTypeOfObserv);
			for(int n = 1; n < nLine; n ++) 
			{
				fprintf(pObsfile,"%6s"," ");
				for(int i = 0; i < 9; i ++)
					fprintf(pObsfile,"%6s",obsId2String(m_header.pbyObsTypeList[9 * n + i]).c_str());
			fprintf(pObsfile,"%20s\n", Rinex2_1_MaskString::szTypeOfObserv);
			}
			if(nResidue > 0)
			{
				fprintf(pObsfile,"%-6s"," ");
				for(int i = 0; i < nResidue; i ++)
					fprintf(pObsfile,"%6s",obsId2String(m_header.pbyObsTypeList[9 * nLine + i]).c_str());
			}
			fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex2_1_MaskString::szTypeOfObserv);
		}
		////}
		//int nBlank = 60 - (6 + 6 * m_header.byObsTypes);//
		//string strBlank;
		//strBlank.append(nBlank,' ');
		//fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex2_1_MaskString::szTypeOfObserv);

		// ����Ϣ�����, 2008/07/27
		//for(size_t s_i = 1; s_i <= m_header.pbySatList.size(); s_i++)
		//{
		//	fprintf(pObsfile,"%-3sG%2d%-54s%20s\n"," ",m_header.pbySatList[s_i-1]," ",Rinex2_1_MaskString::szPRNNumOfObs);
		//}	

		// ������� Comment ��Ϣ,  2008/07/27
		for( size_t s_i = 0; s_i < m_header.pstrCommentList.size(); s_i++)
		{
			fprintf(pObsfile,"%s",m_header.pstrCommentList[s_i].c_str());
		}

		fprintf(pObsfile,"%-60s%20s\n"," ",Rinex2_1_MaskString::szEndOfHead);
		// �ļ�ͷ��д���

		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			Rinex2_1_ObsEpoch obsEpoch = m_data[s_i];
			// ʱ����������, 2007/11/06 
			// ���ļ�����ʱ,Ϊ�˱������ 60 ��, ���Ӳ���λ�����
			DayTime t = obsEpoch.t;
			t.second = 0;
			double sencond = Round(obsEpoch.t.second * 1.0E+7) * 1.0E-7;
			t = t + sencond;
			//Epoch/SAT--1--32
			fprintf(pObsfile," %1d%1d %2d %2d %2d %2d%11.7f  %1d%3d",
				              getIntBit(obsEpoch.t.year, 1),
							  getIntBit(obsEpoch.t.year, 0),
					          t.month,
							  t.day,
							  t.hour,
							  t.minute,
							  t.second,
							  obsEpoch.byEpochFlag,
							  obsEpoch.bySatCount);
			//32----68  32X, 12(A1,I2)  68----80  receiver clock offset F12.9
			int nLine    = obsEpoch.bySatCount / 12;
			int nResidue = obsEpoch.bySatCount % 12;
			if(obsEpoch.bySatCount <= 12) 
			{// �۲����Ǹ���С�ڵ���12
				for(Rinex2_1_SatMap::iterator it = obsEpoch.obs.begin(); it != obsEpoch.obs.end(); ++it)
				{
					fprintf(pObsfile,"%c%02d", m_header.getSatSystemChar(),it->first);
				}
				nBlank = 36 - (3 * obsEpoch.bySatCount); // Ϳ���� 2011/06/13
				strBlank.erase(0, strBlank.length());
				strBlank.append(nBlank, ' ');
				fprintf(pObsfile, "%s\n", strBlank.c_str());
			}
			else // �۲����Ǹ�������12
			{// ��ȡǰ12������
				Rinex2_1_SatMap::iterator it = obsEpoch.obs.begin();
				for(int j = 0; j < 12; j++)
				{
					fprintf(pObsfile,"%c%02d", m_header.getSatSystemChar(),it->first);
					++it;
				}
				fprintf(pObsfile,"\n");
				for(int j = 1; j < nLine; j++)
				{// ��ȡ�м�nLine-1������
					fprintf(pObsfile, "%-32s", " "); // ÿһ��ǰ����32X
					for(int ii = 0; ii < 12; ii++)
					{
						fprintf(pObsfile,"%c%02d", m_header.getSatSystemChar(),it->first);
						++it;
					}
					fprintf(pObsfile,"\n");
				}
				if(nResidue > 0)
				{// ��ȡ���nResidue������
					fprintf(pObsfile,"%-32s"," ");
					for(int ii = 0; ii < nResidue; ii++)
					{
						fprintf(pObsfile,"%c%02d", m_header.getSatSystemChar(),it->first);
						++it;
					}
					nBlank = 36 - 3 * nResidue; // ����հ׸���
					strBlank.erase(0, strBlank.length());
					strBlank.append(nBlank,' ');
					fprintf(pObsfile,"%s\n", strBlank.c_str());
				}
			}
			// ����Epoch/SAT�������ɼ��۲�����--------------------
			for(Rinex2_1_SatMap::iterator it = obsEpoch.obs.begin(); it != obsEpoch.obs.end(); ++it)
			{
				int nLine    = m_header.byObsTypes / 5;
				int nResidue = m_header.byObsTypes % 5;
				if(m_header.byObsTypes <= 5) // �۲����͵ĸ���С�ڵ���5
				{
					for(int j = 0; j < m_header.byObsTypes; j++)
					{
						Rinex2_1_ObsDatum obsDatum = it->second[j];
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
				else // �۲����ʹ���5
				{// ��ȡǰ5������
					for(int j = 0; j < 5; j++)
					{
						Rinex2_1_ObsDatum obsDatum = it->second[j];
						if(obsDatum.data != DBL_MAX)
							fprintf(pObsfile,"%14.3f%c%c",
							                 obsDatum.data,
											 obsDatum.lli,
											 obsDatum.ssi);
						else
							fprintf(pObsfile, "%-16s", " ");
					}
					fprintf(pObsfile, "\n");
					for(int j = 1; j < nLine; j++)
					{// ��ȡ�м�nLine-1������
						for(int ii = 0; ii < 5; ii++)
						{
							Rinex2_1_ObsDatum obsDatum = it->second[j * 5 + ii];
							if(obsDatum.data != DBL_MAX)
								fprintf(pObsfile,"%14.3f%c%c",
								                 obsDatum.data,
												 obsDatum.lli,
												 obsDatum.ssi);
							else
								fprintf(pObsfile,"%-16s"," ");
						}
						fprintf(pObsfile, "\n");
					}
					if(nResidue > 0)
					{// ��ȡ���nResidue������
						for(int j = 0; j < nResidue; j++)
						{
							Rinex2_1_ObsDatum obsDatum = it->second[nLine * 5 + j];
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
		}
		fclose(pObsfile);
		return true;
	}

	bool Rinex2_1_ObsFile::write(string strObsfileName_noExp)
	{
		string strObsfileName_all;
		return write(strObsfileName_noExp, strObsfileName_all);
	}

	// �ӳ������ƣ� downSampling   
	// ���ܣ�ԭʼ�۲����ݵĽ��������� 
	// �������ͣ�nSampleSpan    : ��������Ĳ������, nSampleSpan Ӧ����ԭʼ�����ʵı���
	//           flag_method    : �������, 0: ����ƽ������
	//           flag_Int       :����ʱ���Ƿ�������ֵ 
	// ���룺nSampleSpan, flag_method,flag_Int
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/11/06
	// �汾ʱ�䣺2015/04/29
	// �޸ļ�¼��2021/9/2  ���Ӳ���ʱ���Ƿ�������ֵ�ж�
	// ��ע��
	bool Rinex2_1_ObsFile::downSampling(int nSampleSpan, int flag_method,bool flag_Int)
	{
			if(isEmpty())
				return false;
			if(flag_method == 0)
			{
				vector<Rinex2_1_ObsEpoch> obsEpochList;
				obsEpochList.clear();
				BYTE pbySatList[MAX_PRN_GPS];
				GPST t0 = m_data[0].t;
				for( int i = 0; i < MAX_PRN_GPS; i++)
					pbySatList[i] = 0;
				for(size_t s_i = 0; s_i < m_data.size(); s_i++)
				{
						// ����һ��ʱ�� t������ nSampleSpan ��С��������һ����Чʱ�� ti��
				// ͨ���ж� t �� ti �Ľӽ��̶ȣ������� t �Ƿ�������ҵ������
					if(flag_Int)
					{
						double integer  = Round(m_data[s_i].t.second / nSampleSpan);      // ��������, floor����ΪRound, 20150603
						double fraction = m_data[s_i].t.second - integer * nSampleSpan;   // �ӽ��̶��ж�
						if(fabs(fraction) < 0.05) // �������� 0.5 -> 0.05, ����Ӧ10Hz���ݵĽ�����, 20100612
						{// Ѱ�ҵ������
							obsEpochList.push_back(m_data[s_i]);
							for(Rinex2_1_SatMap::iterator it = m_data[s_i].obs.begin(); it != m_data[s_i].obs.end(); ++it)
							{
								pbySatList[it->first] = 1;
							}
						}
					}
					else
					{
					//double t_integer = floor(m_data[s_i].t.second);
					//double integer  = Round(t_integer / nSampleSpan);      // ��������, floor����ΪRound, 20150603
					//double fraction = t_integer - integer * nSampleSpan;   // �ӽ��̶��ж�
					//if(fabs(fraction) < 0.05) // �������� 0.5 -> 0.05, ����Ӧ10Hz���ݵĽ�����, 20100612
					//{// Ѱ�ҵ������
					//	obsEpochList.push_back(m_data[s_i]);
					//	for(Rinex2_1_SatMap::iterator it = m_data[s_i].obs.begin(); it != m_data[s_i].obs.end(); ++it)
					//	{
					//		pbySatList[it->first] = 1;
					//	}
					//}
						double t = m_data[s_i].t - t0;
						double integer  = floor(t / nSampleSpan);      // ��������
						double fraction = t - integer * nSampleSpan;   // �ӽ��̶��ж�
						if(fabs(fraction) < 0.05) // �������� 0.5 -> 0.05, ����Ӧ10Hz���ݵĽ�����, 20100612
						{// Ѱ�ҵ������
							obsEpochList.push_back(m_data[s_i]);
							for(Rinex2_1_SatMap::iterator it = m_data[s_i].obs.begin(); it != m_data[s_i].obs.end(); ++it)
							{
								pbySatList[it->first] = 1;
							}
						}
					}
				}
				if(obsEpochList.size() == 0)
					return false;
				m_data.clear();
				m_data = obsEpochList;
				// �����ļ�ͷ��Ϣ, ���³�ʼ�۲�ʱ�̺����۲�ʱ��
				m_header.tmStart = m_data[0].t;
				m_header.tmEnd   = m_data[m_data.size() - 1].t;
				// �ۺ�ͳ�ƿ��������б�
				m_header.pbySatList.clear();
				for(int i = 0; i < MAX_PRN_GPS; i++)
				{
					if(pbySatList[i] == 1)
					{
						m_header.pbySatList.push_back(BYTE(i));
					}
				}
				m_header.bySatCount = BYTE(m_header.pbySatList.size());
				m_header.Interval   = double(nSampleSpan);

				// �ļ���������
				DayTime T_Now;
				T_Now.Now();
				sprintf(m_header.szFileDate,"%04d-%02d-%02d %02d:%02d:%02d",T_Now.year, T_Now.month,  T_Now.day,
																			T_Now.hour, T_Now.minute, int(T_Now.second));
				sprintf(m_header.szProgramName,"%-20s","downSampling");
				sprintf(m_header.szProgramAgencyName,"%-20s","NUDT");
			
				// ע����
				m_header.pstrCommentList.clear();
				char szComment[100];
				sprintf(szComment,"%3d%-57s%20s\n", nSampleSpan,"s downSampling, with unsmoothed method",Rinex2_1_MaskString::szComment);
				m_header.pstrCommentList.push_back(szComment);
			}
			return true;
	}
}