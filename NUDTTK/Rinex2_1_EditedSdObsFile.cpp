#include "Rinex2_1_EditedSdObsFile.hpp"
#pragma warning(disable: 4996)

namespace NUDTTK
{
	Rinex2_1_EditedSdObsFile::Rinex2_1_EditedSdObsFile(void)
	{
	}


	Rinex2_1_EditedSdObsFile::~Rinex2_1_EditedSdObsFile(void)
	{
	}
	void Rinex2_1_EditedSdObsFile::clear()
	{
		m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
		m_data.clear();
	}

	bool Rinex2_1_EditedSdObsFile::isEmpty()
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
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע�� 
	bool Rinex2_1_EditedSdObsFile::cutdata(DayTime t_Begin, DayTime t_End)
	{
		// ȷ�� t_Begin <= t_End
		if( t_Begin - t_End >= 0 || t_End - m_header.tmStart <= 0 || t_Begin - m_header.tmEnd > 0 )
			return false;
		vector<Rinex2_1_EditedSdObsEpoch> obsDataList;
		obsDataList.clear();
		BYTE pbySatList[MAX_PRN];
		for( int i = 0; i < MAX_PRN; i++ )
			pbySatList[i] = 0;
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			Rinex2_1_EditedSdObsEpoch ObsEpoch = m_data[s_i];
			if( ObsEpoch.t - t_Begin >= 0 && ObsEpoch.t - t_End < 0)
			{
				for(Rinex2_1_EditedSdObsSatMap::iterator it = ObsEpoch.editedObs.begin(); it != ObsEpoch.editedObs.end(); ++it)
				{
					it->second.nObsTime = int(obsDataList.size());
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
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/3/13
	// �޸ļ�¼��
	// ��ע��
	int  Rinex2_1_EditedSdObsFile::isValidEpochLine(string strLine, FILE * pEditedSdObsFile)
	{
		// ���漸��������int��, ������Ϊ strLine �ĸ�ʽ�������� sscanf ������������
		DayTime tmEpoch;
		int byEpochFlag = 1;     // 0: ok,  1: power failure between previous and current epoch  > 1: Event flag (2-5) 
		int bySatCount = -1;     // ��ʱ�̿�������(���վ)����
		if(pEditedSdObsFile != NULL)  // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pEditedSdObsFile))
				return 0;
		}
		sscanf(strLine.c_str(), "%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
								&tmEpoch.year,
								&tmEpoch.month,
								&tmEpoch.day,
								&tmEpoch.hour,
								&tmEpoch.minute,
								&tmEpoch.second,
								&byEpochFlag,
								&bySatCount);
		int nFlag = 1;
		if(tmEpoch.month > 12 || tmEpoch.month < 0)
			nFlag = 2;
		if(tmEpoch.day > 31 || tmEpoch.day < 0)
			nFlag = 2;
		if(tmEpoch.hour > 24||tmEpoch.hour < 0)
			nFlag = 2;
		if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
			nFlag = 2;
		if(tmEpoch.second > 60||tmEpoch.second < 0)
			nFlag = 2;
		if(byEpochFlag > 5 || byEpochFlag < 0)
			nFlag = 2;
		if(bySatCount > MAX_PRN_GPS || bySatCount < 0)
			nFlag = 2;
		return nFlag;
	}

	// �ӳ������ƣ� open   
	// ���ܣ��༭��ĵ���۲����ݽ��� 
	// �������ͣ�strEditedSdObsFileName :�༭��ĵ���۲������ļ�·��
	// ���룺strEditedSdObsFileName
	// �����
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע�� 
	bool Rinex2_1_EditedSdObsFile::open(string  strEditedSdObsFileName)
	{
		FILE * pEditedSdObsFile = fopen(strEditedSdObsFileName.c_str(), "r+t");
		if(pEditedSdObsFile == NULL) 
			return false;
		m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
		// ��ʼѭ����ȡÿһ������, ֱ�� END OF HEADER
		int bFlag = 1;
		while(bFlag)
		{// 2008-08-08 �����˵������ļ�ͷ��Ϣ�õ��˲���
			char line[200];
			fgets(line, 100, pEditedSdObsFile);
			string strLineMask = line;
			string strLine     = line;
			strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
			// �޳� \n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n,1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length �������� '\0'
				strLineMask.append(20 - strLineMask.length(), ' ');

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
				sscanf(line,"%14lf%14lf%14lf",
							&m_header.ApproxPos.x,
							&m_header.ApproxPos.y,
							&m_header.ApproxPos.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szAntDeltaHEN)
			{  
				sscanf(line,"%14lf%14lf%14lf",
							&m_header.AntOffset.x,
							&m_header.AntOffset.y,
							&m_header.AntOffset.z);
			}
			else if(strLineMask == Rinex2_1_MaskString::szWaveLenFact)
			{  
				sscanf(line,"%6d%6d%6d",
							&m_header.bL1WaveLengthFact,
							&m_header.bL2WaveLengthFact,
							&m_header.bL5WaveLengthFact);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfFirstObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",
							&m_header.tmStart.year,  
							&m_header.tmStart.month,
							&m_header.tmStart.day,   
							&m_header.tmStart.hour,
							&m_header.tmStart.minute,
							&m_header.tmStart.second);
				strLine.copy(m_header.szTimeType, 3, 48);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTmOfLastObs)
			{  
				sscanf(line,"%6d%6d%6d%6d%6d%12lf",
							&m_header.tmEnd.year,	 
							&m_header.tmEnd.month,
							&m_header.tmEnd.day,	 
							&m_header.tmEnd.hour,
							&m_header.tmEnd.minute,  
							&m_header.tmEnd.second);
			}
			else if(strLineMask == Rinex2_1_MaskString::szInterval)
			{  
				sscanf(line, "%10lf", &m_header.Interval);
			}
			else if(strLineMask == Rinex2_1_MaskString::szLeapSec)
			{  
				sscanf(line, "%6d", &m_header.LeapSecond);
			}
			else if(strLineMask == Rinex2_1_MaskString::szNumsOfSv)
			{  
				sscanf(line, "%6d", &m_header.bySatCount);
			}
			else if(strLineMask == Rinex2_1_MaskString::szTypeOfObserv)
			{  
				sscanf(line, "%6d", &m_header.byObsTypes);
				for(BYTE i = 0; i < m_header.byObsTypes; i++)
				{
					char strObsType[7];
					strLine.copy(strObsType, 6, 6 + i * 6);
					strObsType[6] = '\0';
					m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
				}
			}
			else if(strLineMask == Rinex2_1_MaskString::szPRNNumOfObs)
			{
				// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
				/*
				BYTE bPRN;
				sscanf(line, "%*4c%2d", &bPRN);
				m_header.pbySatList.push_back(bPRN);
				*/
			}
			else if(strLineMask == Rinex2_1_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else 
			{
				// Comment�Ȳ�������
			}
		}
		// �۲�����
		bFlag = true;
		m_data.clear();
		int k = 0;
		char line[300];
		fgets(line, 300, pEditedSdObsFile);
		BYTE pbySatList[MAX_PRN]; // �����б�
		for(int i = 0; i < MAX_PRN; i++)
			pbySatList[i] = 0;

		while(bFlag)
		{
			string strLine = line;
			int nFlag = isValidEpochLine(strLine, pEditedSdObsFile);
			if(nFlag == 0)      // �ļ�ĩβ
			{
				bFlag = false;
			}
			else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
			{
				k++;
				Rinex2_1_EditedSdObsEpoch editedObsEpoch;
				// ����Epoch/SAT -- 1 -- 32
				sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d%14lf%14lf%14lf%14lf%14lf%14lf%14lf",
					                   &editedObsEpoch.t.year,
									   &editedObsEpoch.t.month,
									   &editedObsEpoch.t.day,
									   &editedObsEpoch.t.hour,
									   &editedObsEpoch.t.minute,
									   &editedObsEpoch.t.second,
									   &editedObsEpoch.byEpochFlag,
									   &editedObsEpoch.bySatCount,										  
								       &editedObsEpoch.A_clock, 
									   &editedObsEpoch.A_tropZenithDelayPriori_H,
									   &editedObsEpoch.A_tropZenithDelayPriori_W, 
									   &editedObsEpoch.A_tropZenithDelayEstimate,
									   &editedObsEpoch.A_temperature,
								       &editedObsEpoch.A_humidity,
									   &editedObsEpoch.A_pressure);
				editedObsEpoch.t.year  = yearB2toB4(editedObsEpoch.t.year);
				fgets(line, 300, pEditedSdObsFile);
				sscanf(line,"%*32c%14lf%14lf%14lf%14lf%14lf%14lf%14lf",										  
									   &editedObsEpoch.B_clock,
									   &editedObsEpoch.B_tropZenithDelayPriori_H,
									   &editedObsEpoch.B_tropZenithDelayPriori_W, 
									   &editedObsEpoch.B_tropZenithDelayEstimate,
									   &editedObsEpoch.B_temperature,
									   &editedObsEpoch.B_humidity,
									   &editedObsEpoch.B_pressure);

				editedObsEpoch.editedObs.clear();
				for(int i = 0; i < editedObsEpoch.bySatCount; i++)
				{
					fgets(line, 300, pEditedSdObsFile);
					string strLine = line;
					Rinex2_1_EditedSdObsLine editedObsLine;
					editedObsLine.nObsTime = k - 1; // 2007/07/22 ���
					sscanf(line,"%*1c%2d%8lf%8lf%8lf%8lf",
								&editedObsLine.Id,
								&editedObsLine.Elevation_A,
								&editedObsLine.Azimuth_A,
								&editedObsLine.Elevation_B,
								&editedObsLine.Azimuth_B);

					pbySatList[editedObsLine.Id] = 1; // �ɼ����� PRN ��Ŵ���� 1
					editedObsLine.obsTypeList.clear();
					for(int j = 0; j < m_header.byObsTypes; j++)
					{
						Rinex2_1_EditedObsDatum editedObsDatum;
						char strEditedObsDatum[18];
						strLine.copy(strEditedObsDatum, 17, 35 + j * 17);
						strEditedObsDatum[17] = '\0';
						// �� edited �ļ��Ķ�ȡ���иĽ�, ��ֹ�ո��޷���ȡ, 2008-01-04
						char szObsValue[15];
						char szEditedMark1[2];
						char szEditedMark2[2];
						sscanf(strEditedObsDatum, "%14c%*1c%1c%1c", szObsValue, szEditedMark1, szEditedMark2);
						szObsValue[14]  = '\0';
						szEditedMark1[1] = '\0';
						szEditedMark2[1] = '\0';
						if(strcmp(szObsValue,"              ") == 0)
							editedObsDatum.obs.data = DBL_MAX;
						else
							sscanf(szObsValue, "%14lf", &editedObsDatum.obs.data);
						editedObsDatum.byEditedMark1 = atoi(szEditedMark1);
						editedObsDatum.byEditedMark2 = atoi(szEditedMark2);
						editedObsLine.obsTypeList.push_back(editedObsDatum);
					}
					editedObsEpoch.editedObs.insert(Rinex2_1_EditedSdObsSatMap::value_type(editedObsLine.Id, editedObsLine));
				}
				m_data.push_back(editedObsEpoch);
				fgets(line, 300, pEditedSdObsFile);
			}
			else  // ��Ч������, ����հ���, �ӹ���������
			{   
				fgets(line, 300, pEditedSdObsFile);
			}
		}

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
		// ���³�ʼ�۲�ʱ�̺����۲�ʱ��
		size_t nListNum = m_data.size();
		if(nListNum > 0)
		{
			m_header.tmStart = m_data[0].t;
			m_header.tmEnd   = m_data[nListNum - 1].t;
		}
		fclose(pEditedSdObsFile);
		return true;
	}
	// �ӳ������ƣ� write 
	// ���ܣ����༭��ĵ���۲�����д���ļ� 
	// �������ͣ�strEditedSdObsFileName    : �༭��ĵ���۲������ļ�·��
	// ���룺strEditedSdObsFileName
	// �����
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::write(string strEditedSdObsFileName)
	{
		size_t nlistnum = m_data.size();
		if(nlistnum <= 0)
			return false;
		// 2008/08/08 �����˵���, �ļ�ͷ��Ϣ�õ��˲���
		FILE* pEditedSdObsFile = fopen(strEditedSdObsFileName.c_str(), "w+");
		fprintf(pEditedSdObsFile,"%20s%-20s%20s%20s\n",
							     m_header.szRinexVersion,
							     m_header.szFileType,
							     m_header.szSatlliteSystem,
							     Rinex2_1_MaskString::szVerType);
		fprintf(pEditedSdObsFile,"%-20s%-20s%-20s%20s\n",
							     m_header.szProgramName, 
							     m_header.szProgramAgencyName, 
							     m_header.szFileDate,
							     Rinex2_1_MaskString::szPgmRunDate);
		fprintf(pEditedSdObsFile,"%60s%20s\n",
							     m_header.szMarkName,
							     Rinex2_1_MaskString::szMarkerName);
		// ���OBSERVER / AGENCY, 2008/07/27
		fprintf(pEditedSdObsFile,"%-20s%-40s%20s\n",
							     m_header.szObserverName, 
							     m_header.szObserverAgencyName, 
							     Rinex2_1_MaskString::szObservAgency);
		// ����Ϣ�����, 2008/07/27
		fprintf(pEditedSdObsFile,"%20s%20s%20s%20s\n",
							     m_header.szRecNumber,
							     m_header.szRecType,
							     m_header.szRecVersion,
							     Rinex2_1_MaskString::szRecTypeVers);
		fprintf(pEditedSdObsFile,"%20s%20s%-20s%20s\n",
							     m_header.szAntNumber,
							     m_header.szAntType,
							     " ",
							     Rinex2_1_MaskString::szAntType);
		fprintf(pEditedSdObsFile,"%14.4f%14.4f%14.4f%-18s%20s\n",
							     m_header.ApproxPos.x,
							     m_header.ApproxPos.y,
							     m_header.ApproxPos.z,
							     " ",
							     Rinex2_1_MaskString::szApproxPosXYZ);
		fprintf(pEditedSdObsFile,"%14.4f%14.4f%14.4f%-18s%20s\n",
							     m_header.AntOffset.x,
							     m_header.AntOffset.y,
							     m_header.AntOffset.z,
							     " ",
							     Rinex2_1_MaskString::szAntDeltaHEN);
		fprintf(pEditedSdObsFile,"%6d%6d%6d%-42s%20s\n",
							     m_header.bL1WaveLengthFact,
							     m_header.bL2WaveLengthFact,
							     m_header.bL5WaveLengthFact,
							     " ",
							     Rinex2_1_MaskString::szWaveLenFact);
		fprintf(pEditedSdObsFile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
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
		fprintf(pEditedSdObsFile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
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
			fprintf(pEditedSdObsFile,"%10.3f%-50s%20s\n",
								     m_header.Interval, 
								     " ",
								     Rinex2_1_MaskString::szInterval);
		if(m_header.LeapSecond != INT_MAX)
			fprintf(pEditedSdObsFile,"%6d%-54s%20s\n",
								     m_header.LeapSecond,
								     " ",
								     Rinex2_1_MaskString::szLeapSec);
		fprintf(pEditedSdObsFile,"%6d%-54s%20s\n",
							     m_header.bySatCount,
							     " ",
							     Rinex2_1_MaskString::szNumsOfSv);
		fprintf(pEditedSdObsFile,"%6d",m_header.byObsTypes);
		for(int i = 1; i <= m_header.byObsTypes; i++)
		{// ����CHAMP��ʽҪ��˴���4X2Aֱ�ӵ���Ϊ6A,����μ� glCodeIDToString
			fprintf(pEditedSdObsFile,"%6s",obsId2String(m_header.pbyObsTypeList[i-1]).c_str());
		}
		int nBlank = 60 - (6 + 6 * m_header.byObsTypes);
		string strBlank;
		strBlank.append(nBlank,' ');
		fprintf(pEditedSdObsFile,"%s%20s\n", 
							     strBlank.c_str(),
							     Rinex2_1_MaskString::szTypeOfObserv);

		// ������� Comment ��Ϣ,  2008/07/27
		for( size_t s_i = 0; s_i < m_header.pstrCommentList.size(); s_i++)
		{
			fprintf(pEditedSdObsFile,"%s",m_header.pstrCommentList[s_i].c_str());
		}
		fprintf(pEditedSdObsFile,"%-60s%20s\n",
							     " ",
							     Rinex2_1_MaskString::szEndOfHead);
		
		// �༭��۲�����
		for(size_t s_i = 0; s_i < nlistnum; s_i ++)
		{
			size_t nsatnum = m_data[s_i].editedObs.size();
			fprintf(pEditedSdObsFile," %1d%1d %2d %2d %2d %2d%11.7f  %1d%3d",
								     getIntBit(m_data[s_i].t.year, 1),
								     getIntBit(m_data[s_i].t.year, 0),
								     m_data[s_i].t.month,
								     m_data[s_i].t.day,
								     m_data[s_i].t.hour,
								     m_data[s_i].t.minute,
								     m_data[s_i].t.second,
								     m_data[s_i].byEpochFlag,
								     nsatnum);
			//// ���Ӷ�����Խ����ж�, 2008/09/26
			//if( fabs(m_data[s_i].A_pos.x) >= 1.0E+9
			// || fabs(m_data[s_i].A_pos.y) >= 1.0E+9
			// || fabs(m_data[s_i].A_pos.z) >= 1.0E+9
			// || fabs(m_data[s_i].A_vel.x) >= 1.0E+9
			// || fabs(m_data[s_i].A_vel.y) >= 1.0E+9
			// || fabs(m_data[s_i].A_vel.z) >= 1.0E+9)
			//    fprintf(pEditedSdObsFile,"%3d%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3f\n",
			//	                         m_data[s_i].A_byRAIMFlag,
			//	                         m_data[s_i].A_pdop,
			//	                         m_data[s_i].A_pos.x,
			//						     m_data[s_i].A_pos.y,
			//						     m_data[s_i].A_pos.z,
			//						     m_data[s_i].A_vel.x,
			//						     m_data[s_i].A_vel.y,
			//						     m_data[s_i].A_vel.z,
			//						     m_data[s_i].A_clock);
			//else
		    fprintf(pEditedSdObsFile,"%14.3f%14.4f%14.4f%14.4f%14.3f%14.3f%14.3f\n",				                        
			                         m_data[s_i].A_clock,
									 m_data[s_i].A_tropZenithDelayPriori_H,
			                         m_data[s_i].A_tropZenithDelayPriori_W,
								     m_data[s_i].A_tropZenithDelayEstimate,
								     m_data[s_i].A_temperature,
								     m_data[s_i].A_humidity,
								     m_data[s_i].A_pressure);
			//// ���Ӷ�����Խ����ж�, 2008/09/26
			//if( fabs(m_data[s_i].B_pos.x) >= 1.0E+9
			// || fabs(m_data[s_i].B_pos.y) >= 1.0E+9
			// || fabs(m_data[s_i].B_pos.z) >= 1.0E+9
			// || fabs(m_data[s_i].B_vel.x) >= 1.0E+9
			// || fabs(m_data[s_i].B_vel.y) >= 1.0E+9
			// || fabs(m_data[s_i].B_vel.z) >= 1.0E+9)
			//    fprintf(pEditedSdObsFile,"%-32s%3d%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3f\n",
			//	                         " ",
			//	                         m_data[s_i].B_byRAIMFlag,
			//	                         m_data[s_i].B_pdop,
			//	                         m_data[s_i].B_pos.x,
			//						     m_data[s_i].B_pos.y,
			//						     m_data[s_i].B_pos.z,
			//						     m_data[s_i].B_vel.x,
			//						     m_data[s_i].B_vel.y,
			//						     m_data[s_i].B_vel.z,
			//						     m_data[s_i].B_clock);
			//else
		    fprintf(pEditedSdObsFile,"%-32s%14.3f%14.4f%14.4f%14.4f%14.3f%14.3f%14.3f\n",
			                         " ",				                       
			                         m_data[s_i].B_clock,
									 m_data[s_i].B_tropZenithDelayPriori_H,
			                         m_data[s_i].B_tropZenithDelayPriori_W,
								     m_data[s_i].B_tropZenithDelayEstimate,
								     m_data[s_i].B_temperature,
								     m_data[s_i].B_humidity,
								     m_data[s_i].B_pressure);

			for(Rinex2_1_EditedSdObsSatMap::iterator it = m_data[s_i].editedObs.begin(); it != m_data[s_i].editedObs.end(); ++it)
			{//fprintf(pObsfile,"%c%2d", m_header.getSatSystemChar(),it->first);
				Rinex2_1_EditedSdObsLine editedObsLine = it->second;
				fprintf(pEditedSdObsFile,"%c%2d%8.2f%8.2f%8.2f%8.2f",
									     m_header.getSatSystemChar(),
									     editedObsLine.Id,
									     editedObsLine.Elevation_A,
									     editedObsLine.Azimuth_A,
										 editedObsLine.Elevation_B,
										 editedObsLine.Azimuth_B); 

				for(size_t s_k = 0; s_k < editedObsLine.obsTypeList.size(); s_k ++)
				{
					if(editedObsLine.obsTypeList[s_k].obs.data != DBL_MAX)
						fprintf(pEditedSdObsFile,"%14.3f %1d%1d",
											     editedObsLine.obsTypeList[s_k].obs.data,
											     editedObsLine.obsTypeList[s_k].byEditedMark1,
											     editedObsLine.obsTypeList[s_k].byEditedMark2);
					else
						fprintf(pEditedSdObsFile,"%-14s %1d%1d",
											     " ",
											     editedObsLine.obsTypeList[s_k].byEditedMark1,
											     editedObsLine.obsTypeList[s_k].byEditedMark2);
				}
				fprintf(pEditedSdObsFile, "\n");
			}
		}
		fclose(pEditedSdObsFile);
		return true;
	}
	// �ӳ������ƣ� getEditedObsEpochList   
	// ���ܣ����ݱ༭���ļ����ݣ���� editedObsEpochlist
	// �������ͣ�editedObsEpochlist   : Ԥ���������
	// ���룺
	// �����editedObsEpochlist 
	// ������ 
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::getEditedObsEpochList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist)
	{
		if(isEmpty())
			return false;
		editedObsEpochlist = m_data;
		return true;
	}
	// �ӳ������ƣ� getEditedObsEpochList   
	// ���ܣ����ݱ༭���ļ����ݡ���ʼʱ�� t_Begin ����ֹʱ�� t_End, ��� editedObsEpochlist
	//         ( ע��nObsTimeҪ���±�� )
	// �������ͣ�editedObsEpochlist   : Ԥ��������� [t_Begin, t_End)
	//           t_Begin              : ��ʼʱ��
	//           t_End                : ��ֹʱ��
	// ���룺
	// �����editedObsEpochlist 
	// ������ 
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::getEditedObsEpochList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End)
	{
		if(isEmpty())
			return false;
		editedObsEpochlist.clear();
		int k = 0;
		for(size_t i = 0; i < m_data.size(); i++)
		{
			Rinex2_1_EditedSdObsEpoch editedObsEpoch = m_data[i];
			if(editedObsEpoch.t - t_Begin < 0 )      // ȷ�� editedObsEpoch.T >= T_Begin
				continue;
			else if(editedObsEpoch.t - t_End >= 0 )  // ȷ�� editedObsEpoch.T <  T_End
				break;
			else                                     // T_Begin =< editedObsEpoch.T < T_End
			{
				k++;
				// ������Чʱ����, 20070910 ���
				for(Rinex2_1_EditedSdObsSatMap::iterator it = editedObsEpoch.editedObs.begin(); it != editedObsEpoch.editedObs.end(); ++it)
				{
					it->second.nObsTime = k - 1;
				}
				editedObsEpochlist.push_back(editedObsEpoch);
			}
		}
		if(editedObsEpochlist.size() <= 0)
			return false;
		return true;
	}
	// �ӳ������ƣ� getEditedObsSatList   
	// ���ܣ����ݱ༭���ļ����ݣ���� editedObsSatlist
	// �������ͣ�editedObsSatlist   : Ԥ���������
	// ���룺
	// �����editedObsSatlist 
	// ������ 
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist)
	{
		Rinex2_1_EditedSdObsSat editedObsSatlist_max[MAX_PRN]; 
		for(int i = 0; i < MAX_PRN; i++)
		{
			editedObsSatlist_max[i].Id = i;
			editedObsSatlist_max[i].editedObs.clear();
		}
		// ����ÿ����Ԫ�Ĺ۲�����   /* ��ʱ7������ */
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			// ��ÿ����Ԫ������ÿ��GPS���ǵ�����
			for(Rinex2_1_EditedSdObsSatMap::iterator it = m_data[s_i].editedObs.begin(); it != m_data[s_i].editedObs.end(); ++it)
			{
				BYTE bySatPRN = it->first;
				editedObsSatlist_max[bySatPRN].editedObs.insert(Rinex2_1_EditedSdObsEpochMap::value_type(m_data[s_i].t, it->second));
			}
		}
		// �������п�����������
		editedObsSatlist.clear();
		for(int i = 0; i < MAX_PRN; i++)
		{
			if(editedObsSatlist_max[i].editedObs.size()>0)
			{
				editedObsSatlist.push_back(editedObsSatlist_max[i]);
			}
		}
		return true;
	}
	// �ӳ������ƣ� getEditedObsSatList   
	// ���ܣ����ݱ༭���ļ����ݡ���ʼʱ�� T_Begin ����ֹʱ�� T_End ����� editedObsSatlist
	// �������ͣ�  editedObsSatlist   : Ԥ��������� [t_Begin, t_End)
	//             t_Begin            : ��ʼʱ��
	//             t_End              : ��ֹʱ��
	// ���룺
	// �����editedObsSatlist 
	// ������ 
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End)
	{
		vector<Rinex2_1_EditedSdObsEpoch> editedObsEpochlist;
		if(!getEditedObsEpochList(editedObsEpochlist, t_Begin, t_End))
			return false;
		return getEditedObsSatList(editedObsEpochlist, editedObsSatlist);
	}
	// �ӳ������ƣ� getEditedObsSatList   
	// ���ܣ����� editedObsEpochlist , ��� editedObsSatlist
	// �������ͣ�editedObsEpochlist  : 
	//           editedObsSatlist    : 
	// ���룺editedObsEpochlist
	// �����editedObsSatlist 
	// ������ 
	// ���ԣ�C++
	// �����ߣ������ꡢ�ȵ·�
	// ����ʱ�䣺2012/12/13
	// �汾ʱ�䣺2012/12/13
	// �޸ļ�¼��
	// ��ע��
	bool Rinex2_1_EditedSdObsFile::getEditedObsSatList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist)
	{
		if(editedObsEpochlist.size() <= 0)
			return false;

		Rinex2_1_EditedSdObsSat editedObsSatlist_max[MAX_PRN]; // �����б�
		for(int i = 0; i < MAX_PRN; i++)
		{
			editedObsSatlist_max[i].Id = i;
			editedObsSatlist_max[i].editedObs.clear();
		}
		// ����ÿ����Ԫ�Ĺ۲�����   /* ��ʱ7������ */
		for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
		{
			// ��ÿ����Ԫ������ÿ��GPS���ǵ�����
			for(Rinex2_1_EditedSdObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
			{
				BYTE byID = it->first;
				editedObsSatlist_max[byID].editedObs.insert(Rinex2_1_EditedSdObsEpochMap::value_type(editedObsEpochlist[s_i].t, it->second));
			}
		}
		// �������п�����������
		editedObsSatlist.clear();
		for(int i = 0; i < MAX_PRN; i++)
		{
			if(editedObsSatlist_max[i].editedObs.size() > 0)
			{
				editedObsSatlist.push_back(editedObsSatlist_max[i]);
			}
		}
		return true;
	}
}
